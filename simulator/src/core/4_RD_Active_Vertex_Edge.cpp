#include "core.h"
#include "parameter.h"
#include "structure.h"
#include "util.h"
#include "debug.h"

#if DEBUG
    debug_RD_Active_Vertex_Edge debug_M4;
#endif

extern int clk;
extern int rst_rd;

using namespace std;

void RD_Active_Vertex_Edge_Single(
        int Core_ID,
        int Front_Active_V_ID, int Front_Active_V_DValid,
        int Front_Iteration_End, int Front_Iteration_End_DValid, int Front_Iteration_ID,
        int Front_Active_V_LOffset, int Front_Active_V_Roffset, int Front_Offset_DValid,
        V_VALUE_TP Front_Active_V_Value, int Front_Active_V_Value_Valid,
        int CombineStage_Full,

        int *Stage_Full,
        int *Rd_HBM_Edge_Addr, BitVector_16 *Rd_HBM_Edge_Mask,
        int *HBM_Active_V_ID, V_VALUE_TP *HBM_Active_V_Value, int *Rd_HBM_Edge_Valid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID);

void RD_Active_Vertex_Edge(
        int Front_Active_V_ID[], int Front_Active_V_DValid[], 
        int Front_Iteration_End[], int Front_Iteration_End_DValid[], int Front_Iteration_ID[],
        int Front_Active_V_LOffset[], int Front_Active_V_Roffset[], int Front_Offset_DValid[],
        V_VALUE_TP Front_Active_V_Value[], int Front_Active_V_Value_Valid[],
        int CombineStage_Full[],

        int *Stage_Full,
        int *Rd_HBM_Edge_Addr, BitVector_16 *Rd_HBM_Edge_Mask,
        int *HBM_Active_V_ID, V_VALUE_TP *HBM_Active_V_Value, int *Rd_HBM_Edge_Valid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID) {

    for (int i = 0; i < CORE_NUM; ++ i) {
        RD_Active_Vertex_Edge_Single(
            i,
            Front_Active_V_ID[i], Front_Active_V_DValid[i],
            Front_Iteration_End[i], Front_Iteration_End_DValid[i], Front_Iteration_ID[i],
            Front_Active_V_LOffset[i], Front_Active_V_Roffset[i], Front_Offset_DValid[i],
            Front_Active_V_Value[i], Front_Active_V_Value_Valid[i],
            CombineStage_Full[i],

            &Stage_Full[i],
            &Rd_HBM_Edge_Addr[i], &Rd_HBM_Edge_Mask[i],
            &HBM_Active_V_ID[i], &HBM_Active_V_Value[i], &Rd_HBM_Edge_Valid[i],
            &Iteration_End[i], &Iteration_End_DValid[i], &Iteration_ID[i]);
    }
}


void RD_Active_Vertex_Edge_Single(
        int Core_ID,
        int Front_Active_V_ID, int Front_Active_V_DValid,
        int Front_Iteration_End, int Front_Iteration_End_DValid, int Front_Iteration_ID,
        int Front_Active_V_LOffset, int Front_Active_V_ROffset, int Front_Offset_DValid,
        V_VALUE_TP Front_Active_V_Value, int Front_Active_V_Value_Valid,
        int CombineStage_Full,

        int *Stage_Full,
        int *Rd_HBM_Edge_Addr, BitVector_16 *Rd_HBM_Edge_Mask,
        int *HBM_Active_V_ID, V_VALUE_TP *HBM_Active_V_Value, int *Rd_HBM_Edge_Valid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID) {

    static queue<int> v_id_buffer[CORE_NUM];
    static queue<V_VALUE_TP> v_value_buffer[CORE_NUM];
    static queue<int> v_loffset_buffer[CORE_NUM], v_roffset_buffer[CORE_NUM];
    static int v_buffer_empty[CORE_NUM];
    static int o_buffer_empty[CORE_NUM];
    static int value_buffer_empty[CORE_NUM];

    static int now_loffset[CORE_NUM];

    #if DEBUG
        debug_M4._v_id_buffer = v_id_buffer;
        debug_M4._v_value_buffer = v_value_buffer;
        debug_M4._v_loffset_buffer = v_loffset_buffer;
        debug_M4._v_roffset_buffer = v_roffset_buffer;
        debug_M4._now_loffset = now_loffset;
    #endif

    v_buffer_empty[Core_ID] = (v_id_buffer[Core_ID].size() == 0);
    value_buffer_empty[Core_ID] = (v_value_buffer[Core_ID].size() == 0);
    o_buffer_empty[Core_ID] = (v_loffset_buffer[Core_ID].size() == 0);

    if (rst_rd) {
        now_loffset[Core_ID] = 1000000000;

        for (int i = 0; i < CACHELINE_LEN; ++i) {
            Rd_HBM_Edge_Mask->flag[i] = 0;
        }
        *HBM_Active_V_ID = 0;
        *HBM_Active_V_Value = 0;
        *Rd_HBM_Edge_Addr = 0;
        *Rd_HBM_Edge_Valid = 0;
    } else {
        if (o_buffer_empty[Core_ID] || value_buffer_empty[Core_ID]) {
            now_loffset[Core_ID] = 1000000000;

            for (int i = 0; i < CACHELINE_LEN; ++i) {
                Rd_HBM_Edge_Mask->flag[i] = 0;
            }
            *HBM_Active_V_ID = 0;
            *HBM_Active_V_Value = 0;
            *Rd_HBM_Edge_Addr = 0;
            *Rd_HBM_Edge_Valid = 0;
        } else {
            if (CombineStage_Full) {
                for (int i = 0; i < CACHELINE_LEN; ++i) {
                    Rd_HBM_Edge_Mask->flag[i] = 0;
                }
                *HBM_Active_V_ID = 0;
                *HBM_Active_V_Value = 0;
                *Rd_HBM_Edge_Addr = 0;
                *Rd_HBM_Edge_Valid = 0;
            } else {
                if (v_loffset_buffer[Core_ID].front() == v_roffset_buffer[Core_ID].front() + 1) {
                    for (int i = 0; i < CACHELINE_LEN; ++i) {
                        Rd_HBM_Edge_Mask->flag[i] = 0;
                    }
                    *HBM_Active_V_ID = 0;
                    *HBM_Active_V_Value = 0;
                    *Rd_HBM_Edge_Addr = 0;
                    *Rd_HBM_Edge_Valid = 0;
                    now_loffset[Core_ID] = 1000000000;

                    v_id_buffer[Core_ID].pop();
                    v_loffset_buffer[Core_ID].pop();
                    v_roffset_buffer[Core_ID].pop();
                    v_value_buffer[Core_ID].pop();
                } else if (now_loffset[Core_ID] / CACHELINE_LEN > v_roffset_buffer[Core_ID].front() / CACHELINE_LEN){
                    for (int i = 0; i < CACHELINE_LEN; ++ i) {
                        if (i < v_loffset_buffer[Core_ID].front() % CACHELINE_LEN || (i > v_roffset_buffer[Core_ID].front() % CACHELINE_LEN && v_loffset_buffer[Core_ID].front() / CACHELINE_LEN == v_roffset_buffer[Core_ID].front() / CACHELINE_LEN)){
                            Rd_HBM_Edge_Mask->flag[i] = 0;
                        }
                        else {
                            Rd_HBM_Edge_Mask->flag[i] = 1;
                        }
                    }
                    *HBM_Active_V_ID = v_id_buffer[Core_ID].front();
                    *HBM_Active_V_Value = v_value_buffer[Core_ID].front() / (v_roffset_buffer[Core_ID].front() - v_loffset_buffer[Core_ID].front() + 1) * DAMPING;
                    *Rd_HBM_Edge_Addr = v_loffset_buffer[Core_ID].front() / CACHELINE_LEN;
                    *Rd_HBM_Edge_Valid = 1;

                    if (v_loffset_buffer[Core_ID].front() / CACHELINE_LEN == v_roffset_buffer[Core_ID].front() / CACHELINE_LEN) {
                        v_id_buffer[Core_ID].pop();
                        v_loffset_buffer[Core_ID].pop();
                        v_roffset_buffer[Core_ID].pop();
                        v_value_buffer[Core_ID].pop();

                        if (v_loffset_buffer[Core_ID].size() >= 1) {
                            now_loffset[Core_ID] = v_loffset_buffer[Core_ID].front();
                        } else {
                            now_loffset[Core_ID] = 1000000000;
                        }
                    } else {
                        now_loffset[Core_ID] = (v_loffset_buffer[Core_ID].front() / CACHELINE_LEN + 1) * CACHELINE_LEN;
                    }
                } else {
                    for (int i = 0; i < CACHELINE_LEN; ++ i) {
                        if (i < now_loffset[Core_ID] % CACHELINE_LEN || (i > v_roffset_buffer[Core_ID].front() % CACHELINE_LEN && now_loffset[Core_ID] / CACHELINE_LEN == v_roffset_buffer[Core_ID].front() / CACHELINE_LEN)){
                            Rd_HBM_Edge_Mask->flag[i] = 0;
                        }
                        else {
                            Rd_HBM_Edge_Mask->flag[i] = 1;
                        }
                    }
                    *HBM_Active_V_ID = v_id_buffer[Core_ID].front();
                    *HBM_Active_V_Value = v_value_buffer[Core_ID].front() / (v_roffset_buffer[Core_ID].front() - v_loffset_buffer[Core_ID].front() + 1) * DAMPING;
                    *Rd_HBM_Edge_Addr = now_loffset[Core_ID] / CACHELINE_LEN;
                    *Rd_HBM_Edge_Valid = 1;

                    if (now_loffset[Core_ID] / CACHELINE_LEN == v_roffset_buffer[Core_ID].front() / CACHELINE_LEN) {
                        v_id_buffer[Core_ID].pop();
                        v_loffset_buffer[Core_ID].pop();
                        v_roffset_buffer[Core_ID].pop();
                        v_value_buffer[Core_ID].pop();
                        if (v_loffset_buffer[Core_ID].size() >= 1) {
                            now_loffset[Core_ID] = v_loffset_buffer[Core_ID].front();
                        } else {
                            now_loffset[Core_ID] = 1000000000;
                        }
                    } else {
                        now_loffset[Core_ID] = (now_loffset[Core_ID] / CACHELINE_LEN + 1) * CACHELINE_LEN;
                    }
                }
            }
        }
    }

    static int end_ct[CORE_NUM];
    if (!rst_rd && Front_Iteration_End && Front_Iteration_End_DValid && v_buffer_empty[Core_ID] && !Front_Offset_DValid) {
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

    // value from bram
    if (rst_rd) {
        clear_buffer(v_value_buffer[Core_ID]);
    }
    else {
        if (Front_Active_V_Value_Valid) { 
            // 此处需要判定迭代轮次是否一致，对于无入边点来说，迭代轮次始终为0，仅前两轮对其处理
            if (Front_Active_V_Value._iteration_id != Front_Iteration_ID) {
                Front_Active_V_Value._v_value = 0;
            }
            v_value_buffer[Core_ID].push({Front_Active_V_Value});           
        }
    }

    // id from front stage
    if (rst_rd) {
        clear_buffer(v_id_buffer[Core_ID]);
    }
    else {
        if (Front_Active_V_DValid) { 
            v_id_buffer[Core_ID].push({Front_Active_V_ID});           
        }
    }

    // offset from uram
    if (rst_rd) {
        clear_buffer(v_loffset_buffer[Core_ID]);
        clear_buffer(v_roffset_buffer[Core_ID]);
    } else {
        if (Front_Offset_DValid) {
            v_loffset_buffer[Core_ID].push({Front_Active_V_LOffset});
            v_roffset_buffer[Core_ID].push({Front_Active_V_ROffset - 1});
        }
    }

    //assign stage_full = v_buffer_full
    *Stage_Full = (v_id_buffer[Core_ID].size() >= FIFO_SIZE);
}
