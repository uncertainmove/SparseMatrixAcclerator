#include "memory.h"
#include "parameter.h"
#include "cpu.h"
#include "debug.h"

#if DEBUG
    debug_HBM_Interface debug_M6;
#endif

using namespace std;

extern int clk;
extern int rst_rd;
extern vector<int> Edge_MEM[PSEUDO_CHANNEL_NUM];

pair<uint64_t, int> rq_reorder_buffer[PSEUDO_CHANNEL_NUM][32];
int rq_reorder_buffer_rd[PSEUDO_CHANNEL_NUM], rq_reorder_buffer_wr[PSEUDO_CHANNEL_NUM];

//HBM理论带宽：16*64B*1GHz = 953.67GB/s

void dummy_call_back_0(uint64_t addr) {
    int channel_id = (addr >> 11) & ((1 << 3) - 1);
    uint64_t ori_addr = (addr & ((1 << 11) - 1)) + (addr >> 14 << 11);
    bool finding = false;
    for (int i = rq_reorder_buffer_rd[channel_id]; i != rq_reorder_buffer_wr[channel_id]; i = (i + 1) % 32) {
        if ((rq_reorder_buffer[channel_id][i].first == ori_addr || rq_reorder_buffer[channel_id][i].first + CACHELINE_LEN * 4 == ori_addr) &&
                rq_reorder_buffer[channel_id][i].second != 2) {
            rq_reorder_buffer[channel_id][i].second += 1;
            finding = true;
            break;
        }
    }
    DEBUG_MSG(!finding, "[ERROR] Dramsim return addr not matched in reorder buffer!");
    return;
}

void HBM_Interface_Send_Rqst_Single(
    int Channel_ID,
    int Front_Rd_HBM_Edge_Addr, int Front_Rd_HBM_Edge_Valid,
    int HBM_Controller_Full,

    int *Stage_Full,
    int *Rd_HBM_Edge_Addr, int *Rd_HBM_Edge_Valid);

void HBM_Controller_IP(
    int Channel_ID,
    int Rd_HBM_Edge_Addr, int Rd_HBM_Edge_Valid,

    int *HBM_Controller_Full,
    Cacheline_16 *HBM_Controller_Edge, int *HBM_Controller_DValid);

void HBM_Interface_Recv_Edge_Single(
    int Channel_ID,
    Cacheline_16 HBM_Controller_Edge, int HBM_Controller_DValid,

    Cacheline_16 *Active_V_Edge, int *Active_V_Edge_Valid);

void HBM_Interface(
        int Front_Rd_HBM_Edge_Addr[], int Front_Rd_HBM_Edge_Valid[],
        Cacheline_16 HBM_Controller_Edge[], int HBM_Controller_DValid[],
        int HBM_Controller_Full[],

        int *Stage_Full,
        int *Rd_HBM_Edge_Addr, int *Rd_HBM_Edge_Valid,
        int *Active_V_Edge, int *Active_V_Edge_Valid) {

    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        Cacheline_16 tmp_active_v_edge;
        int tmp_active_v_edge_valid;

        HBM_Interface_Send_Rqst_Single(
            i,
            Front_Rd_HBM_Edge_Addr[i], Front_Rd_HBM_Edge_Valid[i],
            HBM_Controller_Full[i],

            &Stage_Full[i],
            &Rd_HBM_Edge_Addr[i], &Rd_HBM_Edge_Valid[i]);

        HBM_Interface_Recv_Edge_Single(
            i,
            HBM_Controller_Edge[i], HBM_Controller_DValid[i],

            &tmp_active_v_edge, &tmp_active_v_edge_valid);

        for (int j = 0; j < CACHELINE_LEN; ++ j) {
            Active_V_Edge[i * GROUP_CORE_NUM + j] = tmp_active_v_edge.data[j];
            Active_V_Edge_Valid[i * GROUP_CORE_NUM + j] = tmp_active_v_edge_valid;
        }
    }
}

void HBM_Interface_Send_Rqst_Single(
    int Channel_ID,
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

void HBM_Interface_Recv_Edge_Single(
        int Channel_ID,
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

void HBM_Controller_IP (
        int Rd_HBM_Edge_Addr[], int Rd_HBM_Edge_Valid[],

        int *HBM_Controller_Full,
        Cacheline_16 *HBM_Controller_Edge, int *HBM_Controller_DValid) {

    static int rd_hbm_edge_addr[PSEUDO_CHANNEL_NUM], rd_hbm_edge_valid[PSEUDO_CHANNEL_NUM];
    static queue<uint64_t> transaction_addr_buffer[PSEUDO_CHANNEL_NUM];

    static dramsim3::MemorySystem dramsys_0("../3rd/DRAMsim3-master/configs/HBM2_4Gb_x128.ini", ".", dummy_call_back_0, dummy_call_back_0);
    static dramsim3::Config dramsys_config("../3rd/DRAMsim3-master/configs/HBM2_4Gb_x128.ini", ".");

    for(int Channel_ID = 0; Channel_ID < PSEUDO_CHANNEL_NUM; ++Channel_ID){
        if(rst_rd){
        }
        else{
            if (Rd_HBM_Edge_Valid[Channel_ID]) {
                // one addr means 4 byte
                // use pseudo channel
                uint64_t real_addr = Rd_HBM_Edge_Addr[Channel_ID] * CACHELINE_LEN * 4 * 2;
                transaction_addr_buffer[Channel_ID].push(real_addr);
                transaction_addr_buffer[Channel_ID].push(real_addr + CACHELINE_LEN * 4);
            }
        }
    }

    for(int Channel_ID = 0; Channel_ID < PSEUDO_CHANNEL_NUM; Channel_ID++){
        if(rst_rd){
            rq_reorder_buffer_rd[Channel_ID] = 0;
            rq_reorder_buffer_wr[Channel_ID] = 0;
        } else {
            // wr reoder buffer
            if (!transaction_addr_buffer[Channel_ID].empty() && ((rq_reorder_buffer_wr[Channel_ID] + 1) % 32) != rq_reorder_buffer_rd[Channel_ID]) {
                uint64_t addr = transaction_addr_buffer[Channel_ID].front();
                uint64_t tmp_address = ((uint64_t(Channel_ID) << 11) + (addr & ((1 << 11) - 1)) + ((addr >> 11) << 14));//21bit
                bool ok;
                ok = dramsys_0.AddTransaction(tmp_address,false);
                int channel = dramsys_0.dram_system_->GetChannel(tmp_address);

                DEBUG_MSG(Channel_ID != channel, "[ERROR] Dramsim channel not matched!");
                
                if (ok) {
                    if (addr % (CACHELINE_LEN * 4 * 2) == 0) {
                        rq_reorder_buffer[Channel_ID][rq_reorder_buffer_wr[Channel_ID]] = pair<int, int>(addr, 0); // addr, valid
                        rq_reorder_buffer_wr[Channel_ID] = (rq_reorder_buffer_wr[Channel_ID] + 1) % 32;
                    }
                    transaction_addr_buffer[Channel_ID].pop();
                }
            }

            // rd reorder buffer
            if (rq_reorder_buffer_rd[Channel_ID] != rq_reorder_buffer_wr[Channel_ID] &&
                    rq_reorder_buffer[Channel_ID][rq_reorder_buffer_rd[Channel_ID]].second == 2) {
                uint64_t addr = rq_reorder_buffer[Channel_ID][rq_reorder_buffer_rd[Channel_ID]].first;
                rd_hbm_edge_addr[Channel_ID] = addr / CACHELINE_LEN / 4 / 2;
                rd_hbm_edge_valid[Channel_ID] = 1;

                rq_reorder_buffer_rd[Channel_ID] = (rq_reorder_buffer_rd[Channel_ID] + 1) % 32;
            } else {
                rd_hbm_edge_addr[Channel_ID] = 0;
                rd_hbm_edge_valid[Channel_ID] = 0;
            }
        }
    }
        
    for (int dram_tick = 0 ; dram_tick < 10; dram_tick++){
        dramsys_0.ClockTick();
    }

    for (int Channel_ID = 0; Channel_ID < PSEUDO_CHANNEL_NUM; Channel_ID++) {
        if (rst_rd) {
            for (int i = 0; i < CACHELINE_LEN; ++ i) HBM_Controller_Edge[Channel_ID].data[i] = 0;
            HBM_Controller_DValid[Channel_ID] = 0;
        }
        else {
            if (rd_hbm_edge_valid[Channel_ID]) {
                for (int i = 0; i < CACHELINE_LEN; ++ i) {
                    // printf("%d\n", Rd_HBM_Edge_Addr[Channel_ID]);
                    HBM_Controller_Edge[Channel_ID].data[i] = Edge_MEM[Channel_ID][rd_hbm_edge_addr[Channel_ID] * CACHELINE_LEN + i];
                }
                HBM_Controller_DValid[Channel_ID] = 1;
            }
            else{
                for (int i = 0; i < CACHELINE_LEN; ++ i) HBM_Controller_Edge[Channel_ID].data[i] = 0;
                HBM_Controller_DValid[Channel_ID] = 0;
            }
        }

        HBM_Controller_Full[Channel_ID] = (transaction_addr_buffer[Channel_ID].size() > FIFO_SIZE);
    }
}

void HBM_Controller_IP_Ideal (
        int Rd_HBM_Edge_Addr[], int Rd_HBM_Edge_Valid[],

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

