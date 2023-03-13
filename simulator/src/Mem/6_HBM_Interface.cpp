#include "Accelerator.h"
#if DEBUG
    #include "debug.h"
    debug_HBM_Interface debug_M6;
#endif

extern int clk;
extern int rst_rd;

using namespace std;

extern vector<int> Edge_MEM[PSEUDO_CHANNEL_NUM];

void HBM_Interface_Send_Rqst_Single(int Channel_ID,
                                    int Front_Rd_HBM_Edge_Addr, int Front_Rd_HBM_Edge_Valid,
                                    int HBM_Controller_Full,

                                    int *Stage_Full,
                                    int *Rd_HBM_Edge_Addr, int *Rd_HBM_Edge_Valid);

void HBM_Controller_IP(int Channel_ID,
                       int Rd_HBM_Edge_Addr, int Rd_HBM_Edge_Valid,

                       int *HBM_Controller_Full,
                       Cacheline_16 *HBM_Controller_Edge, int *HBM_Controller_DValid);

void HBM_Interface_Recv_Edge_Single(int Channel_ID,
                                    Cacheline_16 HBM_Controller_Edge, int HBM_Controller_DValid,

                                    Cacheline_16 *Active_V_Edge, int *Active_V_Edge_Valid);

void HBM_Interface(int Front_Rd_HBM_Edge_Addr[], int Front_Rd_HBM_Edge_Valid[],
                   Cacheline_16 HBM_Controller_Edge[], int HBM_Controller_DValid[],
                   int HBM_Controller_Full[],

                   int *Stage_Full,
                   int *Rd_HBM_Edge_Addr, int *Rd_HBM_Edge_Valid,
                   int *Active_V_Edge, int *Active_V_Edge_Valid) {

    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        Cacheline_16 tmp_active_v_edge;
        int tmp_active_v_edge_valid;

        HBM_Interface_Send_Rqst_Single(i,
                                       Front_Rd_HBM_Edge_Addr[i], Front_Rd_HBM_Edge_Valid[i],
                                       HBM_Controller_Full[i],

                                       &Stage_Full[i],
                                       &Rd_HBM_Edge_Addr[i], &Rd_HBM_Edge_Valid[i]);

        HBM_Interface_Recv_Edge_Single(i,
                                       HBM_Controller_Edge[i], HBM_Controller_DValid[i],

                                       &tmp_active_v_edge, &tmp_active_v_edge_valid);

        for (int j = 0; j < CACHELINE_LEN; ++ j) {
            Active_V_Edge[i * GROUP_CORE_NUM + j] = tmp_active_v_edge.data[j];
            Active_V_Edge_Valid[i * GROUP_CORE_NUM + j] = tmp_active_v_edge_valid;
        }
    }
}

void HBM_Interface_Send_Rqst_Single(int Channel_ID,
                                    int Front_Rd_HBM_Edge_Addr, int Front_Rd_HBM_Edge_Valid,
                                    int HBM_Controller_Full,

                                    int *Stage_Full,
                                    int *Rd_HBM_Edge_Addr, int *Rd_HBM_Edge_Valid) {

    ///asynchronous FIFO
    static queue<int> edge_addr_buffer[PSEUDO_CHANNEL_NUM];
    static int buffer_empty[PSEUDO_CHANNEL_NUM];

    #if DEBUG
        debug_M6._edge_addr_buffer = edge_addr_buffer;
    #endif
    // printf("send\n");

    buffer_empty[Channel_ID] = (edge_addr_buffer[Channel_ID].size() == 0);

    if (rst_rd) {
        /*initialize edge_addr_buffer*/
        while (!edge_addr_buffer[Channel_ID].empty())    edge_addr_buffer[Channel_ID].pop();
    }
    else {
        if (Front_Rd_HBM_Edge_Valid) {
            edge_addr_buffer[Channel_ID].push(Front_Rd_HBM_Edge_Addr);
        }
    }

    if (rst_rd) {
        *Rd_HBM_Edge_Addr = 0;
        *Rd_HBM_Edge_Valid = 0;
    }
    else {
        if (HBM_Controller_Full || buffer_empty[Channel_ID]) {
            *Rd_HBM_Edge_Addr = 0;
            *Rd_HBM_Edge_Valid = 0;
        }
        else {
            *Rd_HBM_Edge_Addr = edge_addr_buffer[Channel_ID].front();
            *Rd_HBM_Edge_Valid = 1;

            edge_addr_buffer[Channel_ID].pop();
        }

    }

    *Stage_Full = (edge_addr_buffer[Channel_ID].size() >= FIFO_SIZE);
}

void HBM_Interface_Recv_Edge_Single(int Channel_ID,
                                    Cacheline_16 HBM_Controller_Edge, int HBM_Controller_DValid,

                                    Cacheline_16 *Active_V_Edge, int *Active_V_Edge_Valid) {

    ///asynchronous FIFO
    static queue<Cacheline_16> edge_buffer[PSEUDO_CHANNEL_NUM];
    static int edge_buffer_empty[PSEUDO_CHANNEL_NUM];

    edge_buffer_empty[Channel_ID] = (edge_buffer[Channel_ID].size() == 0);

    #if DEBUG
        debug_M6._edge_buffer = edge_buffer;
    #endif


    if (rst_rd) {
        /*initialize rr_index_buffer*/
        while (!edge_buffer[Channel_ID].empty())     edge_buffer[Channel_ID].pop();
    }
    else {
        if (HBM_Controller_DValid) {
            edge_buffer[Channel_ID].push(HBM_Controller_Edge);
        }
    }

    if (rst_rd) {
        for (int i = 0; i < CACHELINE_LEN; ++ i){
            Active_V_Edge->data[i] = 0;
        }
        *Active_V_Edge_Valid = 0;
    } else {
        if (edge_buffer_empty[Channel_ID]) {
            for (int i = 0; i < CACHELINE_LEN; ++ i){
                Active_V_Edge->data[i] = 0;
            }
            *Active_V_Edge_Valid = 0;
        }
        else {
            for (int i = 0; i < CACHELINE_LEN; ++ i){
                Active_V_Edge->data[i] = edge_buffer[Channel_ID].front().data[i];
            }
            *Active_V_Edge_Valid = 1;
            edge_buffer[Channel_ID].pop();
        }
    }
}

void HBM_Controller_IP(int Rd_HBM_Edge_Addr[], int Rd_HBM_Edge_Valid[],

                       int *HBM_Controller_Full,
                       Cacheline_16 *HBM_Controller_Edge, int *HBM_Controller_DValid) {
    for(int Channel_ID = 0; Channel_ID < PSEUDO_CHANNEL_NUM; ++ Channel_ID) {
        if (rst_rd) {
            HBM_Controller_Full[Channel_ID] = 0;
            for (int i = 0; i < CACHELINE_LEN; ++ i) HBM_Controller_Edge[Channel_ID].data[i] = 0;
            HBM_Controller_DValid[Channel_ID] = 0;
        }
        else {
            HBM_Controller_Full[Channel_ID] = 0;
            if (Rd_HBM_Edge_Valid[Channel_ID]) {
                for (int i = 0; i < CACHELINE_LEN; ++ i) {
                    // printf("%d\n", Rd_HBM_Edge_Addr[Channel_ID]);
                    HBM_Controller_Edge[Channel_ID].data[i] = Edge_MEM[Channel_ID][Rd_HBM_Edge_Addr[Channel_ID] * CACHELINE_LEN + i];
                }
                HBM_Controller_DValid[Channel_ID] = 1;
            }
            else{
                for (int i = 0; i < CACHELINE_LEN; ++ i) HBM_Controller_Edge[Channel_ID].data[i] = 0;
                HBM_Controller_DValid[Channel_ID] = 0;
            }
        }
    }
}
