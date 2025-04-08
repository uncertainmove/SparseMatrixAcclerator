#include "core.h"
#include "parameter.h"
#include "structure.h"
#include "util.h"
#include "debug.h"
#include <bits/stdc++.h>

#if DEBUG
    debug_RD_ACTIVE_VERTEX debug_M1;
#endif

using namespace std;

extern int clk;
extern int rst_rd;
extern int VTX_NUM;
extern int v_updated;
extern int MAX_ITERATION;

int iteration_end_flag;
int active_vertex_num = 0;

void RD_ACTIVE_VERTEX_SINGLE(
        int Core_ID,
        int Backend_Active_V_ID, int Backend_Active_V_Updated, int Backend_Active_V_DValid,
        int Backend_Iteration_End, int Backend_Iteration_End_DValid,
        int NextStage_Full,

        int *Active_V_ID, int *Active_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid,
        int *Iteration_ID);

void RD_ACTIVE_VERTEX(
        int Backend_Active_V_ID[], int Backend_Active_V_Updated[], int Backend_Active_V_DValid[],
        int Backend_Iteration_End[], int Backend_Iteration_End_DValid[],
        int NextStage_Full[],

        int *Active_V_ID, int *Active_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid,
        int *Iteration_ID) {
    for (int i = 0; i < CORE_NUM; ++i) {
        RD_ACTIVE_VERTEX_SINGLE(
            i,
            Backend_Active_V_ID[i], Backend_Active_V_Updated[i], Backend_Active_V_DValid[i],
            Backend_Iteration_End[i], Backend_Iteration_End_DValid[i],
            NextStage_Full[i],

            &Active_V_ID[i], &Active_V_DValid[i],
            &Iteration_End[i], &Iteration_End_DValid[i],
            &Iteration_ID[i]);
    }
}

void RD_ACTIVE_VERTEX_SINGLE(
    int Core_ID,
    int Backend_Active_V_ID, int Backend_Active_V_Updated, int Backend_Active_V_DValid,
    int Backend_Iteration_End, int Backend_Iteration_End_DValid,
    int NextStage_Full,

    int *Active_V_ID, int *Active_V_DValid,
    int *Iteration_End, int *Iteration_End_DValid,
    int *Iteration_ID) {

    ///local data structures. static int -> reg, int -> wire
    //control signal
    static int iteration_id[CORE_NUM];
    static int iteration_end[CORE_NUM];

    //pull mode: active vertex map, each element represents 32 vertices
    static bitmap vis_bitmap[CORE_NUM][2][BitMap_Compressed_NUM + 1];
    static int vis_bitmap_index[CORE_NUM];
    static bitmap vis_bitmap_now[CORE_NUM];

    static int iteration_flag[CORE_NUM]; //用来判断传进来的iteration end是否是连续的（无效的）

    static int send_v = 0;

    #if DEBUG
        debug_M1._iteration_id = iteration_id;
        debug_M1._iteration_end = iteration_end;
        debug_M1._vis_bitmap = vis_bitmap;
        debug_M1._vis_bitmap_index = vis_bitmap_index;
        debug_M1._vis_bitmap_now = vis_bitmap_now;
    #endif

    int now_bitmap_id = iteration_id[Core_ID] % 2;
    int next_bitmap_id = (iteration_id[Core_ID] + 1) % 2;

    iteration_end_flag = (iteration_id[Core_ID] == MAX_ITERATION);

    int backend_v_bitmap_id1 = (Backend_Active_V_ID / CORE_NUM) / BitMap_Compressed_Length;
    int backend_v_bitmap_id2 = (Backend_Active_V_ID / CORE_NUM) % BitMap_Compressed_Length;

    ///using wires to find an active vertex for pull mode
    int max_length = BitMap_Compressed_Length;
    int pull_active_bitmap[max_length], pull_active_bitmap_index[max_length];

    for (int i = 0; i < max_length; ++ i) {
        pull_active_bitmap[i] = vis_bitmap_now[Core_ID].v[i];
        pull_active_bitmap_index[i] = i;
    }
    while (max_length > 1) {
        max_length /= 2;
        for (int i = 0; i < max_length; ++ i) { // 从vis_bitmap当前页选出位置最小的标记为1的点的位置
            pull_active_bitmap_index[i] = (pull_active_bitmap[2 * i] == 1) ? pull_active_bitmap_index[2 * i] : pull_active_bitmap_index[2 * i + 1];
            pull_active_bitmap[i] = pull_active_bitmap[2 * i] | pull_active_bitmap[2 * i + 1]; // 1 说明存在标记为1的点
        }
    }

    int active_vertex_index = pull_active_bitmap_index[0];
    int active_vertex_bitmap = pull_active_bitmap[0];

    //Send active vertex to the next pipeline stage
    if (rst_rd) {

        *Active_V_ID = 0;
        *Active_V_DValid = 0;
    } else {
        if (NextStage_Full) {
            *Active_V_ID = 0;
            *Active_V_DValid = 0;
        } else {
            //pull does not end and any vertex in current bitmap line is not visited
            // pull 有效 当前页存在激活点
            if (vis_bitmap_index[Core_ID] < BitMap_Compressed_NUM && active_vertex_bitmap == 1) {
                *Active_V_ID = (vis_bitmap_index[Core_ID] * BitMap_Compressed_Length + active_vertex_index) * CORE_NUM + Core_ID;
                *Active_V_DValid = (*Active_V_ID < VTX_NUM);
                if (*Active_V_DValid) {
                    send_v++;
                    // cout << "M01: send task, clk: " << clk << ", id: " << *Active_V_ID << endl;
                }
            } else {
                *Active_V_ID = 0;
                *Active_V_DValid = 0;
            }
        }
    }

    ///iteration end management
    if (rst_rd) {
        *Iteration_End = 0;
        *Iteration_End_DValid = 0;
    }
    else {
        if (iteration_end[Core_ID]) {
            *Iteration_End = 1;
            *Iteration_End_DValid = 1;
        }
        else {
            *Iteration_End = 0;
            *Iteration_End_DValid = 0;
        }
    }

    ///iteration_id management
    if (rst_rd) {
        iteration_id[Core_ID] = 0;
        iteration_end[Core_ID] = 0;
    } else {
        if (iteration_end[Core_ID] == 1) {
            if (Backend_Iteration_End && Backend_Iteration_End_DValid && iteration_flag[Core_ID]) {
                if (Core_ID == 0) {
                    cout << "iteration " << iteration_id[Core_ID] << " end at clk " << clk <<
                    " send vertex " << send_v << " edge updated " << v_updated <<
                    " active vertex " << active_vertex_num << endl;
                    if (active_vertex_num == 0) iteration_end_flag = 1;
                    active_vertex_num = 0;
                }
                send_v = 0;
                v_updated = 0;
                iteration_id[Core_ID]++;
                iteration_end[Core_ID] = 0;
            }
        } else {
            if (vis_bitmap_index[Core_ID] >= BitMap_Compressed_NUM) {
                iteration_end[Core_ID] = 1;
            }
        }
    }

    if (rst_rd) {
        vis_bitmap_now[Core_ID] = vis_bitmap[Core_ID][now_bitmap_id][0];
        vis_bitmap_index[Core_ID] = 0;

        for (int i = 0; i < BitMap_Compressed_NUM; ++i) {
            for (int j = 0; j < BitMap_Compressed_Length; ++j) {
                vis_bitmap[Core_ID][now_bitmap_id][i].v[j] = 1;
                vis_bitmap[Core_ID][next_bitmap_id][i].v[j] = 1;
            }
        }
    } else {
        if (vis_bitmap_index[Core_ID] < BitMap_Compressed_NUM) {
            if (active_vertex_bitmap == 0) {
                for (int i = 0; i < BitMap_Compressed_Length; ++ i) {
                    vis_bitmap[Core_ID][now_bitmap_id][vis_bitmap_index[Core_ID]].v[i] = 0;
                }
                vis_bitmap_now[Core_ID] = vis_bitmap[Core_ID][now_bitmap_id][vis_bitmap_index[Core_ID] + 1];
                vis_bitmap_index[Core_ID]++;
            } else {
                //any vertex in current bitmap line is scheduled
                if (!NextStage_Full) {
                    vis_bitmap_now[Core_ID].v[active_vertex_index] = 0;
                }
            }
        } else if (vis_bitmap_index[Core_ID] >= BitMap_Compressed_NUM &&
                    Backend_Iteration_End && Backend_Iteration_End_DValid &&
                    iteration_flag[Core_ID]) {
            vis_bitmap_now[Core_ID] = vis_bitmap[Core_ID][next_bitmap_id][0];
            vis_bitmap_index[Core_ID] = 0;
        }
        //update the bitmap
        if (Backend_Active_V_DValid) {
            // filter visited vertex in push mode
            if (vis_bitmap[Core_ID][next_bitmap_id][backend_v_bitmap_id1].v[backend_v_bitmap_id2] && !Backend_Active_V_Updated) {
                active_vertex_num -= 1;
                // cout << "clk: " << clk << ", M01 delete active vertex, id: " << Backend_Active_V_ID << endl;
            } else if (!vis_bitmap[Core_ID][next_bitmap_id][backend_v_bitmap_id1].v[backend_v_bitmap_id2] && Backend_Active_V_Updated) {
                active_vertex_num += 1;
                // cout << "clk: " << clk << ", M01 insert active vertex, id: " << Backend_Active_V_ID << endl;
            }
            vis_bitmap[Core_ID][next_bitmap_id][backend_v_bitmap_id1].v[backend_v_bitmap_id2] = Backend_Active_V_Updated;
        }
    }

    if (rst_rd) {
        iteration_flag[Core_ID] = 1;
    }
    else {
        if(Backend_Iteration_End && Backend_Iteration_End_DValid){
            iteration_flag[Core_ID] = 0;
        }
        else{
            iteration_flag[Core_ID] = 1;
        }
    }

    // assign
    *Iteration_ID = iteration_id[Core_ID];
}
