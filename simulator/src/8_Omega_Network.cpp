#include "Accelerator.h"
#if DEBUG
    #include "debug.h"
    debug_Omega_Network debug_M8;
#endif

extern int clk;
extern int rst_rd;

using namespace std;

int array2n[10];

void Omega_Network_1_Single(int Omega_Depth_ID, int Omega_Switch_ID,
                            int Push_Flag1, int V_ID_In1, int V_Value_In1, int Pull_First_Flag1, int V_Valid_In1, int Iteration_End_In1, int Iteration_End_DValid_In1,
                            int Push_Flag2, int V_ID_In2, int V_Value_In2, int Pull_First_Flag2, int V_Valid_In2, int Iteration_End_In2, int Iteration_End_DValid_In2,
                            int Next_Stage_Full1, int Next_Stage_Full2,

                            int *Stage_Full1, int *Stage_Full2,
                            int *Om1_Push_Flag_Out1, int *Om1_V_ID_Out1, int *Om1_V_Value_Out1, int *Om1_Pull_First_Flag1, int *Om1_V_Valid_Out1, int *Om1_Iteration_End_Out1, int *Om1_Iteration_End_DValid_Out1,
                            int *Om1_Push_Flag_Out2, int *Om1_V_ID_Out2, int *Om1_V_Value_Out2, int *Om1_Pull_First_Flag2, int *Om1_V_Valid_Out2, int *Om1_Iteration_End_Out2, int *Om1_Iteration_End_DValid_Out2);

void Omega_Network_2_Single(int Omega_Depth_ID, int Omega_Switch_ID,
                            int Push_Flag1, int V_ID_In1, int V_Value_In1, int Pull_First_Flag1, int V_Valid_In1, int Iteration_End_In1, int Iteration_End_DValid_In1,
                            int Push_Flag2, int V_ID_In2, int V_Value_In2, int Pull_First_Flag2, int V_Valid_In2, int Iteration_End_In2, int Iteration_End_DValid_In2,
                            int Next_Stage_Full1, int Next_Stage_Full2,

                            int *Stage_Full1, int *Stage_Full2,
                            int *Om2_Push_Flag_Out1, int *Om2_V_ID_Out1, int *Om2_V_Value_Out1, int *Om2_Pull_First_Flag1, int *Om2_V_Valid_Out1, int *Om2_Iteration_End_Out1, int *Om2_Iteration_End_DValid_Out1,
                            int *Om2_Push_Flag_Out2, int *Om2_V_ID_Out2, int *Om2_V_Value_Out2, int *Om2_Pull_First_Flag2, int *Om2_V_Valid_Out2, int *Om2_Iteration_End_Out2, int *Om2_Iteration_End_DValid_Out2);

void Omega_Network(int Om1_Front_Push_Flag[], int Om1_Front_Update_V_ID[], int Om1_Front_Update_V_Value[], int Om1_Front_Pull_First_Flag[], int Om1_Front_Update_V_DValid[], int Om1_Front_Iteration_End[], int Om1_Front_Iteration_End_DValid[],
                   int Om2_Front_Push_Flag[], int Om2_Front_Update_V_ID[], int Om2_Front_Update_V_Value[], int Om2_Front_Pull_First_Flag[], int Om2_Front_Update_V_DValid[], int Om2_Front_Iteration_End[], int Om2_Front_Iteration_End_DValid[],
                   int Source_Core_Full[],

                   int *Stage_Full_Om1, int *Stage_Full_Om2,
                   int *Om1_Push_Flag, int *Om1_Update_V_ID, int *Om1_Update_V_Value, int *Om1_Pull_First_Flag, int *Om1_Update_V_DValid, int *Om1_Iteration_End, int *Om1_Iteration_End_DValid,
                   int *Om2_Push_Flag, int *Om2_Update_V_ID, int *Om2_Update_V_Value, int *Om2_Pull_First_Flag, int *Om2_Update_V_DValid, int *Om2_Iteration_End, int *Om2_Iteration_End_DValid) {

    static int tmp_om1_push_flag_in[OMEGA_DEPTH + 1][CORE_NUM], tmp_om1_v_id_in[OMEGA_DEPTH + 1][CORE_NUM], tmp_om1_v_value_in[OMEGA_DEPTH + 1][CORE_NUM], tmp_om1_pull_first_flag_in[OMEGA_DEPTH + 1][CORE_NUM], tmp_om1_v_valid_in[OMEGA_DEPTH + 1][CORE_NUM],
        tmp_om1_iteration_end[OMEGA_DEPTH + 1][CORE_NUM], tmp_om1_iteration_end_dvalid[OMEGA_DEPTH + 1][CORE_NUM];

    static int tmp_om2_push_flag_in[OMEGA_DEPTH + 1][CORE_NUM], tmp_om2_v_id_in[OMEGA_DEPTH + 1][CORE_NUM], tmp_om2_v_value_in[OMEGA_DEPTH + 1][CORE_NUM], tmp_om2_pull_first_flag_in[OMEGA_DEPTH + 1][CORE_NUM], tmp_om2_v_valid_in[OMEGA_DEPTH + 1][CORE_NUM],
        tmp_om2_iteration_end[OMEGA_DEPTH + 1][CORE_NUM], tmp_om2_iteration_end_dvalid[OMEGA_DEPTH + 1][CORE_NUM];

    static int tmp_om1_stage_full[OMEGA_DEPTH + 1][CORE_NUM], tmp_om2_stage_full[OMEGA_DEPTH + 1][CORE_NUM];

    for (int i = 0; i < CORE_NUM; ++ i) {
        tmp_om1_push_flag_in[0][i] = Om1_Front_Push_Flag[i];
        tmp_om1_v_id_in[0][i] = Om1_Front_Update_V_ID[i];
        tmp_om1_v_value_in[0][i] = Om1_Front_Update_V_Value[i];
        tmp_om1_pull_first_flag_in[0][i] = Om1_Front_Pull_First_Flag[i];
        tmp_om1_v_valid_in[0][i] = Om1_Front_Update_V_DValid[i];
        tmp_om1_iteration_end[0][i] = Om1_Front_Iteration_End[i];
        tmp_om1_iteration_end_dvalid[0][i] = Om1_Front_Iteration_End_DValid[i];
        tmp_om1_stage_full[OMEGA_DEPTH][i] = Source_Core_Full[i];
        // check power-law vertex
        #if DEBUG
            for (int j = 0; j < POWER_LAW_V_NUM; j++) {
                if (Om1_Front_Update_V_Value[i] == VERTEX_NOT_READ[j]) {
                    printf("catch power-law vertex in %d, v_id=%d, v_value=%d\n", i, Om1_Front_Update_V_ID[i], Om1_Front_Update_V_Value[i]);
                    printf("Maybe you should set CONTROL_FLAG to 0\n");
                    exit(-1);
                }
            }
        #endif

        tmp_om2_push_flag_in[0][i] = Om2_Front_Push_Flag[i];
        tmp_om2_v_id_in[0][i] = Om2_Front_Update_V_ID[i];
        tmp_om2_v_value_in[0][i] = Om2_Front_Update_V_Value[i];
        tmp_om2_pull_first_flag_in[0][i] = Om2_Front_Pull_First_Flag[i];
        tmp_om2_v_valid_in[0][i] = Om2_Front_Update_V_DValid[i];
        tmp_om2_iteration_end[0][i] = Om2_Front_Iteration_End[i];
        tmp_om2_iteration_end_dvalid[0][i] = Om2_Front_Iteration_End_DValid[i];
        tmp_om2_stage_full[OMEGA_DEPTH][i] = 0; //destination core never full
    }


    for (int i = OMEGA_DEPTH; i >= 1; -- i) {
        for (int j = 0; j < OMEGA_SWITCH_NUM; ++ j) {
            int in_id1 = j;
            int in_id2 = j + OMEGA_SWITCH_NUM;
            int out_id1 = j * 2;
            int out_id2 = j * 2 + 1;
            // int out_id1 = (j < OMEGA_SWITCH_NUM / 2) ? (j * 4) : (j * 4 - CORE_NUM + 1);
            // int out_id2 = (j < OMEGA_SWITCH_NUM / 2) ? (j * 4 + 2) : (j * 4 - CORE_NUM + 3);

            Omega_Network_1_Single(i - 1, j,
                                   tmp_om1_push_flag_in[i - 1][in_id1], tmp_om1_v_id_in[i - 1][in_id1], tmp_om1_v_value_in[i - 1][in_id1], tmp_om1_pull_first_flag_in[i - 1][in_id1], tmp_om1_v_valid_in[i - 1][in_id1], tmp_om1_iteration_end[i - 1][in_id1], tmp_om1_iteration_end_dvalid[i - 1][in_id1],
                                   tmp_om1_push_flag_in[i - 1][in_id2], tmp_om1_v_id_in[i - 1][in_id2], tmp_om1_v_value_in[i - 1][in_id2], tmp_om1_pull_first_flag_in[i - 1][in_id2], tmp_om1_v_valid_in[i - 1][in_id2], tmp_om1_iteration_end[i - 1][in_id2], tmp_om1_iteration_end_dvalid[i - 1][in_id2],
                                   tmp_om1_stage_full[i][out_id1], tmp_om1_stage_full[i][out_id2],

                                   &tmp_om1_stage_full[i - 1][in_id1], &tmp_om1_stage_full[i - 1][in_id2],
                                   &tmp_om1_push_flag_in[i][out_id1], &tmp_om1_v_id_in[i][out_id1], &tmp_om1_v_value_in[i][out_id1], &tmp_om1_pull_first_flag_in[i][out_id1], &tmp_om1_v_valid_in[i][out_id1], &tmp_om1_iteration_end[i][out_id1], &tmp_om1_iteration_end_dvalid[i][out_id1],
                                   &tmp_om1_push_flag_in[i][out_id2], &tmp_om1_v_id_in[i][out_id2], &tmp_om1_v_value_in[i][out_id2], &tmp_om1_pull_first_flag_in[i][out_id2], &tmp_om1_v_valid_in[i][out_id2], &tmp_om1_iteration_end[i][out_id2], &tmp_om1_iteration_end_dvalid[i][out_id2]);

            Omega_Network_2_Single(i - 1, j,
                                   tmp_om2_push_flag_in[i - 1][in_id1], tmp_om2_v_id_in[i - 1][in_id1], tmp_om2_v_value_in[i - 1][in_id1], tmp_om2_pull_first_flag_in[i - 1][in_id1], tmp_om2_v_valid_in[i - 1][in_id1], tmp_om2_iteration_end[i - 1][in_id1], tmp_om2_iteration_end_dvalid[i - 1][in_id1],
                                   tmp_om2_push_flag_in[i - 1][in_id2], tmp_om2_v_id_in[i - 1][in_id2], tmp_om2_v_value_in[i - 1][in_id2], tmp_om2_pull_first_flag_in[i - 1][in_id2], tmp_om2_v_valid_in[i - 1][in_id2], tmp_om2_iteration_end[i - 1][in_id2], tmp_om2_iteration_end_dvalid[i - 1][in_id2],
                                   tmp_om2_stage_full[i][out_id1], tmp_om2_stage_full[i][out_id2],

                                   &tmp_om2_stage_full[i - 1][in_id1], &tmp_om2_stage_full[i - 1][in_id2],
                                   &tmp_om2_push_flag_in[i][out_id1], &tmp_om2_v_id_in[i][out_id1], &tmp_om2_v_value_in[i][out_id1], &tmp_om2_pull_first_flag_in[i][out_id1], &tmp_om2_v_valid_in[i][out_id1], &tmp_om2_iteration_end[i][out_id1], &tmp_om2_iteration_end_dvalid[i][out_id1],
                                   &tmp_om2_push_flag_in[i][out_id2], &tmp_om2_v_id_in[i][out_id2], &tmp_om2_v_value_in[i][out_id2], &tmp_om2_pull_first_flag_in[i][out_id2], &tmp_om2_v_valid_in[i][out_id2], &tmp_om2_iteration_end[i][out_id2], &tmp_om2_iteration_end_dvalid[i][out_id2]);
        }
    }

    for (int i = 0; i < CORE_NUM; ++ i) {
        Stage_Full_Om1[i] = tmp_om1_stage_full[0][i];
        Stage_Full_Om2[i] = tmp_om2_stage_full[0][i];
    }

    for (int i = 0; i < CORE_NUM; ++ i) {
        Om1_Push_Flag[i] = tmp_om1_push_flag_in[OMEGA_DEPTH][i];
        Om1_Update_V_ID[i] = tmp_om1_v_id_in[OMEGA_DEPTH][i];
        Om1_Update_V_Value[i] = tmp_om1_v_value_in[OMEGA_DEPTH][i];
        Om1_Pull_First_Flag[i] = tmp_om1_pull_first_flag_in[OMEGA_DEPTH][i];
        Om1_Update_V_DValid[i] = tmp_om1_v_valid_in[OMEGA_DEPTH][i];
        Om1_Iteration_End[i] = tmp_om1_iteration_end[OMEGA_DEPTH][i];
        Om1_Iteration_End_DValid[i] = tmp_om1_iteration_end_dvalid[OMEGA_DEPTH][i];

        #if (DEBUG && PRINT_CONS)
            if (Om1_Update_V_ID[i] == PRINT_ID && Om1_Update_V_DValid[i] == 1) {
                printf("clk=%d, om1_push_flag=%d, om1_update_v_id=%d, om1_update_v_value=%d, om1_pull_first_flag=%d, om1_update_v_dvalid=%d, om1_iteration_end=%d, om1_iteration_end_dvalid=%d\n",
                    clk, Om1_Push_Flag[i], Om1_Update_V_ID[i], Om1_Update_V_Value[i], Om1_Pull_First_Flag[i], Om1_Update_V_DValid[i], Om1_Iteration_End[i], Om1_Iteration_End_DValid[i]);
            }
        #endif

        Om2_Push_Flag[i] = tmp_om2_push_flag_in[OMEGA_DEPTH][i];
        Om2_Update_V_ID[i] = tmp_om2_v_id_in[OMEGA_DEPTH][i];
        Om2_Update_V_Value[i] = tmp_om2_v_value_in[OMEGA_DEPTH][i];
        Om2_Pull_First_Flag[i] = tmp_om2_pull_first_flag_in[OMEGA_DEPTH][i];
        Om2_Update_V_DValid[i] = tmp_om2_v_valid_in[OMEGA_DEPTH][i];
        Om2_Iteration_End[i] = tmp_om2_iteration_end[OMEGA_DEPTH][i];
        Om2_Iteration_End_DValid[i] = tmp_om2_iteration_end_dvalid[OMEGA_DEPTH][i];

        #if (DEBUG && PRINT_CONS)
            if (Om2_Update_V_ID[i] == PRINT_ID && Om2_Update_V_DValid[i] == 1) {
                printf("clk=%d, om2_push_flag=%d, om2_update_v_id=%d, om2_update_v_value=%d, om2_pull_first_flag=%d, om2_update_v_dvalid=%d, om2_iteration_end=%d, om2_iteration_end_dvalid=%d\n",
                    clk, Om1_Push_Flag[i], Om2_Update_V_ID[i], Om2_Update_V_Value[i], Om2_Pull_First_Flag[i], Om2_Update_V_DValid[i], Om2_Iteration_End[i], Om2_Iteration_End_DValid[i]);
            }
        #endif
    }
}

void Omega_Network_Global_Signal(int Front_Global_Iteration_ID[], Powerlaw_Vid_Set Front_Global_Powerlaw_V_ID[], Powerlaw_Vvisit_Set Front_Global_Powerlaw_V_Visit[],

                                int *Global_Iteration_ID, Powerlaw_Vid_Set *Global_Powerlaw_V_ID, Powerlaw_Vvisit_Set *Global_Powerlaw_V_Visit) {
    static int local_Global_Iteration_ID[CORE_NUM];
    static Powerlaw_Vid_Set local_Global_Powerlaw_V_ID[CORE_NUM];
    static Powerlaw_Vvisit_Set local_Global_Powerlaw_V_Visit[CORE_NUM];

    for (int i = 0; i < CORE_NUM; i++) {
        if (rst_rd) {
            Global_Iteration_ID[i] = 0;
            for (int j = 0; j < POWER_LAW_V_NUM; j++) {
                Global_Powerlaw_V_ID[i].v[j] = VERTEX_NOT_READ[j];
                Global_Powerlaw_V_Visit[i].visit[j] = false;
            }
        } else {
            Global_Iteration_ID[i] = local_Global_Iteration_ID[i];
            Global_Powerlaw_V_ID[i].set(local_Global_Powerlaw_V_ID[i]);
            Global_Powerlaw_V_Visit[i].set(local_Global_Powerlaw_V_Visit[i]);
        }

        if (rst_rd) {
            local_Global_Iteration_ID[i] = 0;
            for (int j = 0; j < POWER_LAW_V_NUM; j++) {
                local_Global_Powerlaw_V_ID[i].v[j] = VERTEX_NOT_READ[j];
                local_Global_Powerlaw_V_Visit[i].visit[j] = false;
            }
        } else {
            local_Global_Iteration_ID[i] = Front_Global_Iteration_ID[i];
            for (int j = 0; j < POWER_LAW_V_NUM; j++) {
                local_Global_Powerlaw_V_ID[i].set(Front_Global_Powerlaw_V_ID[i]);
                local_Global_Powerlaw_V_Visit[i].set(Front_Global_Powerlaw_V_Visit[i]);
            }
        }
    }
}

void Omega_Network_1_Single(int Omega_Depth_ID, int Omega_Switch_ID,
                            int Push_Flag1, int V_ID_In1, int V_Value_In1, int Pull_First_Flag1, int V_Valid_In1, int Iteration_End_In1, int Iteration_End_DValid_In1,
                            int Push_Flag2, int V_ID_In2, int V_Value_In2, int Pull_First_Flag2, int V_Valid_In2, int Iteration_End_In2, int Iteration_End_DValid_In2,
                            int Next_Stage_Full1, int Next_Stage_Full2,

                            int *Stage_Full1, int *Stage_Full2,
                            int *Om1_Push_Flag_Out1, int *Om1_V_ID_Out1, int *Om1_V_Value_Out1, int *Om1_Pull_First_Flag1, int *Om1_V_Valid_Out1, int *Om1_Iteration_End_Out1, int *Om1_Iteration_End_DValid_Out1,
                            int *Om1_Push_Flag_Out2, int *Om1_V_ID_Out2, int *Om1_V_Value_Out2, int *Om1_Pull_First_Flag2, int *Om1_V_Valid_Out2, int *Om1_Iteration_End_Out2, int *Om1_Iteration_End_DValid_Out2) {

    static queue<int> push_flag_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], id_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], pull_first_flag_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static queue<int> push_flag_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], id_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], pull_first_flag_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static queue<int> push_flag_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], id_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], pull_first_flag_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static queue<int> push_flag_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], id_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], pull_first_flag_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static int buffer_empty_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], buffer_empty_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM];
    static int buffer_empty_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], buffer_empty_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM];
    static int buffer_full_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], buffer_full_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM];
    static int buffer_full_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], buffer_full_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM];

    #if DEBUG
        debug_M8._om1_push_flag_buffer_in1_out1 = push_flag_buffer_in1_out1;
        debug_M8._om1_id_buffer_in1_out1 = id_buffer_in1_out1;
        debug_M8._om1_value_buffer_in1_out1 = value_buffer_in1_out1;
        debug_M8._om1_push_flag_buffer_in2_out1 = push_flag_buffer_in2_out1;
        debug_M8._om1_id_buffer_in2_out1 = id_buffer_in2_out1;
        debug_M8._om1_value_buffer_in2_out1 = value_buffer_in2_out1;
        debug_M8._om1_push_flag_buffer_in1_out2 = push_flag_buffer_in1_out2;
        debug_M8._om1_id_buffer_in1_out2 = id_buffer_in1_out2;
        debug_M8._om1_value_buffer_in1_out2 = value_buffer_in1_out2;
        debug_M8._om1_push_flag_buffer_in2_out2 = push_flag_buffer_in2_out2;
        debug_M8._om1_id_buffer_in2_out2 = id_buffer_in2_out2;
        debug_M8._om1_value_buffer_in2_out2 = value_buffer_in2_out2;
    #endif

    // int in1_port_id = Omega_Switch_ID, in2_port_id = Omega_Switch_ID + OMEGA_SWITCH_NUM;
    // int in1_src_route = in1_port_id & array2n[OMEGA_DEPTH - 1], in2_src_route = in2_port_id & array2n[OMEGA_DEPTH - 1];
    int in1_src_route = 0, in2_src_route = array2n[OMEGA_DEPTH - 1];
    int in1_dst_route = Push_Flag1 ? ((V_ID_In1 << Omega_Depth_ID) & array2n[OMEGA_DEPTH - 1]) : ((V_Value_In1 << Omega_Depth_ID) & array2n[OMEGA_DEPTH - 1]);
    int in2_dst_route = Push_Flag2 ? ((V_ID_In2 << Omega_Depth_ID) & array2n[OMEGA_DEPTH - 1]) : ((V_Value_In2 << Omega_Depth_ID) & array2n[OMEGA_DEPTH - 1]);
    int in1_route_flag = (in1_src_route ^ in1_dst_route), in2_route_flag = (in2_src_route ^ in2_dst_route);

    buffer_empty_in1_out1[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].size() == 0);
    buffer_empty_in1_out2[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].size() == 0);
    buffer_empty_in2_out1[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].size() == 0);
    buffer_empty_in2_out2[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].size() == 0);

    #if DEBUG
        debug_M8._om1_push_flag_buffer_in1_out1 = push_flag_buffer_in1_out1;
        debug_M8._om1_id_buffer_in1_out1 = id_buffer_in1_out1;
        debug_M8._om1_value_buffer_in1_out1 = value_buffer_in1_out1;
        debug_M8._om1_push_flag_buffer_in2_out1 = push_flag_buffer_in2_out1;
        debug_M8._om1_id_buffer_in2_out1 = id_buffer_in2_out1;
        debug_M8._om1_value_buffer_in2_out1 = value_buffer_in2_out1;
        debug_M8._om1_push_flag_buffer_in1_out2 = push_flag_buffer_in1_out2;
        debug_M8._om1_id_buffer_in1_out2 = id_buffer_in1_out2;
        debug_M8._om1_value_buffer_in1_out2 = value_buffer_in1_out2;
        debug_M8._om1_push_flag_buffer_in2_out2 = push_flag_buffer_in2_out2;
        debug_M8._om1_id_buffer_in2_out2 = id_buffer_in2_out2;
        debug_M8._om1_value_buffer_in2_out2 = value_buffer_in2_out2;
    #endif

    if (rst_rd) {
        /*buffer initilization*/
        for (int i = 0; i < OMEGA_DEPTH; ++ i) array2n[i] = 1 << i;
        while (!push_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].empty())        push_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!push_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].empty())        push_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!push_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].empty())        push_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!push_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].empty())        push_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].empty())               id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].empty())               id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].empty())               id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].empty())               id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].empty())            value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].empty())            value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].empty())            value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].empty())            value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!pull_first_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].empty())  pull_first_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!pull_first_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].empty())  pull_first_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!pull_first_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].empty())  pull_first_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!pull_first_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].empty())  pull_first_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
    }
    else {
        if (V_Valid_In1) {
            if (in1_route_flag == 0) {
                push_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].push(Push_Flag1);
                id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In1);
                value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In1);
                pull_first_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].push(Pull_First_Flag1);
            }
            else {
                push_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].push(Push_Flag1);
                id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In1);
                value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In1);
                pull_first_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].push(Pull_First_Flag1);
            }
        }
        if (V_Valid_In2) {
            if (in2_route_flag == 0) {
                push_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].push(Push_Flag2);
                id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In2);
                value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In2);
                pull_first_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].push(Pull_First_Flag2);
            }
            else {
                push_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].push(Push_Flag2);
                id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In2);
                value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In2);
                pull_first_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].push(Pull_First_Flag2);
            }
        }
    }

    if (rst_rd || Next_Stage_Full1) {
        *Om1_Push_Flag_Out1 = 0;
        *Om1_V_ID_Out1 = 0;
        *Om1_V_Value_Out1 = 0;
        *Om1_V_Valid_Out1 = 0;
    }
    else {
        if (id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].size() > id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].size()) { //in1_out1 cannot be empty
            *Om1_Push_Flag_Out1 = push_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om1_V_ID_Out1 = id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om1_V_Value_Out1 = value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om1_Pull_First_Flag1 = pull_first_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om1_V_Valid_Out1 = 1;

            push_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
            id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
            value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
            pull_first_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        }
        else {
            if (buffer_empty_in2_out1[Omega_Depth_ID][Omega_Switch_ID]) {
                *Om1_Push_Flag_Out1 = 0;
                *Om1_V_ID_Out1 = 0;
                *Om1_V_Value_Out1 = 0;
                *Om1_V_Valid_Out1 = 0;
            }
            else {
                *Om1_Push_Flag_Out1 = push_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om1_V_ID_Out1 = id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om1_V_Value_Out1 = value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om1_Pull_First_Flag1 = pull_first_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om1_V_Valid_Out1 = 1;

                push_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
                id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
                value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
                pull_first_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
            }
        }
    }

    if (rst_rd || Next_Stage_Full2) {
        *Om1_Push_Flag_Out2 = 0;
        *Om1_V_ID_Out2 = 0;
        *Om1_V_Value_Out2 = 0;
        *Om1_V_Valid_Out2 = 0;
    }
    else {
        if (id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].size() > id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].size()) { //in1_out1 cannot be empty
            *Om1_Push_Flag_Out2 = push_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om1_V_ID_Out2 = id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om1_V_Value_Out2 = value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om1_Pull_First_Flag2 = pull_first_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om1_V_Valid_Out2 = 1;

            push_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
            id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
            value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
            pull_first_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        }
        else {
            if (buffer_empty_in2_out2[Omega_Depth_ID][Omega_Switch_ID]) {
                *Om1_Push_Flag_Out2 = 0;
                *Om1_V_ID_Out2 = 0;
                *Om1_V_Value_Out2 = 0;
                *Om1_V_Valid_Out2 = 0;
            }
            else {
                *Om1_Push_Flag_Out2 = push_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om1_V_ID_Out2 = id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om1_V_Value_Out2 = value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om1_Pull_First_Flag2 = pull_first_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om1_V_Valid_Out2 = 1;

                push_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
                id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
                value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
                pull_first_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
            }
        }
    }

    if (!rst_rd && Iteration_End_In1 && Iteration_End_DValid_In1 && buffer_empty_in1_out1[Omega_Depth_ID][Omega_Switch_ID] && buffer_empty_in2_out1[Omega_Depth_ID][Omega_Switch_ID]) {
        *Om1_Iteration_End_Out1 = 1;
        *Om1_Iteration_End_DValid_Out1 = 1;
    }
    else {
        *Om1_Iteration_End_Out1 = 0;
        *Om1_Iteration_End_DValid_Out1 = 0;
    }

    if (!rst_rd && Iteration_End_In2 && Iteration_End_DValid_In2 && buffer_empty_in1_out2[Omega_Depth_ID][Omega_Switch_ID] && buffer_empty_in2_out2[Omega_Depth_ID][Omega_Switch_ID]) {
        *Om1_Iteration_End_Out2 = 1;
        *Om1_Iteration_End_DValid_Out2 = 1;
    }
    else {
        *Om1_Iteration_End_Out2 = 0;
        *Om1_Iteration_End_DValid_Out2 = 0;
    }

    buffer_full_in1_out1[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].size() >= OM_FIFO_SIZE);
    buffer_full_in1_out2[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].size() >= OM_FIFO_SIZE);
    buffer_full_in2_out1[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].size() >= OM_FIFO_SIZE);
    buffer_full_in2_out2[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].size() >= OM_FIFO_SIZE);

    *Stage_Full1 = buffer_full_in1_out1[Omega_Depth_ID][Omega_Switch_ID] || buffer_full_in1_out2[Omega_Depth_ID][Omega_Switch_ID];
    *Stage_Full2 = buffer_full_in2_out1[Omega_Depth_ID][Omega_Switch_ID] || buffer_full_in2_out2[Omega_Depth_ID][Omega_Switch_ID];
}

void Omega_Network_2_Single(int Omega_Depth_ID, int Omega_Switch_ID,
                            int Push_Flag1, int V_ID_In1, int V_Value_In1, int Pull_First_Flag1, int V_Valid_In1, int Iteration_End_In1, int Iteration_End_DValid_In1,
                            int Push_Flag2, int V_ID_In2, int V_Value_In2, int Pull_First_Flag2, int V_Valid_In2, int Iteration_End_In2, int Iteration_End_DValid_In2,
                            int Next_Stage_Full1, int Next_Stage_Full2,

                            int *Stage_Full1, int *Stage_Full2,
                            int *Om2_Push_Flag_Out1, int *Om2_V_ID_Out1, int *Om2_V_Value_Out1, int *Om2_Pull_First_Flag1, int *Om2_V_Valid_Out1, int *Om2_Iteration_End_Out1, int *Om2_Iteration_End_DValid_Out1,
                            int *Om2_Push_Flag_Out2, int *Om2_V_ID_Out2, int *Om2_V_Value_Out2, int *Om2_Pull_First_Flag2, int *Om2_V_Valid_Out2, int *Om2_Iteration_End_Out2, int *Om2_Iteration_End_DValid_Out2) {

    static queue<int> push_flag_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], id_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], pull_first_flag_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static queue<int> push_flag_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], id_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], pull_first_flag_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static queue<int> push_flag_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], id_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], pull_first_flag_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static queue<int> push_flag_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], id_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], pull_first_flag_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static int buffer_empty_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], buffer_empty_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM];
    static int buffer_empty_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], buffer_empty_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM];
    static int buffer_full_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], buffer_full_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM];
    static int buffer_full_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], buffer_full_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM];

    #if DEBUG
        debug_M8._om2_push_flag_buffer_in1_out1 = push_flag_buffer_in1_out1;
        debug_M8._om2_id_buffer_in1_out1 = id_buffer_in1_out1;
        debug_M8._om2_value_buffer_in1_out1 = value_buffer_in1_out1;
        debug_M8._om2_push_flag_buffer_in2_out1 = push_flag_buffer_in2_out1;
        debug_M8._om2_id_buffer_in2_out1 = id_buffer_in2_out1;
        debug_M8._om2_value_buffer_in2_out1 = value_buffer_in2_out1;
        debug_M8._om2_push_flag_buffer_in1_out2 = push_flag_buffer_in1_out2;
        debug_M8._om2_id_buffer_in1_out2 = id_buffer_in1_out2;
        debug_M8._om2_value_buffer_in1_out2 = value_buffer_in1_out2;
        debug_M8._om2_push_flag_buffer_in2_out2 = push_flag_buffer_in2_out2;
        debug_M8._om2_id_buffer_in2_out2 = id_buffer_in2_out2;
        debug_M8._om2_value_buffer_in2_out2 = value_buffer_in2_out2;
    #endif

    int in1_port_id = Omega_Switch_ID, in2_port_id = Omega_Switch_ID + OMEGA_SWITCH_NUM;
    int in1_src_route = in1_port_id & array2n[OMEGA_DEPTH - 1], in2_src_route = in2_port_id & array2n[OMEGA_DEPTH - 1];
    //both push && pull model are shuffled based on the v_id
    int in1_dst_route = ((V_ID_In1 << Omega_Depth_ID) & array2n[OMEGA_DEPTH - 1]), in2_dst_route =  ((V_ID_In2 << Omega_Depth_ID) & array2n[OMEGA_DEPTH - 1]);
    int in1_route_flag = (in1_src_route ^ in1_dst_route), in2_route_flag = (in2_src_route ^ in2_dst_route);

    buffer_empty_in1_out1[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].size() == 0);
    buffer_empty_in1_out2[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].size() == 0);
    buffer_empty_in2_out1[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].size() == 0);
    buffer_empty_in2_out2[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].size() == 0);

    if (rst_rd) {
        /*buffer initilization*/
        for (int i = 0; i < OMEGA_DEPTH; ++ i) array2n[i] = 1 << i;
        while (!push_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].empty())        push_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!push_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].empty())        push_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!push_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].empty())        push_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!push_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].empty())        push_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].empty())               id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].empty())               id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].empty())               id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].empty())               id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].empty())            value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].empty())            value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].empty())            value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].empty())            value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!pull_first_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].empty())  pull_first_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!pull_first_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].empty())  pull_first_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!pull_first_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].empty())  pull_first_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        while (!pull_first_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].empty())  pull_first_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
    }
    else {
        if (V_Valid_In1) {
            if (in1_route_flag == 0) {
                push_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].push(Push_Flag1);
                id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In1);
                value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In1);
                pull_first_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].push(Pull_First_Flag1);
            }
            else {
                push_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].push(Push_Flag1);
                id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In1);
                value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In1);
                pull_first_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].push(Pull_First_Flag1);
            }
        }
        if (V_Valid_In2) {
            if (in2_route_flag == 0) {
                push_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].push(Push_Flag2);
                id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In2);
                value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In2);
                pull_first_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].push(Pull_First_Flag2);
            }
            else {
                push_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].push(Push_Flag2);
                id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In2);
                value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In2);
                pull_first_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].push(Pull_First_Flag2);
            }
        }
    }

    if (rst_rd || Next_Stage_Full1) {
        *Om2_Push_Flag_Out1 = 0;
        *Om2_V_ID_Out1 = 0;
        *Om2_V_Value_Out1 = 0;
        *Om2_V_Valid_Out1 = 0;
    }
    else {
        if (id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].size() > id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].size()) { //in1_out1 cannot be empty
            *Om2_Push_Flag_Out1 = push_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om2_V_ID_Out1 = id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om2_V_Value_Out1 = value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om2_Pull_First_Flag1 = pull_first_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om2_V_Valid_Out1 = 1;

            push_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
            id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
            value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
            pull_first_flag_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        }
        else {
            if (buffer_empty_in2_out1[Omega_Depth_ID][Omega_Switch_ID]) {
                *Om2_Push_Flag_Out1 = 0;
                *Om2_V_ID_Out1 = 0;
                *Om2_V_Value_Out1 = 0;
                *Om2_V_Valid_Out1 = 0;
            }
            else {
                *Om2_Push_Flag_Out1 = push_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om2_V_ID_Out1 = id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om2_V_Value_Out1 = value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om2_Pull_First_Flag1 = pull_first_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om2_V_Valid_Out1 = 1;

                push_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
                id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
                value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
                pull_first_flag_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
            }
        }
    }

    if (rst_rd || Next_Stage_Full2) {
        *Om2_Push_Flag_Out2 = 0;
        *Om2_V_ID_Out2 = 0;
        *Om2_V_Value_Out2 = 0;
        *Om2_V_Valid_Out2 = 0;
    }
    else {
        if (id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].size() > id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].size()) { //in1_out1 cannot be empty
            *Om2_Push_Flag_Out2 = push_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om2_V_ID_Out2 = id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om2_V_Value_Out2 = value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om2_Pull_First_Flag2 = pull_first_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om2_V_Valid_Out2 = 1;

            push_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
            id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
            value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
            pull_first_flag_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        }
        else {
            if (buffer_empty_in2_out2[Omega_Depth_ID][Omega_Switch_ID]) {
                *Om2_Push_Flag_Out2 = 0;
                *Om2_V_ID_Out2 = 0;
                *Om2_V_Value_Out2 = 0;
                *Om2_V_Valid_Out2 = 0;
            }
            else {
                *Om2_Push_Flag_Out2 = push_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om2_V_ID_Out2 = id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om2_V_Value_Out2 = value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om2_Pull_First_Flag2 = pull_first_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om2_V_Valid_Out2 = 1;

                push_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
                id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
                value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
                pull_first_flag_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
            }
        }
    }

    if (!rst_rd && Iteration_End_In1 && Iteration_End_DValid_In1 && buffer_empty_in1_out1[Omega_Depth_ID][Omega_Switch_ID] && buffer_empty_in2_out1[Omega_Depth_ID][Omega_Switch_ID]) {
        *Om2_Iteration_End_Out1 = 1;
        *Om2_Iteration_End_DValid_Out1 = 1;
    }
    else {
        *Om2_Iteration_End_Out1 = 0;
        *Om2_Iteration_End_DValid_Out1 = 0;
    }

    if (!rst_rd && Iteration_End_In2 && Iteration_End_DValid_In2 && buffer_empty_in1_out2[Omega_Depth_ID][Omega_Switch_ID] && buffer_empty_in2_out2[Omega_Depth_ID][Omega_Switch_ID]) {
        *Om2_Iteration_End_Out2 = 1;
        *Om2_Iteration_End_DValid_Out2 = 1;
    }
    else {
        *Om2_Iteration_End_Out2 = 0;
        *Om2_Iteration_End_DValid_Out2 = 0;
    }

    buffer_full_in1_out1[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].size() >= OM_FIFO_SIZE);
    buffer_full_in1_out2[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].size() >= OM_FIFO_SIZE);
    buffer_full_in2_out1[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].size() >= OM_FIFO_SIZE);
    buffer_full_in2_out2[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].size() >= OM_FIFO_SIZE);

    *Stage_Full1 = buffer_full_in1_out1[Omega_Depth_ID][Omega_Switch_ID] || buffer_full_in1_out2[Omega_Depth_ID][Omega_Switch_ID];
    *Stage_Full2 = buffer_full_in2_out1[Omega_Depth_ID][Omega_Switch_ID] || buffer_full_in2_out2[Omega_Depth_ID][Omega_Switch_ID];
}
