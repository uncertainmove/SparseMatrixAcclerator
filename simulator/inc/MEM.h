#include "Accelerator.h"

extern int vertex_updated;
struct BRAM{
    int bram[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1];

    BRAM(){

    }
    void BRAM_IP(int rst_rd,
                int Tmp_rd_addr[], int Tmp_rd_addr_valid[],
                int Tmp_wr_addr[], int tmp_wr_data[], int Tmp_wr_addr_valid[],

                int *Tmp_bram_data, int *Tmp_bram_data_valid) {
        for (int i = 0; i < CORE_NUM; ++ i) {
            if (rst_rd) {
                Tmp_bram_data[i] = 0;
                Tmp_bram_data_valid[i] = 0;
            }
            else {
                if (Tmp_rd_addr_valid[i]) {
                    if (Tmp_rd_addr[i] == -2) {
                        Tmp_bram_data[i] = -2;
                    } else {
                        Tmp_bram_data[i] = bram[i][Tmp_rd_addr[i]];
                    }
                    Tmp_bram_data_valid[i] = 1;
                } else {
                    Tmp_bram_data[i] = 0;
                    Tmp_bram_data_valid[i] = 0;
                }

                if (Tmp_wr_addr_valid[i]) {
                    #if DEBUG
                        //last_update_cycle = clk;
                    #endif
                    #if (DEBUG && PRINT_CONS)
                        if (Tmp_wr_addr[i] * CORE_NUM + Core_ID == PRINT_ID) {
                            printf("clk=%d, update vertex bram: id=%d, value=%d\n", clk, PRINT_ID, tmp_wr_data);
                        }
                    #endif
                    if (tmp_wr_data[i] == -1) {
                        printf("catch -1\n");
                        exit (-1);
                    }
                    if(bram[i][Tmp_wr_addr[i]] == -1 || tmp_wr_data[i] < bram[i][Tmp_wr_addr[i]]) {
                        vertex_updated++;
                    }
                    bram[i][Tmp_wr_addr[i]] = tmp_wr_data[i];
                }
            }
        }
    }
};

struct URAM{
    int uram[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1];

    URAM(){

    }
    void URAM_IP(int rst_rd,int Core_ID,
                int Tmp_rd_addr, int Tmp_rd_addr_valid,
                int Tmp_wr_addr, int tmp_wr_data, int Tmp_wr_addr_valid,

                int *Tmp_bram_data, int *Tmp_bram_data_valid) {
        if (rst_rd) {
            *Tmp_bram_data = 0;
            *Tmp_bram_data_valid = 0;
        }
        else {
            if (Tmp_rd_addr_valid) {
                if (Tmp_rd_addr == -2) {
                    *Tmp_bram_data = -2;
                } else {
                    *Tmp_bram_data = uram[Core_ID][Tmp_rd_addr];
                }
                *Tmp_bram_data_valid = 1;
            } else {
                *Tmp_bram_data = 0;
                *Tmp_bram_data_valid = 0;
            }

            if (Tmp_wr_addr_valid) {
                #if DEBUG
                    //last_update_cycle = clk;
                #endif
                #if (DEBUG && PRINT_CONS)
                    if (Tmp_wr_addr * CORE_NUM + Core_ID == PRINT_ID) {
                        printf("clk=%d, update vertex bram: id=%d, value=%d\n", clk, PRINT_ID, tmp_wr_data);
                    }
                #endif
                if (tmp_wr_data == -1) {
                    printf("catch -1\n");
                    exit (-1);
                }
                uram[Core_ID][Tmp_wr_addr] = tmp_wr_data;
            }
        }
    }
};

// struct HBM{
// private:
//     vector<int> HBM_MEM[PSEUDO_CHANNEL_NUM];
// public:
//     void init(){

//     }
//     void HBM_Controller_IP(int Rd_HBM_Edge_Addr[], int Rd_HBM_Edge_Valid[],
//                         int *HBM_Controller_Full,
//                         Cacheline_16 *HBM_Controller_Edge, int *HBM_Controller_DValid,
//                         int rst_rd) {
//         for(int Channel_ID = 0; Channel_ID < PSEUDO_CHANNEL_NUM; ++ Channel_ID) {
//             if (rst_rd) {
//                 HBM_Controller_Full[Channel_ID] = 0;
//                 for (int i = 0; i < CACHELINE_LEN; ++ i) HBM_Controller_Edge[Channel_ID].data[i] = 0;
//                 HBM_Controller_DValid[Channel_ID] = 0;
//             }
//             else {
//                 HBM_Controller_Full[Channel_ID] = 0;
//                 if (Rd_HBM_Edge_Valid[Channel_ID]) {
//                     for (int i = 0; i < CACHELINE_LEN; ++ i) {
//                         // printf("%d\n", Rd_HBM_Edge_Addr[Channel_ID]);
//                         HBM_Controller_Edge[Channel_ID].data[i] = Edge_MEM[Channel_ID][Rd_HBM_Edge_Addr[Channel_ID] * CACHELINE_LEN + i];
//                     }
//                     HBM_Controller_DValid[Channel_ID] = 1;
//                 }
//                 else{
//                     for (int i = 0; i < CACHELINE_LEN; ++ i) HBM_Controller_Edge[Channel_ID].data[i] = 0;
//                     HBM_Controller_DValid[Channel_ID] = 0;
//                 }
//             }
//         }
//     }
// };