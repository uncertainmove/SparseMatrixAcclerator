// v_buffer ,iteration_buffer, && HBM_data must be synchronized here
#include "core.h"
#include "parameter.h"
#include "util.h"
#include "debug.h"

#if DEBUG
    debug_Schedule debug_M7;
#endif

extern int clk;
extern int rst_rd;

using namespace std;

// 6 signals: stage_full
void Schedule_Single(
        int Core_ID,
        int Front_Active_V_ID, V_VALUE_TP Front_Active_V_Value, int Front_Active_V_DValid,
        int Front_Iteration_End, int Front_Iteration_End_DValid, int Front_Iteration_ID,
        int HBM_Iterface_Active_V_Edge, int HBM_Iterface_Active_V_Edge_Valid,
        int NextStage_Full,

        int *Stage_Full,
        int *Update_V_ID, V_VALUE_TP *Update_V_Value, int *Update_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID);

void Schedule(
        int Front_Active_V_ID[], V_VALUE_TP Front_Active_V_Value[], int Front_Active_V_DValid[],
        int Front_Iteration_End[], int Front_Iteration_End_DValid[], int Front_Iteration_ID[],
        int HBM_Iterface_Active_V_Edge[], int HBM_Iterface_Active_V_Edge_Valid[],
        int NextStage_Full[],

        int *Stage_Full,
        int *Update_V_ID, V_VALUE_TP *Update_V_Value, int *Update_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID) {

    int tmp_stage_full[CORE_NUM];

    for (int i = 0; i < CORE_NUM; ++ i) {
        Schedule_Single(
            i,
            Front_Active_V_ID[i], Front_Active_V_Value[i], Front_Active_V_DValid[i],
            Front_Iteration_End[i], Front_Iteration_End_DValid[i], Front_Iteration_ID[i],
            HBM_Iterface_Active_V_Edge[i], HBM_Iterface_Active_V_Edge_Valid[i],
            NextStage_Full[i],

            &tmp_stage_full[i],
            &Update_V_ID[i], &Update_V_Value[i], &Update_V_DValid[i],
            &Iteration_End[i], &Iteration_End_DValid[i], &Iteration_ID[i]);
    }

    int tmp_stage_full_group;
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        tmp_stage_full_group = 0;
        for (int j = i * GROUP_CORE_NUM; j < (i + 1) * GROUP_CORE_NUM; ++ j) {
            tmp_stage_full_group = tmp_stage_full_group || tmp_stage_full[j];
        }

        Stage_Full[i] = tmp_stage_full_group;
    }
}


void Schedule_Single(
    int Core_ID,
    int Front_Active_V_ID, V_VALUE_TP Front_Active_V_Value, int Front_Active_V_DValid,
    int Front_Iteration_End, int Front_Iteration_End_DValid, int Front_Iteration_ID,
    int HBM_Iterface_Active_V_Edge, int HBM_Iterface_Active_V_Edge_Valid,
    int NextStage_Full,

    int *Stage_Full,
    int *Update_V_ID, V_VALUE_TP *Update_V_Value, int *Update_V_DValid,
    int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID) {

    static queue<int> v_id_buffer[CORE_NUM], edge_buffer[CORE_NUM];
    static queue<V_VALUE_TP> v_value_buffer[CORE_NUM];
    static int v_buffer_empty[CORE_NUM], e_buffer_empty[CORE_NUM];

    #if DEBUG
        debug_M7._v_id_buffer = v_id_buffer;
        debug_M7._edge_buffer = edge_buffer;
        debug_M7._v_value_buffer = v_value_buffer;
    #endif

    v_buffer_empty[Core_ID] = (v_id_buffer[Core_ID].size() == 0);
    e_buffer_empty[Core_ID] = (edge_buffer[Core_ID].size() == 0);

    if (rst_rd || NextStage_Full) {
        *Update_V_ID = 0;
        *Update_V_Value = 0;
        *Update_V_DValid = 0;
    } else {
        if (!v_buffer_empty[Core_ID] && !e_buffer_empty[Core_ID]) {
            if (v_id_buffer[Core_ID].front() != -1) {
                *Update_V_ID = edge_buffer[Core_ID].front();
                *Update_V_Value = v_value_buffer[Core_ID].front();
                *Update_V_DValid = 1;
                // cout << "clk: " << clk << ", M07 send task, src_id: " << v_id_buffer[Core_ID].front() << ", dst_id: " <<
                    // *Update_V_ID << ", value: " << *(float *)Update_V_Value << endl;
            } else {
                *Update_V_ID = 0;
                *Update_V_Value = 0;
                *Update_V_DValid = 0;
            }
            v_id_buffer[Core_ID].pop();
            v_value_buffer[Core_ID].pop();
            edge_buffer[Core_ID].pop();
        }
        else {
            *Update_V_ID = 0;
            *Update_V_Value = 0;
            *Update_V_DValid = 0;
        }
    }

    if (rst_rd) {
        /*initialize push_flag1, v_id1, v_value1 buffers*/
        clear_buffer(v_id_buffer[Core_ID]);
        clear_buffer(v_value_buffer[Core_ID]);
    }
    else {
        if (Front_Active_V_DValid) {
            v_id_buffer[Core_ID].push(Front_Active_V_ID);
            v_value_buffer[Core_ID].push(Front_Active_V_Value);
        }
    }

    if (rst_rd) {
        /*initialize edge_buffer1*/
        clear_buffer(edge_buffer[Core_ID]);
    }
    else {
        if (HBM_Iterface_Active_V_Edge_Valid) {
            edge_buffer[Core_ID].push(HBM_Iterface_Active_V_Edge);
        }
    }

    static int end_ct[CORE_NUM];
    if (!rst_rd && Front_Iteration_End && Front_Iteration_End_DValid && v_buffer_empty[Core_ID]) {
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

    *Stage_Full = (v_id_buffer[Core_ID].size() >= FIFO_SIZE);
}