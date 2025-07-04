#include "core.h"
#include "parameter.h"
#include "structure.h"
#include "util.h"
#include "debug.h"

#if DEBUG
    debug_Generate_HBM_Edge_Rqst debug_M5;
#endif

extern int clk;
extern int rst_rd;

using namespace std;

void Generate_HBM_Edge_Rqst_Single(
        int Channel_ID,
        int Front_Rd_HBM_Edge_Addr[], BitVector_16 Front_Rd_HBM_Edge_Mask[], int Front_Rd_HBM_Edge_Valid[],
        int Front_Active_V_ID[], V_VALUE_TP Front_Active_V_Value[],
        int Front_Iteration_End[], int Front_Iteration_End_DValid[], int Front_Iteration_ID[],
        int HBM_Full, int NextStage_Full,

        int *Stage_Full,
        int *Rd_HBM_Edge_Addr, int *Rd_HBM_Edge_Valid,
        int *Active_V_ID, V_VALUE_TP *Active_V_Value, int *Active_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID);

// 96 signals: Front_Rd_HBM_Edge_Addr, Front_Rd_HBM_Edge_Mask, Front_Rd_HBM_Edge_Valid, Front_Active_V_ID, Front_Active_V_DValid
//              Stage_Full, Active_V_ID, Active_V_DValid
// 6 signals: HBM_Full, NextStage_Full, Rd_HBM_Edge_Addr, Rd_HBM_Edge_Valid
void Generate_HBM_Edge_Rqst(
        int Front_Rd_HBM_Edge_Addr[], BitVector_16 Front_Rd_HBM_Edge_Mask[], int Front_Rd_HBM_Edge_Valid[],
        int Front_Active_V_ID[], V_VALUE_TP Front_Active_V_Value[],
        int Front_Iteration_End[], int Front_Iteration_End_DValid[], int Front_Iteration_ID[],
        int HBM_Full[], int NextStage_Full[],

        int *Stage_Full,
        int *Rd_HBM_Edge_Addr, int *Rd_HBM_Edge_Valid,
        int *Active_V_ID, V_VALUE_TP *Active_V_Value, int *Active_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID) {

    //96 -> 6
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        int tmp_front_rd_hbm_edge_addr[GROUP_CORE_NUM], tmp_front_rd_hbm_edge_valid[GROUP_CORE_NUM];
        BitVector_16 tmp_front_rd_hbm_edge_mask[GROUP_CORE_NUM];
        int tmp_front_active_v_id[GROUP_CORE_NUM];
        V_VALUE_TP tmp_front_active_v_value[GROUP_CORE_NUM];
        int tmp_front_iteration_end[GROUP_CORE_NUM], tmp_front_iteration_end_dvalid[GROUP_CORE_NUM], tmp_front_iteration_id[GROUP_CORE_NUM];

        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            tmp_front_rd_hbm_edge_addr[j] = Front_Rd_HBM_Edge_Addr[i * GROUP_CORE_NUM + j];
            for (int k = 0; k < 16; ++ k) tmp_front_rd_hbm_edge_mask[j].flag[k] = Front_Rd_HBM_Edge_Mask[i * GROUP_CORE_NUM + j].flag[k];
            tmp_front_rd_hbm_edge_valid[j] = Front_Rd_HBM_Edge_Valid[i * GROUP_CORE_NUM + j];
            tmp_front_active_v_id[j] = Front_Active_V_ID[i * GROUP_CORE_NUM + j];
            tmp_front_active_v_value[j] = Front_Active_V_Value[i * GROUP_CORE_NUM + j];
            tmp_front_iteration_end[j] = Front_Iteration_End[i * GROUP_CORE_NUM + j];
            tmp_front_iteration_end_dvalid[j] = Front_Iteration_End_DValid[i * GROUP_CORE_NUM + j];
            tmp_front_iteration_id[j] = Front_Iteration_ID[i * GROUP_CORE_NUM + j];
        }

        Generate_HBM_Edge_Rqst_Single(
            i,
            tmp_front_rd_hbm_edge_addr, tmp_front_rd_hbm_edge_mask, tmp_front_rd_hbm_edge_valid,
            tmp_front_active_v_id, tmp_front_active_v_value,
            tmp_front_iteration_end, tmp_front_iteration_end_dvalid, tmp_front_iteration_id,
            HBM_Full[i], NextStage_Full[i],

            Stage_Full,
            &Rd_HBM_Edge_Addr[i], &Rd_HBM_Edge_Valid[i],
            Active_V_ID, Active_V_Value, Active_V_DValid,
            Iteration_End, Iteration_End_DValid, Iteration_ID);
    }
}


void Generate_HBM_Edge_Rqst_Single(
        int Channel_ID,
        int Front_Rd_HBM_Edge_Addr[], BitVector_16 Front_Rd_HBM_Edge_Mask[], int Front_Rd_HBM_Edge_Valid[],
        int Front_Active_V_ID[], V_VALUE_TP Front_Active_V_Value[],
        int Front_Iteration_End[], int Front_Iteration_End_DValid[], int Front_Iteration_ID[],
        int HBM_Full, int NextStage_Full,

        int *Stage_Full,
        int *Rd_HBM_Edge_Addr, int *Rd_HBM_Edge_Valid,
        int *Active_V_ID, V_VALUE_TP *Active_V_Value, int *Active_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID) {

    static queue<int> edge_addr_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<BitVector_16> edge_mask_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<int> v_id_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<V_VALUE_TP> v_value_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];

    static int v_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static int edge_addr_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], edge_addr_buffer_all_empty[PSEUDO_CHANNEL_NUM];

    #if DEBUG
        debug_M5._edge_addr_buffer = edge_addr_buffer;
        debug_M5._edge_mask_buffer = edge_mask_buffer;
        debug_M5._v_id_buffer = v_id_buffer;
        debug_M5._v_value_buffer = v_value_buffer;
    #endif

    edge_addr_buffer_all_empty[Channel_ID] = 1;
    for (int i = 0; i < GROUP_CORE_NUM; ++ i) {
        v_buffer_empty[Channel_ID][i] = (v_id_buffer[Channel_ID][i].size() == 0);
        edge_addr_buffer_empty[Channel_ID][i] = (edge_addr_buffer[Channel_ID][i].size() == 0);
        edge_addr_buffer_all_empty[Channel_ID] = edge_addr_buffer_all_empty[Channel_ID] & edge_addr_buffer_empty[Channel_ID][i];
    }


    //using binary-tree to implement this wire logic
    int min_edge_addr = 1000000000;
    for (int i = 0; i < GROUP_CORE_NUM; ++ i) {
        if(!edge_addr_buffer_empty[Channel_ID][i])
            min_edge_addr = min(min_edge_addr, edge_addr_buffer[Channel_ID][i].front());
    }

    for (int i = 0; i < GROUP_CORE_NUM; ++ i) {
        if (rst_rd) {
            /*initialize edge_addr_buffer, edge_mask_buffer, push_flag_buffer, v_id_buffer, v_value_buffer*/
            clear_buffer(edge_addr_buffer[Channel_ID][i]);
            clear_buffer(edge_mask_buffer[Channel_ID][i]);
            clear_buffer(v_id_buffer[Channel_ID][i]);
            clear_buffer(v_value_buffer[Channel_ID][i]);
        }
        else {
            if (Front_Rd_HBM_Edge_Valid[i]) {
                edge_mask_buffer[Channel_ID][i].push(Front_Rd_HBM_Edge_Mask[i]);
                edge_addr_buffer[Channel_ID][i].push(Front_Rd_HBM_Edge_Addr[i]);
                v_id_buffer[Channel_ID][i].push(Front_Active_V_ID[i]);
                v_value_buffer[Channel_ID][i].push(Front_Active_V_Value[i]);
            }
        }
    }


    if (rst_rd) {
        *Rd_HBM_Edge_Addr = 0;
        *Rd_HBM_Edge_Valid = 0;
    }
    else {
        if (HBM_Full || NextStage_Full || edge_addr_buffer_all_empty[Channel_ID]) {
            *Rd_HBM_Edge_Addr = 0;
            *Rd_HBM_Edge_Valid = 0;
        }
        else {
            *Rd_HBM_Edge_Addr = min_edge_addr;
            *Rd_HBM_Edge_Valid = 1;
       }
    }

    for (int i = 0; i < CACHELINE_LEN; ++ i) {
        if (rst_rd) {
            Active_V_ID[Channel_ID * GROUP_CORE_NUM + i] = 0;
            Active_V_Value[Channel_ID * GROUP_CORE_NUM + i] = 0;
            Active_V_DValid[Channel_ID * GROUP_CORE_NUM + i] = 0;
        }
        else {
            if (HBM_Full || NextStage_Full || edge_addr_buffer_all_empty[Channel_ID]) {
                Active_V_ID[Channel_ID * GROUP_CORE_NUM + i] = 0;
                Active_V_Value[Channel_ID * GROUP_CORE_NUM + i] = 0;
                Active_V_DValid[Channel_ID * GROUP_CORE_NUM + i] = 0;
            }
            else {
                //using switch-case
                int tmp_j = -1;
                for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
                    if (!edge_addr_buffer_empty[Channel_ID][j] && edge_addr_buffer[Channel_ID][j].front() == min_edge_addr && edge_mask_buffer[Channel_ID][j].front().flag[i]) {
                        tmp_j = j;
                    }
                }
                if (tmp_j != -1) {
                    Active_V_ID[Channel_ID * GROUP_CORE_NUM + i] = v_id_buffer[Channel_ID][tmp_j].front();
                    Active_V_Value[Channel_ID * GROUP_CORE_NUM + i] = v_value_buffer[Channel_ID][tmp_j].front();
                } else {
                    Active_V_ID[Channel_ID * GROUP_CORE_NUM + i] = -1;
                    Active_V_Value[Channel_ID * GROUP_CORE_NUM + i] = -1.0;
                }
                Active_V_DValid[Channel_ID * GROUP_CORE_NUM + i] = 1;
            }
        }
    }

    for (int i = 0; i < GROUP_CORE_NUM; ++ i){
        if (!(rst_rd || HBM_Full || NextStage_Full || edge_addr_buffer_all_empty[Channel_ID])) {
            if (edge_addr_buffer[Channel_ID][i].front() == min_edge_addr && !edge_addr_buffer_empty[Channel_ID][i]) {
                edge_addr_buffer[Channel_ID][i].pop();
                edge_mask_buffer[Channel_ID][i].pop();
                v_id_buffer[Channel_ID][i].pop();
                v_value_buffer[Channel_ID][i].pop();
            }
        }
    }

    int front_iteration_end_all, front_iteration_end_dvalid_all, v_buffer_empty_all;
    // assign logic
    //Check add there initialization
    front_iteration_end_all = 1;
    front_iteration_end_dvalid_all = 1;
    v_buffer_empty_all = 1;
    for (int i = 0; i < GROUP_CORE_NUM; i++) {
        front_iteration_end_all &= Front_Iteration_End[i];
        front_iteration_end_dvalid_all &= Front_Iteration_End_DValid[i];
        v_buffer_empty_all &= v_buffer_empty[Channel_ID][i];
    }

    static int end_ct[CORE_NUM];
    for (int i = 0; i < GROUP_CORE_NUM; ++ i) {
        if (!rst_rd && front_iteration_end_all && front_iteration_end_dvalid_all && v_buffer_empty_all) {
            if (end_ct[Channel_ID * GROUP_CORE_NUM + i] >= WAIT_END_DELAY) {
                Iteration_End[Channel_ID * GROUP_CORE_NUM + i] = 1;
                Iteration_End_DValid[Channel_ID * GROUP_CORE_NUM + i] = 1;
                Iteration_ID[Channel_ID * GROUP_CORE_NUM + i] = Front_Iteration_ID[i];
            } else {
                end_ct[Channel_ID * GROUP_CORE_NUM + i]++;
                Iteration_End[Channel_ID * GROUP_CORE_NUM + i] = 0;
                Iteration_End_DValid[Channel_ID * GROUP_CORE_NUM + i] = 0;
                Iteration_ID[Channel_ID * GROUP_CORE_NUM + i] = Front_Iteration_ID[i];
            }
        }
        else {
            end_ct[Channel_ID * GROUP_CORE_NUM + i] = 0;
            Iteration_End[Channel_ID * GROUP_CORE_NUM + i] = 0;
            Iteration_End_DValid[Channel_ID * GROUP_CORE_NUM + i] = 0;
            Iteration_ID[Channel_ID * GROUP_CORE_NUM + i] = Front_Iteration_ID[i];
        }
    }

    for (int i = 0; i < GROUP_CORE_NUM; ++ i)
        Stage_Full[Channel_ID * GROUP_CORE_NUM + i] = (edge_addr_buffer[Channel_ID][i].size() >= FIFO_SIZE);
}
