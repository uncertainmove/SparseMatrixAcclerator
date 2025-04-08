#include "core.h"
#include "parameter.h"
#include "util.h"
#include "debug.h"
#include <bits/stdc++.h>

#if DEBUG
    debug_RD_ACTIVE_VERTEX_OFFSET debug_M2;
#endif

extern int rst_rd;
extern int clk;

using namespace std;

void Read_Active_Vertex_Offset_Single(
        int Core_ID,
        int Front_Active_V_ID, int Front_Active_V_DValid,
        int Front_Iteration_End, int Front_Ieration_End_DValid, int Front_Iteration_ID,
        int NextStage_Full,

        int *Stage_Full,
        int *Active_V_ID, int *RD_Active_V_Offset_Addr, int *RD_Active_V_Value_Addr, int *Active_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID);

void Read_Active_Vertex_Offset(
        int Front_Active_V_ID[], int Front_Active_V_DValid[],
        int Front_Iteration_End[], int Front_Ieration_End_DValid[], int Front_Iteration_ID[],
        int NextStage_Full[],

        int *Stage_Full,
        int *Active_V_ID, int *RD_Active_V_Offset_Addr, int *RD_Active_V_Value_Addr, int *Active_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID) {

    for (int i = 0; i < CORE_NUM; ++ i) {
        Read_Active_Vertex_Offset_Single(
            i,
            Front_Active_V_ID[i], Front_Active_V_DValid[i],
            Front_Iteration_End[i], Front_Ieration_End_DValid[i], Front_Iteration_ID[i],
            NextStage_Full[i],

            &Stage_Full[i],
            &Active_V_ID[i], &RD_Active_V_Offset_Addr[i], &RD_Active_V_Value_Addr[i], &Active_V_DValid[i],
            &Iteration_End[i], &Iteration_End_DValid[i], &Iteration_ID[i]);
    }
}

void Read_Active_Vertex_Offset_Single(
        int Core_ID,
        int Front_Active_V_ID, int Front_Active_V_DValid,
        int Front_Iteration_End, int Front_Iteration_End_DValid, int Front_Iteration_ID,
        int NextStage_Full,

        int *Stage_Full,
        int *Active_V_ID, int *RD_Active_V_Offset_Addr, int *RD_Active_V_Value_Addr, int *Active_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID) {

    static queue<int> v_id_buffer[CORE_NUM];
    static int v_buffer_empty[CORE_NUM], v_buffer_full[CORE_NUM];

    #if DEBUG
        debug_M2._v_id_buffer = v_id_buffer;
    #endif

    v_buffer_empty[Core_ID] = (v_id_buffer[Core_ID].size() == 0);
    v_buffer_full[Core_ID] = (v_id_buffer[Core_ID].size() >= FIFO_SIZE);
    ///active vertex management
    if (rst_rd) {
        clear_buffer(v_id_buffer[Core_ID]);
    } else {
        if (Front_Active_V_DValid) {
            v_id_buffer[Core_ID].push(Front_Active_V_ID);
        }
    }

    if (rst_rd) {
        *Active_V_ID = 0;
        *RD_Active_V_Offset_Addr = 0;
        *RD_Active_V_Value_Addr = 0;
        *Active_V_DValid = 0;
    } else {
        if (NextStage_Full || v_buffer_empty[Core_ID]) {
            *Active_V_ID = 0;
            *RD_Active_V_Offset_Addr = 0;
            *RD_Active_V_Value_Addr = 0;
            *Active_V_DValid = 0;
        } else {
            *Active_V_ID = v_id_buffer[Core_ID].front();
            *RD_Active_V_Offset_Addr = v_id_buffer[Core_ID].front() / CORE_NUM;
            *RD_Active_V_Value_Addr = v_id_buffer[Core_ID].front() / CORE_NUM;
            *Active_V_DValid = 1;
            
            v_id_buffer[Core_ID].pop();
            // cout << "M02: send task, clk: " << clk << ", id: " << *Active_V_ID << ", off_addr: " << *RD_Active_V_Offset_Addr <<
                // ", value_addr: " << *RD_Active_V_Value_Addr << endl;
        }
    }

    static int end_ct[CORE_NUM];
    ///iteration end management, front_active_v_dvalid is used to avoid early end
    if (!rst_rd && Front_Iteration_End && Front_Iteration_End_DValid && v_buffer_empty[Core_ID] && !Front_Active_V_DValid) {
        if (end_ct[Core_ID] >= WAIT_END_DELAY) {
            *Iteration_End = 1;
            *Iteration_End_DValid = 1;
            *Iteration_ID = Front_Iteration_ID;
        } else {
            end_ct[Core_ID]++;
            *Iteration_End = 0;
            *Iteration_End_DValid = 0;
            *Iteration_ID = Front_Iteration_ID;
        }
    }
    else {
        end_ct[Core_ID] = 0;
        *Iteration_End = 0;
        *Iteration_End_DValid = 0;
        *Iteration_ID = Front_Iteration_ID;
    }


    //assign stage_full = v_id_bufer_full
    *Stage_Full = (v_id_buffer[Core_ID].size() >= FIFO_SIZE);
}