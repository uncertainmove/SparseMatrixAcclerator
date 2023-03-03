#include "Accelerator.h"
#include "debug.h"
#include "MEM.h"
#include "Framework.h"
#include <bits/stdc++.h>

using namespace std;

//Predefined Value
int MAX_CYCLE = 30000000;

//Global Parameter
int clk;
int rst_rd;
int rst_root;
int ROOT_ID = 380019;
int vertex_updated = 0;
int MAX_ITERATION = 10;
int M2_max_buffer_size;
int iteration_end_flag = 0;
int Offset_URAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1][2]; //22 bits * 2
vector<int> Edge_MEM[PSEUDO_CHANNEL_NUM];
//int Edge_MEM[PSEUDO_CHANNEL_NUM][MAX_EDGE_ADDR];
int VERTEX_BRAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1]; //4 bits
int First_Edge_BRAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1]; //20 bits
int VERTEX_DEGREE[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1]; //仅用于预处理
bitmap Init_Bitmap[CORE_NUM][BitMap_Compressed_NUM + 1];

int Csr_Offset[MAX_VERTEX_NUM + 1];
vector<int> Ori_Edge_MEM;
int VTX_NUM, EDGE_NUM;


int P1_Push_Flag_rd[CORE_NUM], P1_Active_V_ID_rd[CORE_NUM], P1_Active_V_Value_rd[CORE_NUM], P1_Active_V_Pull_First_Flag_rd[CORE_NUM], P1_Active_V_DValid_rd[CORE_NUM],
    P1_Iteration_End_rd[CORE_NUM], P1_Iteration_End_Dvalid_rd[CORE_NUM];
int P1_Global_Iteration_ID_rd[CORE_NUM];
Powerlaw_Vid_Set P1_Global_Powerlaw_V_ID_rd[CORE_NUM];
Powerlaw_Vvisit_Set P1_Global_Powerlaw_V_Visit_rd[CORE_NUM];
int P1_Push_Flag_wr[CORE_NUM], P1_Active_V_ID_wr[CORE_NUM], P1_Active_V_Value_wr[CORE_NUM], P1_Active_V_Pull_First_Flag_wr[CORE_NUM], P1_Active_V_DValid_wr[CORE_NUM],
    P1_Iteration_End_wr[CORE_NUM], P1_Iteration_End_Dvalid_wr[CORE_NUM];
int P1_Global_Iteration_ID_wr[CORE_NUM];
Powerlaw_Vid_Set P1_Global_Powerlaw_V_ID_wr[CORE_NUM];
Powerlaw_Vvisit_Set P1_Global_Powerlaw_V_Visit_wr[CORE_NUM];

int P2_Stage_Full_rd[CORE_NUM],
    P2_Push_Flag_rd[CORE_NUM], P2_Active_V_ID_rd[CORE_NUM], P2_Active_V_Value_rd[CORE_NUM], P2_Active_V_Offset_Addr_rd[CORE_NUM], P2_Active_V_Pull_First_Flag_rd[CORE_NUM], P2_Active_V_DValid_rd[CORE_NUM],
    P2_Iteration_End_rd[CORE_NUM], P2_Iteration_End_DValid_rd[CORE_NUM];
int P2_Global_Iteration_ID_rd[CORE_NUM];
Powerlaw_Vid_Set P2_Global_Powerlaw_V_ID_rd[CORE_NUM];
Powerlaw_Vvisit_Set P2_Global_Powerlaw_V_Visit_rd[CORE_NUM];
int P2_Stage_Full_wr[CORE_NUM],
    P2_Push_Flag_wr[CORE_NUM], P2_Active_V_ID_wr[CORE_NUM], P2_Active_V_Value_wr[CORE_NUM], P2_Active_V_Offset_Addr_wr[CORE_NUM], P2_Active_V_Pull_First_Flag_wr[CORE_NUM], P2_Active_V_DValid_wr[CORE_NUM],
    P2_Iteration_End_wr[CORE_NUM], P2_Iteration_End_DValid_wr[CORE_NUM];
int P2_Global_Iteration_ID_wr[CORE_NUM];
Powerlaw_Vid_Set P2_Global_Powerlaw_V_ID_wr[CORE_NUM];
Powerlaw_Vvisit_Set P2_Global_Powerlaw_V_Visit_wr[CORE_NUM];

int Offset_Uram_Active_V_LOffset_rd[CORE_NUM], Offset_Uram_Active_V_ROffset_rd[CORE_NUM], Offset_Uram_Active_V_DValid_rd[CORE_NUM];
int Offset_Uram_Active_V_LOffset_wr[CORE_NUM], Offset_Uram_Active_V_ROffset_wr[CORE_NUM], Offset_Uram_Active_V_DValid_wr[CORE_NUM];

int P4_Stage_Full_rd[CORE_NUM],
    P4_Rd_HBM_Edge_Addr_rd[CORE_NUM], P4_HBM_Push_Flag_rd[CORE_NUM], P4_HBM_Active_V_ID_rd[CORE_NUM], P4_HBM_Active_V_Value_rd[CORE_NUM], P4_Rd_HBM_Edge_Valid_rd[CORE_NUM],
    P4_Rd_BRAM_Edge_Addr_rd[CORE_NUM], P4_BRAM_Push_Flag_rd[CORE_NUM], P4_BRAM_Active_V_ID_rd[CORE_NUM], P4_BRAM_Active_V_Value_rd[CORE_NUM], P4_Rd_BRAM_Edge_Valid_rd[CORE_NUM],
    P4_Iteration_End_rd[CORE_NUM], P4_Iteration_End_DValid_rd[CORE_NUM];
int P4_Global_Iteration_ID_rd[CORE_NUM];
Powerlaw_Vid_Set P4_Global_Powerlaw_V_ID_rd[CORE_NUM];
Powerlaw_Vvisit_Set P4_Global_Powerlaw_V_Visit_rd[CORE_NUM];
int P4_Stage_Full_wr[CORE_NUM],
    P4_Rd_HBM_Edge_Addr_wr[CORE_NUM], P4_HBM_Push_Flag_wr[CORE_NUM], P4_HBM_Active_V_ID_wr[CORE_NUM], P4_HBM_Active_V_Value_wr[CORE_NUM], P4_Rd_HBM_Edge_Valid_wr[CORE_NUM],
    P4_Rd_BRAM_Edge_Addr_wr[CORE_NUM], P4_BRAM_Push_Flag_wr[CORE_NUM], P4_BRAM_Active_V_ID_wr[CORE_NUM], P4_BRAM_Active_V_Value_wr[CORE_NUM], P4_Rd_BRAM_Edge_Valid_wr[CORE_NUM],
    P4_Iteration_End_wr[CORE_NUM], P4_Iteration_End_DValid_wr[CORE_NUM];
BitVector_16 P4_Rd_HBM_Edge_Mask_rd[CORE_NUM];
BitVector_16 P4_Rd_HBM_Edge_Mask_wr[CORE_NUM];
int P4_Global_Iteration_ID_wr[CORE_NUM];
Powerlaw_Vid_Set P4_Global_Powerlaw_V_ID_wr[CORE_NUM];
Powerlaw_Vvisit_Set P4_Global_Powerlaw_V_Visit_wr[CORE_NUM];

int P5_Stage_Full_rd[CORE_NUM],
    // P5_Rd_HBM_Edge_Addr_rd[PSEUDO_CHANNEL_NUM], P5_Rd_HBM_Edge_Valid_rd[PSEUDO_CHANNEL_NUM],
    P5_Push_Flag_rd[CORE_NUM], P5_Active_V_ID_rd[CORE_NUM], P5_Active_V_Value_rd[CORE_NUM], P5_Active_V_DValid_rd[CORE_NUM],
    P5_Iteration_End_rd[CORE_NUM], P5_Iteration_End_DValid_rd[CORE_NUM];
int P5_Global_Iteration_ID_rd[CORE_NUM];
Powerlaw_Vid_Set P5_Global_Powerlaw_V_ID_rd[CORE_NUM];
Powerlaw_Vvisit_Set P5_Global_Powerlaw_V_Visit_rd[CORE_NUM];
int P5_Stage_Full_wr[CORE_NUM],
    // P5_Rd_HBM_Edge_Addr_wr[PSEUDO_CHANNEL_NUM], P5_Rd_HBM_Edge_Valid_wr[PSEUDO_CHANNEL_NUM],
    P5_Push_Flag_wr[CORE_NUM], P5_Active_V_ID_wr[CORE_NUM], P5_Active_V_Value_wr[CORE_NUM], P5_Active_V_DValid_wr[CORE_NUM],
    P5_Iteration_End_wr[CORE_NUM], P5_Iteration_End_DValid_wr[CORE_NUM];
int P5_Global_Iteration_ID_wr[CORE_NUM];
Powerlaw_Vid_Set P5_Global_Powerlaw_V_ID_wr[CORE_NUM];
Powerlaw_Vvisit_Set P5_Global_Powerlaw_V_Visit_wr[CORE_NUM];

int P5_2_Stage_Full_rd[CORE_NUM],
    P5_2_Push_Flag_rd[CORE_NUM], P5_2_Active_V_id_rd[CORE_NUM], P5_2_Active_V_Value_rd[CORE_NUM], P5_2_Acitve_V_Edge_rd[CORE_NUM], P5_2_Active_V_DValid_rd[CORE_NUM],
    P5_2_Iteration_End_rd[CORE_NUM], P5_2_Iteration_End_DValid_rd[CORE_NUM];
int P5_2_Stage_Full_wr[CORE_NUM],
    P5_2_Push_Flag_wr[CORE_NUM], P5_2_Active_V_id_wr[CORE_NUM], P5_2_Active_V_Value_wr[CORE_NUM], P5_2_Acitve_V_Edge_wr[CORE_NUM], P5_2_Active_V_DValid_wr[CORE_NUM],
    P5_2_Iteration_End_wr[CORE_NUM], P5_2_Iteration_End_DValid_wr[CORE_NUM];

int HBM_Interface_Full_rd[PSEUDO_CHANNEL_NUM],
    HBM_Interface_Rd_HBM_Edge_Addr_rd[PSEUDO_CHANNEL_NUM], HBM_Interface_Rd_HBM_Edge_Valid_rd[PSEUDO_CHANNEL_NUM],
    HBM_Interface_Active_V_Edge_rd[CORE_NUM], HBM_Interface_Active_V_Edge_Valid_rd[CORE_NUM];
int HBM_Interface_Full_wr[PSEUDO_CHANNEL_NUM],
    HBM_Interface_Rd_HBM_Edge_Addr_wr[PSEUDO_CHANNEL_NUM], HBM_Interface_Rd_HBM_Edge_Valid_wr[PSEUDO_CHANNEL_NUM],
    HBM_Interface_Active_V_Edge_wr[CORE_NUM], HBM_Interface_Active_V_Edge_Valid_wr[CORE_NUM];

int HBM_Controller_Full_rd[PSEUDO_CHANNEL_NUM], HBM_Controller_DValid_rd[PSEUDO_CHANNEL_NUM];
Cacheline_16 HBM_Controller_Edge_rd[PSEUDO_CHANNEL_NUM];
int HBM_Controller_Full_wr[PSEUDO_CHANNEL_NUM], HBM_Controller_DValid_wr[PSEUDO_CHANNEL_NUM];
Cacheline_16 HBM_Controller_Edge_wr[PSEUDO_CHANNEL_NUM];

int P6_Stage_Full1_rd[PSEUDO_CHANNEL_NUM], P6_Stage_Full2_rd[CORE_NUM];
    // P6_Push_Flag_rd[CORE_NUM], P6_Update_V_ID_rd[CORE_NUM], P6_Update_V_Value_rd[CORE_NUM], P6_Pull_First_Flag_rd[CORE_NUM], P6_Update_V_DValid_rd[CORE_NUM],
    // P6_Iteration_End_rd[CORE_NUM], P6_Iteration_End_DValid_rd[CORE_NUM];
int P6_Global_Iteration_ID_rd[CORE_NUM];
Powerlaw_Vid_Set P6_Global_Powerlaw_V_ID_rd[CORE_NUM];
Powerlaw_Vvisit_Set P6_Global_Powerlaw_V_Visit_rd[CORE_NUM];
int P6_Stage_Full1_wr[PSEUDO_CHANNEL_NUM], P6_Stage_Full2_wr[CORE_NUM];
    // P6_Push_Flag_wr[CORE_NUM], P6_Update_V_ID_wr[CORE_NUM], P6_Update_V_Value_wr[CORE_NUM], P6_Pull_First_Flag_wr[CORE_NUM], P6_Update_V_DValid_wr[CORE_NUM],
    // P6_Iteration_End_wr[CORE_NUM], P6_Iteration_End_DValid_wr[CORE_NUM];
int P6_Global_Iteration_ID_wr[CORE_NUM];
Powerlaw_Vid_Set P6_Global_Powerlaw_V_ID_wr[CORE_NUM];
Powerlaw_Vvisit_Set P6_Global_Powerlaw_V_Visit_wr[CORE_NUM];

// int Om1_Stage_Full_rd[CORE_NUM], Om2_Stage_Full_rd[CORE_NUM],
//     Om1_Push_Flag_rd[CORE_NUM], Om1_Update_V_ID_rd[CORE_NUM], Om1_Update_V_Value_rd[CORE_NUM], Om1_Update_V_Pull_First_Flag_rd[CORE_NUM], Om1_Update_V_DValid_rd[CORE_NUM], Om1_Iteration_End_rd[CORE_NUM], Om1_Iteration_End_DValid_rd[CORE_NUM],
//     Om2_Push_Flag_rd[CORE_NUM], Om2_Update_V_ID_rd[CORE_NUM], Om2_Update_V_Value_rd[CORE_NUM], Om2_Update_V_Pull_First_Flag_rd[CORE_NUM], Om2_Update_V_DValid_rd[CORE_NUM], Om2_Iteration_End_rd[CORE_NUM], Om2_Iteration_End_DValid_rd[CORE_NUM];
int Om_Global_Iteration_ID_rd[CORE_NUM];
Powerlaw_Vid_Set Om_Global_Powerlaw_V_ID_rd[CORE_NUM];
Powerlaw_Vvisit_Set Om_Global_Powerlaw_V_Visit_rd[CORE_NUM];
// int Om1_Stage_Full_wr[CORE_NUM], Om2_Stage_Full_wr[CORE_NUM],
//     Om1_Push_Flag_wr[CORE_NUM], Om1_Update_V_ID_wr[CORE_NUM], Om1_Update_V_Value_wr[CORE_NUM], Om1_Update_V_Pull_First_Flag_wr[CORE_NUM], Om1_Update_V_DValid_wr[CORE_NUM], Om1_Iteration_End_wr[CORE_NUM], Om1_Iteration_End_DValid_wr[CORE_NUM],
//     Om2_Push_Flag_wr[CORE_NUM], Om2_Update_V_ID_wr[CORE_NUM], Om2_Update_V_Value_wr[CORE_NUM], Om2_Update_V_Pull_First_Flag_wr[CORE_NUM], Om2_Update_V_DValid_wr[CORE_NUM], Om2_Iteration_End_wr[CORE_NUM], Om2_Iteration_End_DValid_wr[CORE_NUM];
int Om_Global_Iteration_ID_wr[CORE_NUM];
Powerlaw_Vid_Set Om_Global_Powerlaw_V_ID_wr[CORE_NUM];
Powerlaw_Vvisit_Set Om_Global_Powerlaw_V_Visit_wr[CORE_NUM];

int P6_Source_Core_Full_rd[CORE_NUM];

int P7_Source_Core_Full_rd[CORE_NUM],
    P7_Rd_Vertex_BRAM_Addr_rd[CORE_NUM], P7_Rd_Vertex_BRAM_Valid_rd[CORE_NUM],
    P7_Wr_Vertex_Bram_Push_Flag_rd[CORE_NUM], P7_Wr_Vertex_BRAM_Addr_rd[CORE_NUM], P7_Wr_Vertex_BRAM_Data_rd[CORE_NUM], P7_Wr_Vertex_BRAM_Pull_First_Flag_rd[CORE_NUM], P7_Wr_Vertex_BRAM_Valid_rd[CORE_NUM], P7_Wr_Vertex_BRAM_Iteration_End_rd[CORE_NUM], P7_Wr_Vertex_BRAM_Iteration_End_DValid_rd[CORE_NUM];
    // P7_Om2_Send_Push_Flag_rd[CORE_NUM], P7_Om2_Send_Update_V_ID_rd[CORE_NUM], P7_Om2_Send_Update_V_Value_rd[CORE_NUM], P7_Om2_Send_Update_V_Pull_First_Flag_rd[CORE_NUM], P7_Om2_Send_Update_V_DValid_rd[CORE_NUM], P7_Om2_Iteration_End_rd[CORE_NUM], P7_Om2_Iteration_End_DValid_rd[CORE_NUM];
int P7_Global_Iteration_ID_rd[CORE_NUM];
Powerlaw_Vid_Set P7_Global_Powerlaw_V_ID_rd[CORE_NUM];
Powerlaw_Vvisit_Set P7_Global_Powerlaw_V_Visit_rd[CORE_NUM];
int P7_Source_Core_Full_wr[CORE_NUM],
    P7_Rd_Vertex_BRAM_Addr_wr[CORE_NUM], P7_Rd_Vertex_BRAM_Valid_wr[CORE_NUM],
    P7_Wr_Vertex_Bram_Push_Flag_wr[CORE_NUM], P7_Wr_Vertex_BRAM_Addr_wr[CORE_NUM], P7_Wr_Vertex_BRAM_Data_wr[CORE_NUM], P7_Wr_Vertex_BRAM_Pull_First_Flag_wr[CORE_NUM], P7_Wr_Vertex_BRAM_Valid_wr[CORE_NUM], P7_Wr_Vertex_BRAM_Iteration_End_wr[CORE_NUM], P7_Wr_Vertex_BRAM_Iteration_End_DValid_wr[CORE_NUM];
    // P7_Om2_Send_Push_Flag_wr[CORE_NUM], P7_Om2_Send_Update_V_ID_wr[CORE_NUM], P7_Om2_Send_Update_V_Value_wr[CORE_NUM], P7_Om2_Send_Update_V_Pull_First_Flag_wr[CORE_NUM], P7_Om2_Send_Update_V_DValid_wr[CORE_NUM], P7_Om2_Iteration_End_wr[CORE_NUM], P7_Om2_Iteration_End_DValid_wr[CORE_NUM];
int P7_Global_Iteration_ID_wr[CORE_NUM];
Powerlaw_Vid_Set P7_Global_Powerlaw_V_ID_wr[CORE_NUM];
Powerlaw_Vvisit_Set P7_Global_Powerlaw_V_Visit_wr[CORE_NUM];

BRAM vtx_bram;
BRAM edge_bram;
Network_reg Nreg_1;
Network_reg Nreg_2;

class Info_P6 : public Info {
public:
    int P6_Push_Flag[CORE_NUM];
    int P6_Update_V_ID[CORE_NUM];
    int P6_Update_V_Value[CORE_NUM] ;
    int P6_Pull_First_Flag[CORE_NUM];
    int P6_Update_V_DValid[CORE_NUM] ;
    int P6_Iteration_End[CORE_NUM];
    int P6_Iteration_End_DValid[CORE_NUM];
    virtual void copy(Info* info) {
        memcpy(this, info, sizeof(*this));
    }
 } info_p6_self_rd,info_p6_self_wr;

class Info_P5 : public Info {
public:
    int P5_Rd_HBM_Edge_Addr[CORE_NUM];
    int P5_Rd_HBM_Edge_Valid[CORE_NUM];
    virtual void copy(Info *info) {
        memcpy(this, info, sizeof(*this));
    }
 } info_p5_self_rd,info_p5_self_wr;

class Info_P7 : public Info{
public:
    int P7_Source_Core_Full[CORE_NUM]; 
    int P7_Om2_Send_Push_Flag[CORE_NUM]; 
    int P7_Om2_Send_Update_V_ID[CORE_NUM]; 
    int P7_Om2_Send_Update_V_Value[CORE_NUM]; 
    int P7_Om2_Send_Update_V_Pull_First_Flag[CORE_NUM]; 
    int P7_Om2_Send_Update_V_DValid[CORE_NUM]; 
    int P7_Om2_Iteration_End[CORE_NUM]; 
    int P7_Om2_Iteration_End_DValid[CORE_NUM]; 
    virtual void copy(Info *info) {
        memcpy(this, info, sizeof(*this));
    }
}info_p7_self_rd,info_p7_self_wr;

class Info_P_Om1 : public Info{
public:
    int Om1_Stage_Full[CORE_NUM];
    int Om1_Push_Flag[CORE_NUM];
    int Om1_Update_V_ID[CORE_NUM];
    int Om1_Update_V_Value[CORE_NUM];
    int Om1_Update_V_Pull_First_Flag[CORE_NUM];
    int Om1_Update_V_DValid[CORE_NUM];
    int Om1_Iteration_End[CORE_NUM];
    int Om1_Iteration_End_DValid[CORE_NUM];
    virtual void copy(Info *info) {
        memcpy(this, info, sizeof(*this));
    }
}info_p_om1_self_rd,info_p_om1_self_wr;

class Info_P_Om2 : public Info{
public:
    int Om2_Stage_Full[CORE_NUM];
    int Om2_Push_Flag[CORE_NUM];
    int Om2_Update_V_ID[CORE_NUM];
    int Om2_Update_V_Value[CORE_NUM];
    int Om2_Update_V_Pull_First_Flag[CORE_NUM];
    int Om2_Update_V_DValid[CORE_NUM];
    int Om2_Iteration_End[CORE_NUM];
    int Om2_Iteration_End_DValid[CORE_NUM];
    virtual void copy(Info *info) {
        memcpy(this, info, sizeof(*this));
    }
}info_p_om2_self_rd,info_p_om2_self_wr;

int vtx_bram_data[CORE_NUM], vtx_bram_data_valid[CORE_NUM];
int vtx_bram_Read_Addr[CORE_NUM], vtx_bram_Read_Addr_Valid[CORE_NUM],
    vtx_bram_Wr_Addr[CORE_NUM], vtx_bram_Wr_Data[CORE_NUM], vtx_bram_Wr_Addr_Valid[CORE_NUM];

int edge_bram_data[CORE_NUM], edge_bram_data_valid[CORE_NUM];
int edge_bram_Read_Addr[CORE_NUM], edge_bram_Read_Addr_Valid[CORE_NUM],
    edge_bram_Wr_Addr[CORE_NUM], edge_bram_Wr_Data[CORE_NUM], edge_bram_Wr_Addr_Valid[CORE_NUM];

int Vertex_BRAM_Data_rd[CORE_NUM], Vertex_BRAM_DValid_rd[CORE_NUM];
int Vertex_BRAM_Data_wr[CORE_NUM], Vertex_BRAM_DValid_wr[CORE_NUM];

int Backend_Active_V_ID_rd[CORE_NUM], Backend_Active_V_Updated_rd[CORE_NUM], Backend_Active_V_Pull_First_Flag_rd[CORE_NUM],Backend_Active_V_DValid_rd[CORE_NUM], Backend_Iteration_End_rd[CORE_NUM], Backend_Iteration_End_DValid_rd[CORE_NUM];
int Backend_Active_V_ID_wr[CORE_NUM], Backend_Active_V_Updated_wr[CORE_NUM], Backend_Active_V_Pull_First_Flag_wr[CORE_NUM], Backend_Active_V_DValid_wr[CORE_NUM], Backend_Iteration_End_wr[CORE_NUM], Backend_Iteration_End_DValid_wr[CORE_NUM];

int AIE_Active_V_ID_rd[CORE_NUM], AIE_Active_V_Updated_rd[CORE_NUM], AIE_Active_V_Pull_First_Flag_rd[CORE_NUM], AIE_Active_V_DValid_rd[CORE_NUM], AIE_Iteration_End_rd[CORE_NUM], AIE_Iteration_End_DValid_rd[CORE_NUM];
int AIE_Active_V_ID_wr[CORE_NUM], AIE_Active_V_Updated_wr[CORE_NUM], AIE_Active_V_Pull_First_Flag_wr[CORE_NUM], AIE_Active_V_DValid_wr[CORE_NUM], AIE_Iteration_End_wr[CORE_NUM], AIE_Iteration_End_DValid_wr[CORE_NUM];

int count_ = 0;

void Compute_Core(Pipe* pipes);
void Noc(Pipe* pipes);
void Mem(Pipe* pipes);

void compute_connect(Pipe* pipes);
void noc_connect(Pipe* pipes);
void mem_connect(Pipe* pipes);

#if DEBUG
int _main(char *off_file, char *edge_file, int root_id, int max_clk)
#else
int main(int argc, char **argv)
#endif
{
    #if DEBUG
        ROOT_ID = root_id;
        MAX_CYCLE = max_clk;
    #endif

    #if (DEBUG)
        Initialize_Input_Graph(off_file, edge_file);
    #else
        Initialize_Input_Graph(argv[1], argv[2]);
    #endif
    Initialize_Offset_Uram(VTX_NUM);
    Initialize_Vertex_bram(ROOT_ID);
    Initialize_Edge_bram(VTX_NUM);
    Initialize_VERTEX_DEGREE(VTX_NUM);
    Initialize_Bitmap(ROOT_ID,VTX_NUM);
    compute_connect(pipe);
    mem_connect(pipe);
    noc_connect(pipe);
    for (clk = 0; clk < MAX_CYCLE; ++ clk) {
        // rst_rd must be reset only one clk after the reseting of rst_root
        rst_root = (clk < 99) ? 1 : 0;
        rst_rd = (clk < 100) ? 1 : 0;

        if(clk % 500 == 0) {
            int local_print_id = PRINT_ID % CORE_NUM;
            // cout << "clk=" << clk << ", updated=" << vertex_updated << ", " << endl;
            printf("clk=%d, updated=%d\n",
                clk, vertex_updated);//, 0, P2_Stage_Full_wr[local_print_id], 0, P4_Stage_Full_wr[local_print_id], P5_Stage_Full_wr[local_print_id], P5_2_Stage_Full_wr[local_print_id], HBM_Interface_Full_wr[local_print_id], P6_Stage_Full1_wr[local_print_id], P6_Stage_Full2_wr[local_print_id], Om1_Stage_Full_wr[local_print_id], Om2_Stage_Full_wr[local_print_id], P7_Source_Core_Full_wr[local_print_id]);
         }
        
        #if DEBUG
            if (!rst_rd && iteration_end_flag) {
                printf("M2_max_buffer_size=%d\n", M2_max_buffer_size);
                return 0;
            }
        #endif

        /*if(!rst_rd){
            for(int i = 0 ;i<CORE_NUM;i++){
                cout<<P1_Active_V_ID_wr[i]<<" ";
            }
            cout<<endl;
        }*/

        Compute_Core(pipe);
        Noc(pipe);
        Mem(pipe);
        for(int i = 0 ; i<5 ; i++){
            pipe[i].exec();
        }
    }

    return 0;
}

void Compute_Core(Pipe* pipes){
    static Info_P5 self_info_p5;
    static Info_P6 self_info_p6;
    static Info_P7 self_info_p7;
    static Info_P_Om1 self_info_om1;
    static Info_P_Om2 self_info_om2;
    
    pipes[3].write(&self_info_om1);
    pipes[4].write(&self_info_om2);
    RD_ACTIVE_VERTEX(AIE_Active_V_ID_rd, AIE_Active_V_Updated_rd, AIE_Active_V_Pull_First_Flag_rd, AIE_Active_V_DValid_rd,
                        AIE_Iteration_End_rd, AIE_Iteration_End_DValid_rd,
                        P2_Stage_Full_rd,

                        P1_Push_Flag_wr, P1_Active_V_ID_wr, P1_Active_V_Value_wr, P1_Active_V_Pull_First_Flag_wr, P1_Active_V_DValid_wr,
                        P1_Iteration_End_wr, P1_Iteration_End_Dvalid_wr,
                        P1_Global_Iteration_ID_wr);

    RD_ACTIVE_VERTEX_GLOBAL_SIGNAL(AIE_Active_V_ID_rd, AIE_Active_V_Updated_rd, AIE_Active_V_DValid_rd,
                                    
                                    P1_Global_Powerlaw_V_ID_wr, P1_Global_Powerlaw_V_Visit_wr);

    //cout << P1_Push_Flag_wr[ROOT_ID % CORE_NUM] << " " << P1_Active_V_ID_wr[ROOT_ID % CORE_NUM] << " " << P1_Active_V_Value_wr[ROOT_ID % CORE_NUM] << " " << P1_Active_V_DValid_wr[ROOT_ID % CORE_NUM] << endl;

    Read_Active_Vertex_Offset(P1_Push_Flag_rd, P1_Active_V_ID_rd, P1_Active_V_Value_rd, P1_Active_V_Pull_First_Flag_rd, P1_Active_V_DValid_rd,
                                P1_Iteration_End_rd, P1_Iteration_End_Dvalid_rd,
                                P4_Stage_Full_rd,

                                P2_Stage_Full_wr,
                                P2_Push_Flag_wr, P2_Active_V_ID_wr, P2_Active_V_Value_wr, P2_Active_V_Offset_Addr_wr, P2_Active_V_Pull_First_Flag_wr, P2_Active_V_DValid_wr,
                                P2_Iteration_End_wr, P2_Iteration_End_DValid_wr);

    Read_Active_Vertex_Offset_Global_Signal(P1_Global_Iteration_ID_rd, P1_Global_Powerlaw_V_ID_rd, P1_Global_Powerlaw_V_Visit_rd,
    
                                            P2_Global_Iteration_ID_wr, P2_Global_Powerlaw_V_ID_wr, P2_Global_Powerlaw_V_Visit_wr);

//        cout << clk << ": " << P2_Stage_Full_wr[ROOT_ID % CORE_NUM] << " " << P2_Push_Flag_wr[ROOT_ID % CORE_NUM] <<  " " << P2_Active_V_ID_wr[ROOT_ID % CORE_NUM] << " " << P2_Active_V_Value_wr[ROOT_ID % CORE_NUM]
//        << " " << P2_Active_V_Offset_Addr_wr[ROOT_ID % CORE_NUM] << " " << P2_Active_V_DValid_wr[ROOT_ID % CORE_NUM] << endl;

    RD_Offset_Uram(P2_Active_V_Offset_Addr_rd, P2_Active_V_DValid_rd,

                    Offset_Uram_Active_V_LOffset_wr, Offset_Uram_Active_V_ROffset_wr, Offset_Uram_Active_V_DValid_wr);
    //if(Offset_Uram_Active_V_DValid_wr[ROOT_ID % CORE_NUM]==1)cout << clk << ": " << Offset_Uram_Active_V_LOffset_wr[ROOT_ID % CORE_NUM] << " " << Offset_Uram_Active_V_ROffset_wr[ROOT_ID % CORE_NUM] <<endl;

    RD_Active_Vertex_Edge(P2_Push_Flag_rd, P2_Active_V_ID_rd, P2_Active_V_Value_rd, P2_Active_V_Pull_First_Flag_rd, P2_Active_V_DValid_rd,
                          P2_Iteration_End_rd, P2_Iteration_End_DValid_rd,
                          Offset_Uram_Active_V_LOffset_rd, Offset_Uram_Active_V_ROffset_rd, Offset_Uram_Active_V_DValid_rd,
                          P5_Stage_Full_rd, P5_2_Stage_Full_rd,

                          P4_Stage_Full_wr,
                          P4_Rd_HBM_Edge_Addr_wr, P4_Rd_HBM_Edge_Mask_wr, P4_HBM_Push_Flag_wr,  P4_HBM_Active_V_ID_wr, P4_HBM_Active_V_Value_wr, P4_Rd_HBM_Edge_Valid_wr,
                          P4_Rd_BRAM_Edge_Addr_wr, P4_BRAM_Push_Flag_wr, P4_BRAM_Active_V_ID_wr, P4_BRAM_Active_V_Value_wr, P4_Rd_BRAM_Edge_Valid_wr,
                          P4_Iteration_End_wr, P4_Iteration_End_DValid_wr);

    RD_Active_Vertex_Edge_Global_Signal(P2_Global_Iteration_ID_rd, P2_Global_Powerlaw_V_ID_rd, P2_Global_Powerlaw_V_Visit_rd,
    
                                        P4_Global_Iteration_ID_wr, P4_Global_Powerlaw_V_ID_wr, P4_Global_Powerlaw_V_Visit_wr);
//        cout << clk << ": " << P4_Stage_Full_wr[28 % CORE_NUM] << " " << P4_HBM_Push_Flag_wr[28 % CORE_NUM] <<  " " << P4_HBM_Active_V_ID_wr[28 % CORE_NUM] << " " << P4_HBM_Active_V_Value_wr[28 % CORE_NUM] << " " << P4_Rd_HBM_Edge_Valid_wr[28 % CORE_NUM]
//        << " " << P4_Rd_HBM_Edge_Addr_wr[28 % CORE_NUM] << endl;
//        for(int i=0;i<16;i++)
//            cout<< P4_Rd_HBM_Edge_Mask_wr[28 % CORE_NUM].flag[i] << " ";
//        cout<<endl;

    Generate_HBM_Edge_Rqst(P4_Rd_HBM_Edge_Addr_rd, P4_Rd_HBM_Edge_Mask_rd, P4_Rd_HBM_Edge_Valid_rd,
                            P4_HBM_Push_Flag_rd, P4_HBM_Active_V_ID_rd, P4_HBM_Active_V_Value_rd,
                            P4_Iteration_End_rd, P4_Iteration_End_DValid_rd,
                            HBM_Interface_Full_rd, P6_Stage_Full1_rd,

                            P5_Stage_Full_wr,
                            self_info_p5.P5_Rd_HBM_Edge_Addr, self_info_p5.P5_Rd_HBM_Edge_Valid,
                            P5_Push_Flag_wr, P5_Active_V_ID_wr, P5_Active_V_Value_wr, P5_Active_V_DValid_wr,
                            P5_Iteration_End_wr, P5_Iteration_End_DValid_wr);

    Generate_HBM_Edge_Rqst_Global_Signal(P4_Global_Iteration_ID_rd, P4_Global_Powerlaw_V_ID_rd, P4_Global_Powerlaw_V_Visit_rd,
    
                                            P5_Global_Iteration_ID_wr, P5_Global_Powerlaw_V_ID_wr, P5_Global_Powerlaw_V_Visit_wr);
//        cout << clk << ": " << P5_Stage_Full_wr[28 % CORE_NUM] << " " << P5_Push_Flag_wr[28 % CORE_NUM] <<  " " << P5_Active_V_ID_wr[28 % CORE_NUM] << " " << P5_Active_V_Value_wr[28 % CORE_NUM] << " " << P5_Active_V_DValid_wr[28 % CORE_NUM]
//        << " " << P5_Rd_HBM_Edge_Addr_wr[(28 % 512) / 16] << " " << P5_Rd_HBM_Edge_Valid_wr[(28 % 512) / 16] << endl;

    Rd_First_Edge_Bram(P4_Rd_BRAM_Edge_Addr_rd, P4_BRAM_Push_Flag_rd, P4_BRAM_Active_V_ID_rd, P4_BRAM_Active_V_Value_rd, P4_Rd_BRAM_Edge_Valid_rd,
                        P4_Iteration_End_rd, P4_Iteration_End_DValid_rd,
                        P6_Stage_Full2_rd,
                        edge_bram_data, edge_bram_data_valid,


                        P5_2_Stage_Full_wr,
                        P5_2_Push_Flag_wr, P5_2_Active_V_id_wr, P5_2_Active_V_Value_wr, P5_2_Acitve_V_Edge_wr, P5_2_Active_V_DValid_wr,
                        P5_2_Iteration_End_wr, P5_2_Iteration_End_DValid_wr);

    ///asynchronous FIFO
    Schedule(P5_Push_Flag_rd, P5_Active_V_ID_rd, P5_Active_V_Value_rd, P5_Active_V_DValid_rd, P5_Iteration_End_rd, P5_Iteration_End_DValid_rd,
             HBM_Interface_Active_V_Edge_rd, HBM_Interface_Active_V_Edge_Valid_rd,
             P5_2_Push_Flag_rd, P5_2_Active_V_id_rd, P5_2_Active_V_Value_rd, P5_2_Acitve_V_Edge_rd, P5_2_Active_V_DValid_rd, P5_2_Iteration_End_rd, P5_2_Iteration_End_DValid_rd,
             P5_Global_Powerlaw_V_ID_rd, P5_Global_Powerlaw_V_Visit_rd,
             self_info_om1.Om1_Stage_Full,

             P6_Stage_Full1_wr, P6_Stage_Full2_wr,
             self_info_p6.P6_Push_Flag, self_info_p6.P6_Update_V_ID, self_info_p6.P6_Update_V_Value, self_info_p6.P6_Pull_First_Flag, self_info_p6.P6_Update_V_DValid,
             self_info_p6.P6_Iteration_End, self_info_p6.P6_Iteration_End_DValid);

    Schedule_Global_Signal(P5_Global_Iteration_ID_rd, P5_Global_Powerlaw_V_ID_rd, P5_Global_Powerlaw_V_Visit_rd,
    
                            P6_Global_Iteration_ID_wr, P6_Global_Powerlaw_V_ID_wr, P6_Global_Powerlaw_V_Visit_wr);



    Omega_Network_Global_Signal(P6_Global_Iteration_ID_rd, P6_Global_Powerlaw_V_ID_rd, P6_Global_Powerlaw_V_Visit_rd,
    
                                Om_Global_Iteration_ID_wr, Om_Global_Powerlaw_V_ID_wr, Om_Global_Powerlaw_V_Visit_wr);

    // printf("clk:%d,Om1_Push_Flag[0]:%d,Update_V_ID[0]:%d,Update_V_DValid[0]:%d\n",clk,self_info_om1.Om1_Push_Flag[0],self_info_om1.Om1_Update_V_ID[0],self_info_om1.Om1_Update_V_DValid[0]);

    Backend_Core(self_info_om1.Om1_Push_Flag, self_info_om1.Om1_Update_V_ID, self_info_om1.Om1_Update_V_Value, self_info_om1.Om1_Update_V_Pull_First_Flag, self_info_om1.Om1_Update_V_DValid, self_info_om1.Om1_Iteration_End, self_info_om1.Om1_Iteration_End_DValid,
                self_info_om2.Om2_Push_Flag, self_info_om2.Om2_Update_V_ID, self_info_om2.Om2_Update_V_Value, self_info_om2.Om2_Update_V_Pull_First_Flag, self_info_om2.Om2_Update_V_DValid, self_info_om2.Om2_Iteration_End, self_info_om2.Om2_Iteration_End_DValid,
                Vertex_BRAM_Data_rd, Vertex_BRAM_DValid_rd,
                Om_Global_Iteration_ID_rd,

                P7_Source_Core_Full_wr,
                P7_Rd_Vertex_BRAM_Addr_wr, P7_Rd_Vertex_BRAM_Valid_wr,
                P7_Wr_Vertex_Bram_Push_Flag_wr, P7_Wr_Vertex_BRAM_Addr_wr, P7_Wr_Vertex_BRAM_Data_wr, P7_Wr_Vertex_BRAM_Pull_First_Flag_wr, P7_Wr_Vertex_BRAM_Valid_wr, P7_Wr_Vertex_BRAM_Iteration_End_wr, P7_Wr_Vertex_BRAM_Iteration_End_DValid_wr,
                self_info_p7.P7_Om2_Send_Push_Flag, self_info_p7.P7_Om2_Send_Update_V_ID, self_info_p7.P7_Om2_Send_Update_V_Value, self_info_p7.P7_Om2_Send_Update_V_Pull_First_Flag, self_info_p7.P7_Om2_Send_Update_V_DValid, self_info_p7.P7_Om2_Iteration_End, self_info_p7.P7_Om2_Iteration_End_DValid);

    Backend_Core_Global_Signal(Om_Global_Iteration_ID_rd, Om_Global_Powerlaw_V_ID_rd, Om_Global_Powerlaw_V_Visit_rd,
    
                                P7_Global_Iteration_ID_wr, P7_Global_Powerlaw_V_ID_wr, P7_Global_Powerlaw_V_Visit_wr);

    Vertex_BRAM(P7_Rd_Vertex_BRAM_Addr_rd, P7_Rd_Vertex_BRAM_Valid_rd,
                P7_Wr_Vertex_Bram_Push_Flag_rd, P7_Wr_Vertex_BRAM_Addr_rd, P7_Wr_Vertex_BRAM_Data_rd, P7_Wr_Vertex_BRAM_Pull_First_Flag_rd, P7_Wr_Vertex_BRAM_Valid_rd,
                P7_Wr_Vertex_BRAM_Iteration_End_rd, P7_Wr_Vertex_BRAM_Iteration_End_DValid_rd,
                P7_Global_Iteration_ID_rd,

                vtx_bram_data, vtx_bram_data_valid,
                vtx_bram_Read_Addr, vtx_bram_Read_Addr_Valid,vtx_bram_Wr_Addr, vtx_bram_Wr_Data, vtx_bram_Wr_Addr_Valid,

                Vertex_BRAM_Data_wr, Vertex_BRAM_DValid_wr,
                Backend_Active_V_ID_wr, Backend_Active_V_Updated_wr, Backend_Active_V_Pull_First_Flag_wr, Backend_Active_V_DValid_wr, Backend_Iteration_End_wr, Backend_Iteration_End_DValid_wr);

    for(int i = 0 ; i<CORE_NUM ; i++){
        if(vtx_bram_data_valid[i]){
            count_ ++;
        }
    }
    if(clk % 500 == 0){
        printf("clk:%d count = %d\n",clk,count_);
    }

    Apply_Iteration_End(Backend_Active_V_ID_rd, Backend_Active_V_Updated_rd, Backend_Active_V_Pull_First_Flag_rd, Backend_Active_V_DValid_rd, Backend_Iteration_End_rd, Backend_Iteration_End_DValid_rd,

                        AIE_Active_V_ID_wr, AIE_Active_V_Updated_wr, AIE_Active_V_Pull_First_Flag_wr, AIE_Active_V_DValid_wr, AIE_Iteration_End_wr, AIE_Iteration_End_DValid_wr);

    pipes[0].read(&self_info_p6);
    pipes[1].read(&self_info_p5);
    pipes[2].read(&self_info_p7);
    for (int i = 0; i < CORE_NUM; ++ i) {
            //edge_bram
            edge_bram_Wr_Addr_Valid[i] = 0;
            // 7
            P1_Push_Flag_rd[i] = P1_Push_Flag_wr[i];
            P1_Active_V_ID_rd[i] = P1_Active_V_ID_wr[i];
            P1_Active_V_Value_rd[i] = P1_Active_V_Value_wr[i];
            P1_Active_V_Pull_First_Flag_rd[i] = P1_Active_V_Pull_First_Flag_wr[i];
            P1_Active_V_DValid_rd[i] = P1_Active_V_DValid_wr[i];
            P1_Iteration_End_rd[i] = P1_Iteration_End_wr[i];
            P1_Iteration_End_Dvalid_rd[i] = P1_Iteration_End_Dvalid_wr[i];
            P1_Global_Iteration_ID_rd[i] = P1_Global_Iteration_ID_wr[i];
            P1_Global_Powerlaw_V_ID_rd[i] = P1_Global_Powerlaw_V_ID_wr[i];
            P1_Global_Powerlaw_V_Visit_rd[i] = P1_Global_Powerlaw_V_Visit_wr[i];

            // 9
            P2_Stage_Full_rd[i] = P2_Stage_Full_wr[i];
            P2_Push_Flag_rd[i] = P2_Push_Flag_wr[i];
            P2_Active_V_ID_rd[i] = P2_Active_V_ID_wr[i];
            P2_Active_V_Value_rd[i] = P2_Active_V_Value_wr[i];
            P2_Active_V_Offset_Addr_rd[i] = P2_Active_V_Offset_Addr_wr[i];
            P2_Active_V_Pull_First_Flag_rd[i] = P2_Active_V_Pull_First_Flag_wr[i];
            P2_Active_V_DValid_rd[i] = P2_Active_V_DValid_wr[i];
            P2_Iteration_End_rd[i] = P2_Iteration_End_wr[i];
            P2_Iteration_End_DValid_rd[i] = P2_Iteration_End_DValid_wr[i];
            P2_Global_Iteration_ID_rd[i] = P2_Global_Iteration_ID_wr[i];
            P2_Global_Powerlaw_V_ID_rd[i] = P2_Global_Powerlaw_V_ID_wr[i];
            P2_Global_Powerlaw_V_Visit_rd[i] = P2_Global_Powerlaw_V_Visit_wr[i];

            // 3
            Offset_Uram_Active_V_LOffset_rd[i] = Offset_Uram_Active_V_LOffset_wr[i];
            Offset_Uram_Active_V_ROffset_rd[i] = Offset_Uram_Active_V_ROffset_wr[i];
            Offset_Uram_Active_V_DValid_rd[i] = Offset_Uram_Active_V_DValid_wr[i];

            // 14
            P4_Stage_Full_rd[i] = P4_Stage_Full_wr[i];
            P4_Rd_HBM_Edge_Addr_rd[i] = P4_Rd_HBM_Edge_Addr_wr[i];
            P4_HBM_Push_Flag_rd[i] = P4_HBM_Push_Flag_wr[i];
            P4_HBM_Active_V_ID_rd[i] = P4_HBM_Active_V_ID_wr[i];
            P4_HBM_Active_V_Value_rd[i] = P4_HBM_Active_V_Value_wr[i];
            P4_Rd_HBM_Edge_Mask_rd[i] = P4_Rd_HBM_Edge_Mask_wr[i];
            P4_Rd_HBM_Edge_Valid_rd[i] = P4_Rd_HBM_Edge_Valid_wr[i];
            P4_Rd_BRAM_Edge_Addr_rd[i] = P4_Rd_BRAM_Edge_Addr_wr[i];
            P4_BRAM_Push_Flag_rd[i] = P4_BRAM_Push_Flag_wr[i];
            P4_BRAM_Active_V_ID_rd[i] = P4_BRAM_Active_V_ID_wr[i];
            P4_BRAM_Active_V_Value_rd[i] = P4_BRAM_Active_V_Value_wr[i];
            P4_Rd_BRAM_Edge_Valid_rd[i] = P4_Rd_BRAM_Edge_Valid_wr[i];
            P4_Iteration_End_rd[i] = P4_Iteration_End_wr[i];
            P4_Iteration_End_DValid_rd[i] = P4_Iteration_End_DValid_wr[i];
            P4_Global_Iteration_ID_rd[i] = P4_Global_Iteration_ID_wr[i];
            P4_Global_Powerlaw_V_ID_rd[i] = P4_Global_Powerlaw_V_ID_wr[i];
            P4_Global_Powerlaw_V_Visit_rd[i] = P4_Global_Powerlaw_V_Visit_wr[i];

            // 9 - 2
            P5_Stage_Full_rd[i] = P5_Stage_Full_wr[i];
            P5_Push_Flag_rd[i] = P5_Push_Flag_wr[i];
            P5_Active_V_ID_rd[i] = P5_Active_V_ID_wr[i];
            P5_Active_V_Value_rd[i] = P5_Active_V_Value_wr[i];
            P5_Active_V_DValid_rd[i] = P5_Active_V_DValid_wr[i];
            P5_Iteration_End_rd[i] = P5_Iteration_End_wr[i];
            P5_Iteration_End_DValid_rd[i] = P5_Iteration_End_DValid_wr[i];
            P5_Global_Iteration_ID_rd[i] = P5_Global_Iteration_ID_wr[i];
            P5_Global_Powerlaw_V_ID_rd[i] = P5_Global_Powerlaw_V_ID_wr[i];
            P5_Global_Powerlaw_V_Visit_rd[i] = P5_Global_Powerlaw_V_Visit_wr[i];

            // 8
            P5_2_Stage_Full_rd[i] = P5_2_Stage_Full_wr[i];
            P5_2_Push_Flag_rd[i] = P5_2_Push_Flag_wr[i];
            P5_2_Active_V_id_rd[i] = P5_2_Active_V_id_wr[i];
            P5_2_Active_V_Value_rd[i] = P5_2_Active_V_Value_wr[i];
            P5_2_Acitve_V_Edge_rd[i] = P5_2_Acitve_V_Edge_wr[i];
            P5_2_Active_V_DValid_rd[i] = P5_2_Active_V_DValid_wr[i];
            P5_2_Iteration_End_rd[i] = P5_2_Iteration_End_wr[i];
            P5_2_Iteration_End_DValid_rd[i] = P5_2_Iteration_End_DValid_wr[i];


            // 2
            HBM_Interface_Active_V_Edge_rd[i] = HBM_Interface_Active_V_Edge_wr[i];
            HBM_Interface_Active_V_Edge_Valid_rd[i] = HBM_Interface_Active_V_Edge_Valid_wr[i];

            // 9 - 1 - 7(network)
            P6_Stage_Full2_rd[i] = P6_Stage_Full2_wr[i];
            P6_Global_Iteration_ID_rd[i] = P6_Global_Iteration_ID_wr[i];
            P6_Global_Powerlaw_V_ID_rd[i] = P6_Global_Powerlaw_V_ID_wr[i];
            P6_Global_Powerlaw_V_Visit_rd[i] = P6_Global_Powerlaw_V_Visit_wr[i];

            // 16
            // Om1_Stage_Full_rd[i] = Om1_Stage_Full_wr[i];
            // Om2_Stage_Full_rd[i] = Om2_Stage_Full_wr[i];
            // Om1_Push_Flag_rd[i] = Om1_Push_Flag_wr[i];
            // Om1_Update_V_ID_rd[i] = Om1_Update_V_ID_wr[i];
            // Om1_Update_V_Value_rd[i] = Om1_Update_V_Value_wr[i];
            // Om1_Update_V_Pull_First_Flag_rd[i] = Om1_Update_V_Pull_First_Flag_wr[i];
            // Om1_Update_V_DValid_rd[i] = Om1_Update_V_DValid_wr[i];
            // Om2_Push_Flag_rd[i] = Om2_Push_Flag_wr[i];
            // Om2_Update_V_ID_rd[i] = Om2_Update_V_ID_wr[i];
            // Om2_Update_V_Value_rd[i] = Om2_Update_V_Value_wr[i];
            // Om2_Update_V_Pull_First_Flag_rd[i] = Om2_Update_V_Pull_First_Flag_wr[i];
            // Om2_Update_V_DValid_rd[i] = Om2_Update_V_DValid_wr[i];
            // Om1_Iteration_End_rd[i] = Om1_Iteration_End_wr[i];
            // Om1_Iteration_End_DValid_rd[i] = Om1_Iteration_End_DValid_wr[i];
            // Om2_Iteration_End_rd[i] = Om2_Iteration_End_wr[i];
            // Om2_Iteration_End_DValid_rd[i] = Om2_Iteration_End_DValid_wr[i];
            Om_Global_Iteration_ID_rd[i] = Om_Global_Iteration_ID_wr[i];
            Om_Global_Powerlaw_V_ID_rd[i] = Om_Global_Powerlaw_V_ID_wr[i];
            Om_Global_Powerlaw_V_Visit_rd[i] = Om_Global_Powerlaw_V_Visit_wr[i];

            // 17 - 2 - 5 -1(Network)
            P7_Rd_Vertex_BRAM_Addr_rd[i] = P7_Rd_Vertex_BRAM_Addr_wr[i];
            P7_Rd_Vertex_BRAM_Valid_rd[i] = P7_Rd_Vertex_BRAM_Valid_wr[i];
            P7_Wr_Vertex_Bram_Push_Flag_rd[i] = P7_Wr_Vertex_Bram_Push_Flag_wr[i];
            P7_Wr_Vertex_BRAM_Addr_rd[i] = P7_Wr_Vertex_BRAM_Addr_wr[i];
            P7_Wr_Vertex_BRAM_Data_rd[i] = P7_Wr_Vertex_BRAM_Data_wr[i];
            P7_Wr_Vertex_BRAM_Pull_First_Flag_rd[i] = P7_Wr_Vertex_BRAM_Pull_First_Flag_wr[i];
            P7_Wr_Vertex_BRAM_Valid_rd[i] = P7_Wr_Vertex_BRAM_Valid_wr[i];
            P7_Wr_Vertex_BRAM_Iteration_End_rd[i] = P7_Wr_Vertex_BRAM_Iteration_End_wr[i];
            P7_Wr_Vertex_BRAM_Iteration_End_DValid_rd[i] = P7_Wr_Vertex_BRAM_Iteration_End_DValid_wr[i];
            P7_Global_Iteration_ID_rd[i] = P7_Global_Iteration_ID_wr[i];
            P7_Global_Powerlaw_V_ID_rd[i] = P7_Global_Powerlaw_V_ID_wr[i];
            P7_Global_Powerlaw_V_Visit_rd[i] = P7_Global_Powerlaw_V_Visit_wr[i];

            // 2
            Vertex_BRAM_Data_rd[i] = Vertex_BRAM_Data_wr[i];
            Vertex_BRAM_DValid_rd[i] = Vertex_BRAM_DValid_wr[i];

            // 6
            Backend_Active_V_ID_rd[i] = Backend_Active_V_ID_wr[i];
            Backend_Active_V_Updated_rd[i] = Backend_Active_V_Updated_wr[i];
            Backend_Active_V_Pull_First_Flag_rd[i] = Backend_Active_V_Pull_First_Flag_wr[i];
            Backend_Active_V_DValid_rd[i] = Backend_Active_V_DValid_wr[i];
            Backend_Iteration_End_rd[i] = Backend_Iteration_End_wr[i];
            Backend_Iteration_End_DValid_rd[i] = Backend_Iteration_End_DValid_wr[i];

            // 6
            AIE_Active_V_ID_rd[i] = AIE_Active_V_ID_wr[i];
            AIE_Active_V_Updated_rd[i] = AIE_Active_V_Updated_wr[i];
            AIE_Active_V_Pull_First_Flag_rd[i] = AIE_Active_V_Pull_First_Flag_wr[i];
            AIE_Active_V_DValid_rd[i] = AIE_Active_V_DValid_wr[i];
            AIE_Iteration_End_rd[i] = AIE_Iteration_End_wr[i];
            AIE_Iteration_End_DValid_rd[i] = AIE_Iteration_End_DValid_wr[i];
        }

    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {

        HBM_Interface_Full_rd[i] = HBM_Interface_Full_wr[i];
        HBM_Controller_Full_rd[i] = HBM_Controller_Full_wr[i];
        HBM_Controller_DValid_rd[i] = HBM_Controller_DValid_wr[i];

        for (int j = 0; j < GROUP_CORE_NUM; ++ j) HBM_Controller_Edge_rd[i].data[j] = HBM_Controller_Edge_wr[i].data[j];

        P6_Stage_Full1_rd[i] = P6_Stage_Full1_wr[i];
    }

}

void Noc(Pipe* pipes){
    static Info_P6 self_info_p6;
    static Info_P_Om1 self_info_om1;
    static Info_P_Om2 self_info_om2;
    static Info_P7 self_info_p7;
    
    pipes[0].write(&self_info_p6);
    pipes[2].write(&self_info_p7);

    Network(self_info_p6.P6_Push_Flag, self_info_p6.P6_Update_V_ID, self_info_p6.P6_Update_V_Value, self_info_p6.P6_Pull_First_Flag, self_info_p6.P6_Update_V_DValid, self_info_p6.P6_Iteration_End, self_info_p6.P6_Iteration_End_DValid,
            P7_Source_Core_Full_rd,

            self_info_om1.Om1_Stage_Full,
            self_info_om1.Om1_Push_Flag, self_info_om1.Om1_Update_V_ID, self_info_om1.Om1_Update_V_Value, self_info_om1.Om1_Update_V_Pull_First_Flag, self_info_om1.Om1_Update_V_DValid, self_info_om1.Om1_Iteration_End, self_info_om1.Om1_Iteration_End_DValid,
            Nreg_1,
            0,0);

    Network(self_info_p7.P7_Om2_Send_Push_Flag, self_info_p7.P7_Om2_Send_Update_V_ID, self_info_p7.P7_Om2_Send_Update_V_Value, self_info_p7.P7_Om2_Send_Update_V_Pull_First_Flag, self_info_p7.P7_Om2_Send_Update_V_DValid, self_info_p7.P7_Om2_Iteration_End, self_info_p7.P7_Om2_Iteration_End_DValid,
            P6_Source_Core_Full_rd,

            self_info_om2.Om2_Stage_Full,
            self_info_om2.Om2_Push_Flag, self_info_om2.Om2_Update_V_ID, self_info_om2.Om2_Update_V_Value, self_info_om2.Om2_Update_V_Pull_First_Flag, self_info_om2.Om2_Update_V_DValid, self_info_om2.Om2_Iteration_End, self_info_om2.Om2_Iteration_End_DValid,
            Nreg_2,
            1,0);
    // self_info_om1.Om1_Push_Flag[0] = rand();
    // self_info_om1.Om1_Push_Flag[0] = rand();
    // printf("NOC:clk:%d,Om1_Push_Flag[0]:%d,Update_V_ID[0]:%d,Update_V_DValid[0]:%d\n",clk,self_info_om1.Om1_Push_Flag[0],self_info_om1.Om1_Update_V_ID[0],self_info_om1.Om1_Update_V_DValid[0]);
    for (int i = 0; i < CORE_NUM; ++ i) {
        // 9 - 1 - 1 - 3
        P6_Source_Core_Full_rd[i] = 0;
        // P6_Push_Flag_rd[i] = P6_Push_Flag_wr[i];
        // P6_Update_V_ID_rd[i] = P6_Update_V_ID_wr[i];
        // P6_Update_V_Value_rd[i] = P6_Update_V_Value_wr[i];
        // P6_Pull_First_Flag_rd[i] = P6_Pull_First_Flag_wr[i];
        // P6_Update_V_DValid_rd[i] = P6_Update_V_DValid_wr[i];
        // P6_Iteration_End_rd[i] = P6_Iteration_End_wr[i];
        // P6_Iteration_End_DValid_rd[i] = P6_Iteration_End_DValid_wr[i];

        // 2 + 5 + 1 
        // P7_Source_Core_Full_rd[i] = P7_Source_Core_Full_wr[i];
        // P7_Om2_Send_Push_Flag_rd[i] = P7_Om2_Send_Push_Flag_wr[i];
        // P7_Om2_Send_Update_V_ID_rd[i] = P7_Om2_Send_Update_V_ID_wr[i];
        // P7_Om2_Send_Update_V_Value_rd[i] = P7_Om2_Send_Update_V_Value_wr[i];
        // P7_Om2_Send_Update_V_Pull_First_Flag_rd[i] = P7_Om2_Send_Update_V_Pull_First_Flag_wr[i];
        // P7_Om2_Send_Update_V_DValid_rd[i] = P7_Om2_Send_Update_V_DValid_wr[i];
        // P7_Om2_Iteration_End_rd[i] = P7_Om2_Iteration_End_wr[i];
        // P7_Om2_Iteration_End_DValid_rd[i] = P7_Om2_Iteration_End_DValid_wr[i];


    }
    pipes[3].read(&self_info_om1);
    pipes[4].read(&self_info_om2);
}

void Mem( Pipe* pipes ){
    static Info_P5 self_info_p5;
    pipes[1].write(&self_info_p5);
    vtx_bram.BRAM_IP(rst_rd,
                    vtx_bram_Read_Addr, vtx_bram_Read_Addr_Valid,
                    vtx_bram_Wr_Addr, vtx_bram_Wr_Data, vtx_bram_Wr_Addr_Valid,

                    vtx_bram_data, vtx_bram_data_valid);

    edge_bram.BRAM_IP(rst_rd,
                   P4_Rd_BRAM_Edge_Addr_rd, P4_Rd_BRAM_Edge_Valid_rd,
                   edge_bram_Wr_Addr, edge_bram_Wr_Data, edge_bram_Wr_Addr_Valid,

                   edge_bram_data, edge_bram_data_valid);
    
    ///asynchronous FIFO
    HBM_Interface(self_info_p5.P5_Rd_HBM_Edge_Addr, self_info_p5.P5_Rd_HBM_Edge_Valid,
                  HBM_Controller_Edge_rd, HBM_Controller_DValid_rd,
                  HBM_Controller_Full_rd,

                  HBM_Interface_Full_wr,
                  HBM_Interface_Rd_HBM_Edge_Addr_wr, HBM_Interface_Rd_HBM_Edge_Valid_wr,
                  HBM_Interface_Active_V_Edge_wr, HBM_Interface_Active_V_Edge_Valid_wr);


    HBM_Controller_IP(HBM_Interface_Rd_HBM_Edge_Addr_rd, HBM_Interface_Rd_HBM_Edge_Valid_rd,

                     HBM_Controller_Full_wr,
                     HBM_Controller_Edge_wr, HBM_Controller_DValid_wr);

    //  connnect
    for(int i = 0 ; i<CORE_NUM ; i++){
        edge_bram_Wr_Addr[i] = 0; 
        edge_bram_Wr_Data[i] = 0; 
        edge_bram_Wr_Addr_Valid[i] = 0;
    }
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        // P5_Rd_HBM_Edge_Addr_rd[i] = P5_Rd_HBM_Edge_Addr_wr[i];
        // P5_Rd_HBM_Edge_Valid_rd[i] = P5_Rd_HBM_Edge_Valid_wr[i];
        HBM_Interface_Rd_HBM_Edge_Addr_rd[i] = HBM_Interface_Rd_HBM_Edge_Addr_wr[i];
        HBM_Interface_Rd_HBM_Edge_Valid_rd[i] = HBM_Interface_Rd_HBM_Edge_Valid_wr[i];

    }
}

void compute_connect(Pipe* pipes){
    pipes[0].bind(&info_p6_self_wr,&info_p6_self_rd);
    pipes[1].bind(&info_p5_self_wr,&info_p5_self_rd);
    pipes[2].bind(&info_p7_self_wr,&info_p7_self_rd);
}

void mem_connect(Pipe* pipes){

}

void noc_connect(Pipe* pipes){
    pipes[3].bind(&info_p_om1_self_wr,&info_p_om1_self_rd);
    pipes[4].bind(&info_p_om2_self_wr,&info_p_om2_self_rd);
}