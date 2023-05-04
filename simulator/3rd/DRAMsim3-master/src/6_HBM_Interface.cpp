#include "Accelerator.h"
// #include "./../ext/headers/args.hxx"
#include "cpu.h"
#if DEBUG
    #include "debug.h"
    debug_HBM_Interface debug_M6;
#endif

extern int clk;
extern int rst_rd;

bool call_back_called_0 = false;
bool call_back_called_1 = false;
bool call_back_called_2 = false;
bool call_back_called_3 = false;
std::queue<uint64_t> addr_queue_0;
std::queue<uint64_t> addr_queue_1;
std::queue<uint64_t> addr_queue_2;
std::queue<uint64_t> addr_queue_3;
std::queue<uint64_t> addr_buffer;

//HBM理论带宽：16*64B*1GHz = 953.67GB/s

void dummy_call_back_0(uint64_t addr) {
    call_back_called_0 = true;
    addr_queue_0.push(addr);
    return;
}

void dummy_call_back_1(uint64_t addr) {
    call_back_called_1 = true;
    addr_queue_1.push(addr);
    return;
}

void dummy_call_back_2(uint64_t addr) {
    call_back_called_2 = true;
    addr_queue_2.push(addr);
    return;
}

void dummy_call_back_3(uint64_t addr) {
    call_back_called_3 = true;
    addr_queue_3.push(addr);
    return;
}

// dramsim3::MemorySystem dramsys_0("/home/sunsss/acc/SparseMatrixAccelerator_now_use/SparseMatrixAcclerator/simulator/DRAMsim3-master/configs/HBM2_4Gb_x128.ini", ".", dummy_call_back_0,dummy_call_back_0);
// dramsim3::MemorySystem dramsys_1("/home/sunsss/acc/SparseMatrixAccelerator_now_use/SparseMatrixAcclerator/simulator/DRAMsim3-master/configs/HBM2_4Gb_x128.ini", ".", dummy_call_back_1,dummy_call_back_1);
// dramsim3::MemorySystem dramsys_2("/home/sunsss/acc/SparseMatrixAccelerator_now_use/SparseMatrixAcclerator/simulator/DRAMsim3-master/configs/HBM2_4Gb_x128.ini", ".", dummy_call_back_2,dummy_call_back_2);
// dramsim3::MemorySystem dramsys_3("/home/sunsss/acc/SparseMatrixAccelerator_now_use/SparseMatrixAcclerator/simulator/DRAMsim3-master/configs/HBM2_4Gb_x128.ini", ".", dummy_call_back_3,dummy_call_back_3);

// dramsim3::Config dramsys_config("/home/sunsss/acc/SparseMatrixAccelerator_now_use/SparseMatrixAcclerator/simulator/DRAMsim3-master/configs/HBM2_4Gb_x128.ini", ".");


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
    static int rd_hbm_edge_addr[PSEUDO_CHANNEL_NUM], rd_hbm_edge_valid[PSEUDO_CHANNEL_NUM];
    static int rd_hbm_channel[PSEUDO_CHANNEL_NUM];
    static queue<uint64_t> transaction_addr_buffer[PSEUDO_CHANNEL_NUM];
    static int _count = 0,_total = 0;

    static dramsim3::MemorySystem dramsys_0("../configs/HBM2_4Gb_x128.ini", ".", dummy_call_back_0,dummy_call_back_0);
    static dramsim3::MemorySystem dramsys_1("../configs/HBM2_4Gb_x128.ini", ".", dummy_call_back_1,dummy_call_back_1);
    static dramsim3::MemorySystem dramsys_2("../configs/HBM2_4Gb_x128.ini", ".", dummy_call_back_2,dummy_call_back_2);
    static dramsim3::MemorySystem dramsys_3("../configs/HBM2_4Gb_x128.ini", ".", dummy_call_back_3,dummy_call_back_3);

    static dramsim3::Config dramsys_config("../configs/HBM2_4Gb_x128.ini", ".");


    // channel
    // hex_addr >>= config_.shift_bits;  //
    // hex_addr >>= shift_bits;
    // int channel = (hex_addr >> ch_pos) & ch_mask;
    // int rank = (hex_addr >> ra_pos) & ra_mask;
    // int bg = (hex_addr >> bg_pos) & bg_mask;
    // int ba = (hex_addr >> ba_pos) & ba_mask;
    // int ro = (hex_addr >> ro_pos) & ro_mask;
    // int co = (hex_addr >> co_pos) & co_mask;
    
    // request_size_bytes = bus_width / 8 * BL;  bus_width = 128  ; BL = 4;
    // shift_bits = LogBase2(request_size_bytes); // shift_bits = 6
    int tmp_shift_bits = dramsys_config.shift_bits;
    int tmp_ch_pos = dramsys_config.ch_pos;
    int tmp_ch_mask = dramsys_config.ch_mask;
    // if( clk == 0 ){
    //     printf("shift_bits:%d,ch_pos:%d,ch_mask:%ld,ra_pos:%d,ra_mask:%ld,bg_pos:%d,bg_mask:%ld,ro_pos:%d,ro_mask:%ld,co_pos:%d,co_mask:%ld\n",dramsys_config.shift_bits,
    //     dramsys_config.ch_pos,dramsys_config.ch_mask,dramsys_config.ra_pos,dramsys_config.ra_mask,dramsys_config.bg_pos,dramsys_config.bg_mask,dramsys_config.ro_pos,
    //     dramsys_config.ro_mask,dramsys_config.co_pos,dramsys_config.co_mask);
    // }
    for(int Channel_ID = 0; Channel_ID < PSEUDO_CHANNEL_NUM; ++ Channel_ID){
        if(rst_rd){
        }
        else{
            if(Rd_HBM_Edge_Valid[Channel_ID]){
                transaction_addr_buffer[Channel_ID].push(Rd_HBM_Edge_Addr[Channel_ID]);
                // uint64_t tmp_address = ((uint64_t(Channel_ID)<<tmp_shift_bits)<<tmp_ch_pos)+(uint64_t(Rd_HBM_Edge_Addr[Channel_ID])<<tmp_shift_bits);

                uint64_t tmp_channel = Channel_ID >> 2;
                uint64_t tmp_address = ((uint64_t(tmp_channel)<<11)+((transaction_addr_buffer[Channel_ID].front()&((1<<5)-1))<<6)+((transaction_addr_buffer[Channel_ID].front()>>5)<<16));//21bit
                bool ok;
                // Map[channel][addr] += 1;
                // printf("done 0 channel:%ld address:%ld\n",tmp_channel,tmp_address);
                if(Channel_ID % 4 == 0){
                    ok = dramsys_0.AddTransaction(tmp_address,false);
                }else if(Channel_ID % 4 == 1){
                    ok = dramsys_1.AddTransaction(tmp_address,false);
                }else if(Channel_ID % 4 == 2){
                    ok = dramsys_2.AddTransaction(tmp_address,false);
                }else{
                    ok = dramsys_3.AddTransaction(tmp_address,false);
                }
                if(ok){
                    transaction_addr_buffer[Channel_ID].pop();
                    _count ++;
                    _total ++;
                }
                // printf("add transaction: Channel_ID:%d HBM_Addr:%d\n",Channel_ID,Rd_HBM_Edge_Addr[Channel_ID]);
            }
            rd_hbm_edge_addr[Channel_ID] = 0;
            rd_hbm_edge_valid[Channel_ID] = 0;
            rd_hbm_channel[Channel_ID] = 0 ;
        }
    }
    //clock default to be the same with cpuclock;if not , use cpuclock/clock to make sure
    // printf("begin to clock\n");
    dramsys_0.ClockTick();
    dramsys_1.ClockTick();
    dramsys_2.ClockTick();
    dramsys_3.ClockTick();

    if(call_back_called_0 || !addr_queue_0.empty()){
        while(!addr_queue_0.empty()){
            // uint64_t tmp_channel = (addr_queue.front() >> 21) & ((1<<5)-1);
            // uint64_t tmp_addr = (addr_queue.front()) & ((1 << 21) - 1);
            uint64_t tmp_channel = (((addr_queue_0.front() >> 11) & ((1<<3)-1))<<2)+0;
            // uint64_t tmp_channel = ((addr_queue_0.front() >> 11) & ((1<<5)-1));
            uint64_t tmp_addr = ((addr_queue_0.front() >> 6) & ((1 << 5) - 1))+(((addr_queue_0.front() >> 16) & ((1 << 16) - 1))<<5);
            if(!rd_hbm_channel[tmp_channel]){
                rd_hbm_channel[tmp_channel] = 1;
                rd_hbm_edge_addr[tmp_channel] = tmp_addr;
                rd_hbm_edge_valid[tmp_channel] = 1;
                addr_queue_0.pop();
                _count--;
            }else{
                addr_buffer.push(addr_queue_0.front());
                addr_queue_0.pop();
            }
            // printf("finish transaction: Channel_ID:%ld HBM_Addr:%ld\n",tmp_channel,tmp_addr);
        }
        while(!addr_buffer.empty()){
            addr_queue_0.push(addr_buffer.front());
            addr_buffer.pop();
        }
        call_back_called_0 = false;
    }

    if(call_back_called_1 || !addr_queue_1.empty()){
        while(!addr_queue_1.empty()){
            // uint64_t tmp_channel = (addr_queue.front() >> 21) & ((1<<5)-1);
            // uint64_t tmp_addr = (addr_queue.front()) & ((1 << 21) - 1);
            uint64_t tmp_channel = (((addr_queue_1.front() >> 11) & ((1<<3)-1))<<2)+1;
            uint64_t tmp_addr = ((addr_queue_1.front() >> 6) & ((1 << 5) - 1))+(((addr_queue_1.front() >> 16) & ((1 << 16) - 1))<<5);
            if(!rd_hbm_channel[tmp_channel]){
                rd_hbm_channel[tmp_channel] = 1;
                rd_hbm_edge_addr[tmp_channel] = tmp_addr;
                rd_hbm_edge_valid[tmp_channel] = 1;
                // printf("finish transaction: Channel_ID:%ld HBM_Addr:%ld\n",tmp_channel,tmp_addr);
                addr_queue_1.pop();
                _count--;
            }else{
                addr_buffer.push(addr_queue_1.front());
                addr_queue_1.pop();
            }
        }
        while(!addr_buffer.empty()){
            addr_queue_1.push(addr_buffer.front());
            addr_buffer.pop();
        }
        call_back_called_1 = false;
    }

    if(call_back_called_2 || !addr_queue_2.empty()){
        while(!addr_queue_2.empty()){
            // uint64_t tmp_channel = (addr_queue.front() >> 21) & ((1<<5)-1);
            // uint64_t tmp_addr = (addr_queue.front()) & ((1 << 21) - 1);
            uint64_t tmp_channel = (((addr_queue_2.front() >> 11) & ((1<<3)-1))<<2)+2;
            uint64_t tmp_addr = ((addr_queue_2.front() >> 6) & ((1 << 5) - 1))+(((addr_queue_2.front() >> 16) & ((1 << 16) - 1))<<5);
            if(!rd_hbm_channel[tmp_channel]){
                rd_hbm_channel[tmp_channel] = 1;
                rd_hbm_edge_addr[tmp_channel] = tmp_addr;
                rd_hbm_edge_valid[tmp_channel] = 1;
                // printf("finish transaction: Channel_ID:%ld HBM_Addr:%ld\n",tmp_channel,tmp_addr);
                addr_queue_2.pop();
                _count--;
            }else{
                addr_buffer.push(addr_queue_2.front());
                addr_queue_2.pop();
            }
        }
        while(!addr_buffer.empty()){
            addr_queue_2.push(addr_buffer.front());
            addr_buffer.pop();
        }
        call_back_called_2 = false;
    }

    if(call_back_called_3 || !addr_queue_3.empty()){
        while(!addr_queue_3.empty()){
            // uint64_t tmp_channel = (addr_queue.front() >> 21) & ((1<<5)-1);
            // uint64_t tmp_addr = (addr_queue.front()) & ((1 << 21) - 1);
            uint64_t tmp_channel = (((addr_queue_3.front() >> 11) & ((1<<3)-1))<<2)+3;
            uint64_t tmp_addr = ((addr_queue_3.front() >> 6) & ((1 << 5) - 1))+(((addr_queue_3.front() >> 16) & ((1 << 16) - 1))<<5);
            if(!rd_hbm_channel[tmp_channel]){
                rd_hbm_channel[tmp_channel] = 1;
                rd_hbm_edge_addr[tmp_channel] = tmp_addr;
                rd_hbm_edge_valid[tmp_channel] = 1;
                // printf("finish transaction: Channel_ID:%ld HBM_Addr:%ld\n",tmp_channel,tmp_addr);
                addr_queue_3.pop();
                _count--;
            }else{
                addr_buffer.push(addr_queue_3.front());
                addr_queue_3.pop();
            }
        }
        while(!addr_buffer.empty()){
            addr_queue_3.push(addr_buffer.front());
            addr_buffer.pop();
        }
        call_back_called_3 = false;
    }

    int len = 0;
    for(int i = 0 ; i<PSEUDO_CHANNEL_NUM ; i++){
        len += transaction_addr_buffer[i].size();
    }

    if(clk % 500 == 0){
        printf("dram exist visit:%d,visit_queue_length:%d,total_visit_times:%d\n",_count,len,_total);
    }
    // if(clk % 500 == 0){
    //     printf("count : %d,addr_queue_empty:%d\n",_count,addr_queue_0.empty());
    //     if(!addr_queue.empty() && clk > 14000){
    //         uint64_t tmp_channel = (addr_queue.front() >> 11) & ((1<<5)-1);
    //         uint64_t tmp_addr = ((addr_queue.front() >> 6) & ((1 << 5) - 1))+(((addr_queue.front() >> 18) & ((1 << 16) - 1))<<5);
    //         printf("tmp_channel:%ld,tmp_addr:%ld\n",tmp_channel,tmp_addr);
    //     }
    // }

    for(int Channel_ID = 0; Channel_ID < PSEUDO_CHANNEL_NUM; ++ Channel_ID) {
        if (rst_rd) {
            HBM_Controller_Full[Channel_ID] = 0;
            for (int i = 0; i < CACHELINE_LEN; ++ i) HBM_Controller_Edge[Channel_ID].data[i] = 0;
            HBM_Controller_DValid[Channel_ID] = 0;
        }
        else {
            HBM_Controller_Full[Channel_ID] = 0;
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
    }
}
