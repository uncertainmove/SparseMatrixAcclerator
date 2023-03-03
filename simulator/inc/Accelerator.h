#ifndef ACC_H
#define ACC_H

#define DEBUG 0
//change the macro here
#define PRINT_ID 0
#include "Macro.h"

#if DEBUG
    int _main(char *off_file, char *edge_file, int root_id, int max_clk);
    #define PRINT_CONS 0
    #define PRINT_ID 0
    #define CONTROL_FLAG 1
#endif

#include <bits/stdc++.h>
#include <queue>
using namespace std;
const int FIFO_SIZE = 16;
const int MAX_TASK_NUM = FIFO_SIZE * 2 / 2;
const int OM_FIFO_SIZE = 4;

const int MAX_VERTEX_NUM = 1024 * 1024;
const int MAX_EDGE_NUM = 100000000;
const int CACHELINE_LEN = 16;
const int POWER_LAW_V_NUM = 32;

const int CORE_NUM = 512;
const int BitMap_Compressed_Length = 32;
const int BitMap_Compressed_NUM = 64; // = VERTEX_NUM / (CORE_NUM * BitMap_Compressed_Length)

const int PSEUDO_CHANNEL_NUM = 32;
const int GROUP_CORE_NUM = CORE_NUM / PSEUDO_CHANNEL_NUM;
const int MAX_EDGE_ADDR = MAX_VERTEX_NUM * 32 / PSEUDO_CHANNEL_NUM;

const int OMEGA_DEPTH = 9;
const int OMEGA_SWITCH_NUM = CORE_NUM / 2;

const int VERTEX_NOT_READ[POWER_LAW_V_NUM] = {
    798169, 802022, 859188, 737253, 236717, 800651, 884620, 1038374, 246686, 72581,
    514920, 939780, 433575, 644849, 887126, 817258, 812863, 802479, 701232, 798386,
    1030895, 609384, 504066, 426492, 16463, 644226, 506990, 1045487, 628265, 735673,
    507966, 584249
};

typedef struct BitVector_16{
    bool flag[16];
    BitVector_16() {}
    BitVector_16(bool f) {
        for (int i = 0; i < 16; i++) {
            flag[i] = f;
        }
    }
    bool compare(const BitVector_16 &b) {
        bool res = true;
        for (int i = 0; i < 16; i++) {
            if (flag[i] != b.flag[i]) res = false;
        }
        return res;
    }
    void show() {
        printf("mask:");
        for (int i = 0; i < 16; i++) {
            printf(" %d", flag[i]);
        }
        printf("\n");
    }
}BitVector_16;

typedef struct Cacheline_16{
    int data[16];
    Cacheline_16() {}
    Cacheline_16(int f) {
        for (int i = 0; i < 16; i++) {
            data[i] = f;
        }
    }
    bool compare(const Cacheline_16 &b) {
        bool res = true;
        for (int i = 0; i < 16; i++) {
            if (data[i] != b.data[i]) res = false;
        }
        return res;
    }
    void show() {
        printf("mask:");
        for (int i = 0; i < 16; i++) {
            printf(" %d", data[i]);
        }
        printf("\n");
    }
}Cacheline_16;

typedef struct bitmap {
    bool v[BitMap_Compressed_Length];
    bitmap() {}
    bitmap(bool f) {
        for (int i = 0; i < BitMap_Compressed_Length; i++) {
            v[i] = f;
        }
    }
    bitmap(const bitmap& b) {
        for (int i = 0; i < BitMap_Compressed_Length; i++) {
            v[i] = b.v[i];
        }
    }
    bool compare(const bitmap& b) {
        bool res = true;
        for (int i = 0; i < BitMap_Compressed_Length; i++) {
            if (v[i] != b.v[i]) res = false;
        }
        return res;
    }
    inline void set(bool f) {
        for (int i = 0; i < BitMap_Compressed_Length; i++) {
            v[i] = f;
        }
    }
    void show() {
        printf("bitmap:");
        for (int i = 0; i < BitMap_Compressed_Length; i++) {
            printf(" %d", v[i] == true);
        }
        printf("\n");
    }
}bitmap;

struct Powerlaw_Vid_Set {
    int v[POWER_LAW_V_NUM];
    void set(Powerlaw_Vid_Set& pvs) {
        for (int i = 0; i < POWER_LAW_V_NUM; i++) {
            v[i] = pvs.v[i];
        }
    }
};
struct Powerlaw_Vvisit_Set {
    bool visit[POWER_LAW_V_NUM];
    Powerlaw_Vvisit_Set() {
        for (int i = 0; i < POWER_LAW_V_NUM; i++) {
            visit[i] = false;
        }
    }
    void set(Powerlaw_Vvisit_Set& pvs) {
        for (int i = 0; i < POWER_LAW_V_NUM; i++) {
            visit[i] = pvs.visit[i];
        }
    }
};

struct Network_reg{
    //omega network 
    int tmp_om_push_flag_in[OMEGA_DEPTH + 1][CORE_NUM], tmp_om_v_id_in[OMEGA_DEPTH + 1][CORE_NUM], tmp_om_v_value_in[OMEGA_DEPTH + 1][CORE_NUM], tmp_om_pull_first_flag_in[OMEGA_DEPTH + 1][CORE_NUM], tmp_om_v_valid_in[OMEGA_DEPTH + 1][CORE_NUM],
        tmp_om_iteration_end[OMEGA_DEPTH + 1][CORE_NUM], tmp_om_iteration_end_dvalid[OMEGA_DEPTH + 1][CORE_NUM];

    int tmp_om_stage_full[OMEGA_DEPTH + 1][CORE_NUM];

    int buffer_full_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], buffer_full_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM];
    int buffer_full_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], buffer_full_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM];
    
    //###################### omega network
    queue<int> push_flag_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], id_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], pull_first_flag_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    queue<int> push_flag_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], id_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], pull_first_flag_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    queue<int> push_flag_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], id_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], pull_first_flag_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    queue<int> push_flag_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], id_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], pull_first_flag_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    //######################

};

void Initialize_Input_Graph(char *off_file, char *edge_file);

void Initialize_Offset_Uram(int V_Num);

void Initialize_Vertex_bram(int root_id);

void Initialize_Edge_bram(int V_Num);

void Initialize_VERTEX_DEGREE(int V_Num);

void Initialize_Bitmap(int root_id, int V_Num);

void RD_ACTIVE_VERTEX(int Backend_Active_V_ID[], int Backend_Active_V_Updated[], int Backend_Active_V_Pull_First_Flag[], int Backend_Active_V_DValid[],
                      int Backend_Iteration_End[], int Backend_Iteration_End_DValid[],
                      int NextStage_Full[],

                      int *Push_Flag,
                      int *Active_V_ID, int *Active_V_Value, int *Active_V_Pull_First_Flag, int *Active_V_DValid,
                      int *Iteration_End, int *Iteration_End_DValid,
                      int *Global_Iteration_ID);

void RD_ACTIVE_VERTEX_GLOBAL_SIGNAL(int Backend_Active_V_ID[], int Backend_Active_V_Updated[], int Backend_Active_V_DValid[],

                                    Powerlaw_Vid_Set *Global_Powerlaw_V_ID, Powerlaw_Vvisit_Set *Global_Powerlaw_V_Visit);

void Read_Active_Vertex_Offset(int Front_Push_Flag[], int Front_Active_V_ID[], int Front_Active_V_Value[], int Front_Active_V_Pull_First_Flag[], int Front_Active_V_DValid[],
                               int Front_Iteration_End[], int Front_Iteration_End_DValid[],
                               int NextStage_Full[],

                               int *Stage_Full,
                               int *Push_Flag, int *Active_V_ID, int *Active_V_Value, int *RD_Active_V_Offset_Addr, int *Active_V_Pull_First_Flag, int *Active_V_DValid,
                               int *Iteration_End, int *Iteration_End_DValid);

void Read_Active_Vertex_Offset_Global_Signal(int Front_Global_Iteration_ID[], Powerlaw_Vid_Set Front_Global_Powerlaw_V_ID[], Powerlaw_Vvisit_Set Front_Global_Powerlaw_V_Visit[],

                                            int *Global_Iteration_ID, Powerlaw_Vid_Set *Global_Powerlaw_V_ID, Powerlaw_Vvisit_Set *Global_Powerlaw_V_Visit);

void RD_Offset_Uram(int Front_RD_Active_V_Offset_Addr[], int Front_Active_V_DValid[],

                    int *Active_V_LOffset, int *Active_V_ROffset,int *Active_V_DValid);

void RD_Active_Vertex_Edge(int Front_Push_Flag[], int Front_Active_V_ID[], int Front_Active_V_Value[], int Front_Active_V_Pull_First_Flag[], int Front_Active_V_DValid[],
                           int Front_Iteration_End[], int Front_Iteration_End_DValid[],
                           int Front_Active_V_LOffset[], int Front_Active_V_Roffset[], int Front_Offset_DValid[],
                           int CombineStage_Full[], int BramStage_Full[],

                           int *Stage_Full,
                           int *Rd_HBM_Edge_Addr, BitVector_16 *Rd_HBM_Edge_Mask, int *HBM_Push_Flag, int *HBM_Active_V_ID, int *HBM_Active_V_Value, int *Rd_HBM_Edge_Valid,
                           int *Rd_BRAM_Edge_Addr, int *BRAM_Push_Flag, int *BRAM_Active_V_ID, int *BRAM_Active_V_Value, int *Rd_BRAM_Edge_Valid,
                           int *Iteration_End, int *Iteration_End_DValid);

void RD_Active_Vertex_Edge_Global_Signal(int Front_Global_Iteration_ID[], Powerlaw_Vid_Set Front_Global_Powerlaw_V_ID[], Powerlaw_Vvisit_Set Front_Global_Powerlaw_V_Visit[],

                                            int *Global_Iteration_ID, Powerlaw_Vid_Set *Global_Powerlaw_V_ID, Powerlaw_Vvisit_Set *Global_Powerlaw_V_Visit);

void Generate_HBM_Edge_Rqst(int Front_Rd_HBM_Edge_Addr[], BitVector_16 Front_Rd_HBM_Edge_Mask[], int Front_Rd_HBM_Edge_Valid[],
                            int Front_Push_Flag[], int Front_Active_V_ID[], int Front_Active_V_Value[],
                            int Front_Iteration_End[], int Front_Iteration_End_DValid[],
                            int HBM_Full[], int NextStage_Full[],

                            int *Stage_Full,
                            int *Rd_HBM_Edge_Addr, int *Rd_HBM_Edge_Valid,
                            int *Push_Flag, int *Active_V_ID, int *Active_V_Value, int *Active_V_DValid,
                            int *Iteration_End, int *Iteration_End_DValid);

void Generate_HBM_Edge_Rqst_Global_Signal(int Front_Global_Iteration_ID[], Powerlaw_Vid_Set Front_Global_Powerlaw_V_ID[], Powerlaw_Vvisit_Set Front_Global_Powerlaw_V_Visit[],

                                         int *Global_Iteration_ID, Powerlaw_Vid_Set *Global_Powerlaw_V_ID, Powerlaw_Vvisit_Set *Global_Powerlaw_V_Visit);

void Rd_First_Edge_Bram(int Front_Rd_Edge_Addr[], int Front_Push_Flag[], int Front_Active_V_ID[], int Front_Active_V_Value[], int Front_Rd_Edge_Valid[],
                        int Front_Iteration_End[], int Front_Iteration_End_DValid[],
                        int NextStage_Full[],
                        int edge_bram_data[], int edge_bram_data_valid[],

                        int *Stage_Full,
                        int *Push_Flag, int *Active_V_ID, int *Active_V_Value, int *Active_V_Edge, int *Active_V_DValid,
                        int *Iteration_End, int *Iteration_End_DValid);

void HBM_Interface(int Front_Rd_HBM_Edge_Addr[], int Front_Rd_HBM_Edge_Valid[],
                   Cacheline_16 HBM_Controller_Edge[], int HBM_Controller_DValid[],
                   int HBM_Controller_Full[],

                   int *Stage_Full,
                   int *Rd_HBM_Edge_Addr, int *Rd_HBM_Edge_Valid,
                   int *Active_V_Edge, int *Active_V_Edge_Valid);

void HBM_Controller_IP(int Rd_HBM_Edge_Addr[], int Rd_HBM_Edge_Valid[],

                       int *HBM_Controller_Full,
                       Cacheline_16 *HBM_Controller_Edge, int *HBM_Controller_DValid);

void Schedule(int Front_Push_Flag[], int Front_Active_V_ID[], int Front_Active_V_Value[], int Front_Active_V_DValid[], int Front_Iteration_End[], int Front_Iteration_End_DValid[],
              int HBM_Iterface_Active_V_Edge[], int Active_V_Edge_Valid[],
              int Front2_Push_Flag[], int Front2_Active_V_ID[], int Front2_Active_V_Value[], int Front2_Active_V_Edge[], int Front2_Active_V_DValid[], int Front2_Iteration_End[], int Front2_Iteration_End_DValid[],
              Powerlaw_Vid_Set Front_Global_Powerlaw_V_ID[], Powerlaw_Vvisit_Set Front_Global_Powerlaw_V_Visit[],
              int NextStage_Full[],

              int *Stage_Full1, int *Stage_Full2,
              int *Push_Flag, int *Update_V_ID, int *Update_V_VALUE, int *Pull_First_Flag, int *Update_V_DValid,
              int *Iteration_End, int *Iteration_End_DValid);

void Schedule_Global_Signal(int Front_Global_Iteration_ID[], Powerlaw_Vid_Set Front_Global_Powerlaw_V_ID[], Powerlaw_Vvisit_Set Front_Global_Powerlaw_V_Visit[],

                           int *Global_Iteration_ID, Powerlaw_Vid_Set *Global_Powerlaw_V_ID, Powerlaw_Vvisit_Set *Global_Powerlaw_V_Visit);

void Network(int Om_Front_Push_Flag[], int Om_Front_Update_V_ID[], int Om_Front_Update_V_Value[], int Om_Front_Pull_First_Flag[], int Om_Front_Update_V_DValid[], int Om_Front_Iteration_End[], int Om_Front_Iteration_End_DValid[],
            int Next_Stage_Full[],
            
            int *Stage_Full_Om,
            int *Om_Push_Flag, int *Om_Update_V_ID, int *Om_Update_V_Value, int *Om_Pull_First_Flag, int *Om_Update_V_DValid, int *Om_Iteration_End, int *Om_Iteration_End_DValid,
            Network_reg& Nreg, 
            int switch_router = 0, int network_num = 0);

void Omega_Network_Global_Signal(int Front_Global_Iteration_ID[], Powerlaw_Vid_Set Front_Global_Powerlaw_V_ID[], Powerlaw_Vvisit_Set Front_Global_Powerlaw_V_Visit[],

                                  int *Global_Iteration_ID, Powerlaw_Vid_Set *Global_Powerlaw_V_ID, Powerlaw_Vvisit_Set *Global_Powerlaw_V_Visit);

void Backend_Core(int Om1_Recv_Push_Flag[], int Om1_Recv_Update_V_ID[], int Om1_Recv_Update_V_Value[], int Om1_Recv_Update_V_Pull_First_Flag[], int Om1_Recv_Update_V_DValid[], int Om1_Recv_Iteration_End[], int Om1_Recv_Iteration_End_DValid[],
                  int Om2_Recv_Push_Flag[], int Om2_Recv_Update_V_ID[], int Om2_Recv_Update_V_Value[], int Om2_Recv_Update_V_Pull_First_Flag[], int Om2_Recv_Update_V_DValid[], int Om2_Recv_Iteration_End[], int Om2_Recv_Iteration_End_DValid[],
                  int Vertex_BRAM_Data[], int Vertex_BRAM_DValid[],
                  int Om_Global_Iteration_ID[],

                  int *Source_Core_Full,
                  int *Rd_Vertex_BRAM_Addr, int *Rd_Vertex_BRAM_Valid,
                  int *Wr_Vertex_Bram_Push_Flag, int *Wr_Vertex_BRAM_Addr, int *Wr_Vertex_BRAM_Data, int *Wr_Vertex_BRAM_Pull_First_Flag, int *Wr_Vertex_BRAM_Valid, int *Wr_Vertex_BRAM_Iteration_End, int *Wr_Vertex_BRAM_Iteration_End_DValid,
                  int *Om2_Send_Push_Flag, int *Om2_Send_Update_V_ID, int *Om2_Send_Update_V_Value, int *Om2_Send_Update_V_Pull_First_Flag, int *Om2_Send_Update_V_DValid, int *Om2_Send_Iteration_End, int *Om2_Send_Iteration_End_DValid);

void Backend_Core_Global_Signal(int Front_Global_Iteration_ID[], Powerlaw_Vid_Set Front_Global_Powerlaw_V_ID[], Powerlaw_Vvisit_Set Front_Global_Powerlaw_V_Visit[],

                               int *Global_Iteration_ID, Powerlaw_Vid_Set *Global_Powerlaw_V_ID, Powerlaw_Vvisit_Set *Global_Powerlaw_V_Visit);


void Vertex_BRAM(int Rd_Addr_Src[], int Rd_Valid_Src[],
                 int Wr_Push_Flag_Dst[], int Wr_Addr_Dst[], int Wr_V_Value_Dst[], int Wr_Pull_First_Flag_Dst[], int Wr_Valid_Dst[],
                 int Front_Iteration_End[], int Front_Iteration_End_DValid[],
                 int Front_Iteration_ID[],

                 
                 int vtx_bram_data[], int vtx_bram_data_valid[],
                 int *vtx_bram_Read_Addr, int *vtx_bram_Read_Addr_Valid,int *vtx_bram_Wr_Addr, int *vtx_bram_Wr_Data, int *vtx_bram_Wr_Addr_Valid,

                 int *Src_Recv_Update_V_Value, int *Src_Recv_Update_V_DValid,
                 int *Backend_Active_V_ID, int *Backend_Active_V_Updated, int *Backend_Active_V_Pull_First_Flag, int *Backend_Active_V_DValid, int *Iteration_End, int *Iteration_End_DValid);

void Vertex_BRAM_Global_Signal(int Front_Global_Iteration_ID[], Powerlaw_Vid_Set Front_Global_Powerlaw_V_ID[], Powerlaw_Vvisit_Set Front_Global_Powerlaw_V_Visit[],

                               int *Global_Iteration_ID, Powerlaw_Vid_Set *Global_Powerlaw_V_ID, Powerlaw_Vvisit_Set *Global_Powerlaw_V_Visit);


void Apply_Iteration_End(int Front_Active_V_ID[], int Front_Active_V_Updated[], int Front_Active_V_Pull_First_Flag[], int Front_Active_V_DValid[], int Front_Iteration_End[], int Front_Iteration_End_DValid[],

                         int *Active_V_ID, int *Active_V_Updated, int *Active_V_Pull_First_Flag, int *Active_V_DValid,int *Iteration_End, int *Iteration_End_DValid);

void dump_result(int V_Num);
#endif // ACC_H