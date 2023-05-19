#include "Accelerator.h"

#define cn_size 16

bool call_back_called_0 = false;
bool call_back_called_1 = false;
queue<uint64_t> addr_queue_0;
queue<uint64_t> addr_queue_1;
queue<uint64_t> addr_buffer;

void dummy_call_back_0(uint64_t addr) {
    call_back_called_0 = true;
    // printf("call back addr:%ld\n",addr);
    addr_queue_0.push(addr);
    return;
}

void dummy_call_back_1(uint64_t addr) {
    call_back_called_1 = true;
    // printf("call back addr:%ld\n",addr);
    addr_queue_1.push(addr);
    return;
}



dramsim3::MemorySystem dramsys_0("/home/sunsss/acc/SparseMatrixAccelerator_now_use/SparseMatrixAcclerator/simulator/DRAMsim3-master/configs/HBM2_4Gb_x128.ini", ".", dummy_call_back_0,dummy_call_back_0);
dramsim3::MemorySystem dramsys_1("/home/sunsss/acc/SparseMatrixAccelerator_now_use/SparseMatrixAcclerator/simulator/DRAMsim3-master/configs/HBM2_4Gb_x128.ini", ".", dummy_call_back_1,dummy_call_back_1);

using namespace std;

// int Map[32][200] = {0}; // 95,96
void dram_test(uint64_t channel,uint64_t addr){
    // printf("begin add\n");
    uint64_t tmp_channel = channel >> 1;
    uint64_t tmp_address = ((uint64_t(tmp_channel)<<11)+((uint64_t(addr)&((1<<5)-1))<<6)+((uint64_t(addr)>>5)<<16));
    
    // Map[channel][addr] += 1;
    printf("done 0 channel:%ld address:%ld\n",tmp_channel,tmp_address);
    if(channel % 2){
        dramsys_1.AddTransaction(tmp_address,false);
    }else{
        dramsys_0.AddTransaction(tmp_address,false);
    }
    printf("done 1 channel:%ld address:%ld\n",tmp_channel,tmp_address);
    
}



int main(){
    // if( clk == 0 ){
    //     printf("shift_bits:%d,ch_pos:%d,ch_mask:%ld,ra_pos:%d,ra_mask:%ld,bg_pos:%d,bg_mask:%ld,ro_pos:%d,ro_mask:%ld,co_pos:%d,co_mask:%ld\n",dramsys_config.shift_bits,
    //     dramsys_config.ch_pos,dramsys_config.ch_mask,dramsys_config.ra_pos,dramsys_config.ra_mask,dramsys_config.bg_pos,dramsys_config.bg_mask,dramsys_config.ro_pos,
    //     dramsys_config.ro_mask,dramsys_config.co_pos,dramsys_config.co_mask);
    // }
    
    // dramsim3::MemorySystem dramsys_0("/home/sunsss/acc/SparseMatrixAccelerator_now_use/SparseMatrixAcclerator/simulator/DRAMsim3-master/configs/HBM2_4Gb_x128.ini", ".", dummy_call_back_0,dummy_call_back_0);
    // dramsim3::MemorySystem dramsys_1("/home/sunsss/acc/SparseMatrixAccelerator_now_use/SparseMatrixAcclerator/simulator/DRAMsim3-master/configs/HBM2_4Gb_x128.ini", ".", dummy_call_back_1,dummy_call_back_1);
    int clk = 0;
    if(clk == 0){
        dramsys_0.GetTCK();
        dramsys_0.GetBusBits();
    }
    // printf("begin main,clk:%d\n",clk);
    int HBM_Edge_Valid[cn_size];
    uint64_t Rd_HBM_Edge_Addr[cn_size];
    int rd_hbm_channel_0[cn_size];
    int rd_hbm_channel_1[cn_size];
    uint64_t rd_hbm_edge_addr[cn_size];
    int rd_hbm_edge_valid[cn_size];
    uint64_t addr = 0;
    int _count = 0;
    int rem = 0, total = 0;
    // printf("begin main done 0\n");
    while(true){
        for(int i = 0 ; i<cn_size; i++){
            if(clk < 2000){//95,6
                if(i == 0) addr++;
                // printf("begin main done 0.5\n");
                HBM_Edge_Valid[i] = 1;
                Rd_HBM_Edge_Addr[i] = addr;
                /*addr generate*/
                // addr++;
            }else{
                HBM_Edge_Valid[i] = 0;
            }
        }
        // printf("begin main done 1\n");

        for(int i = 0 ; i<cn_size ; i++){
            // printf("begin main done 2\n");
            if(HBM_Edge_Valid[i]){
                // printf("begin main done 3\n");
                dram_test(i,Rd_HBM_Edge_Addr[i]);

                _count ++;
                total ++;
            }else{
                printf("HBM_Edge_Valid_%d = %d\n",i,HBM_Edge_Valid[i]);
            }
            rd_hbm_channel_0[i] = 0;
            rd_hbm_channel_1[i] = 0;
        }

        if( call_back_called_0 || !addr_queue_0.empty() ){
            while(!addr_queue_0.empty()){
                // assert(addr_queue.front() != 786432);
                // uint64_t tmp_address = ((uint64_t(channel)<<11)+((uint64_t(addr)&((1<<5)-1))<<6)+((uint64_t(addr)>>5)<<16));

                uint64_t tmp_channel = (((addr_queue_0.front() >> 11) & ((1<<3)-1))<<1);
                uint64_t tmp_addr = ((addr_queue_0.front() >> 6) & ((1 << 5) - 1))+(((addr_queue_0.front() >> 16) & ((1 << 16) - 1))<<5);
                if(!rd_hbm_channel_0[tmp_channel]){
                    rd_hbm_channel_0[tmp_channel] = 1;
                    rd_hbm_edge_addr[tmp_channel] = tmp_addr;
                    rd_hbm_edge_valid[tmp_channel] = 1;
                    // Map[tmp_channel][tmp_addr] -= 1;
                    // printf("output channel:%ld addr:%ld tmp_address:%ld\n",tmp_channel,tmp_addr,addr_queue.front());
                    addr_queue_0.pop();
                    _count --;
                    rem ++;
                }else{
                    break;
                }
                // else{
                //     addr_buffer.push(addr_queue.front());
                //     addr_queue.pop();
                // }
            }
            // while(!addr_buffer.empty()){
            //     addr_queue.push(addr_buffer.front());
            //     addr_buffer.pop();
            // }
        }

        if( call_back_called_1 || !addr_queue_1.empty() ){
            while(!addr_queue_1.empty()){
                // assert(addr_queue.front() != 786432);
                // uint64_t tmp_address = ((uint64_t(channel)<<11)+((uint64_t(addr)&((1<<5)-1))<<6)+((uint64_t(addr)>>5)<<16));

                uint64_t tmp_channel = (((addr_queue_1.front() >> 11) & ((1<<3)-1))<<1) + 1;
                uint64_t tmp_addr = ((addr_queue_1.front() >> 6) & ((1 << 5) - 1))+(((addr_queue_1.front() >> 16) & ((1 << 16) - 1))<<5);
                if(!rd_hbm_channel_1[tmp_channel]){
                    rd_hbm_channel_1[tmp_channel] = 1;
                    rd_hbm_edge_addr[tmp_channel] = tmp_addr;
                    rd_hbm_edge_valid[tmp_channel] = 1;
                    // Map[tmp_channel][tmp_addr] -= 1;
                    // printf("output channel:%ld addr:%ld tmp_address:%ld\n",tmp_channel,tmp_addr,addr_queue.front());
                    addr_queue_1.pop();
                    _count --;
                    rem ++;
                }else{
                    break;
                }
                // else{
                //     addr_buffer.push(addr_queue.front());
                //     addr_queue.pop();
                // }
            }
            // while(!addr_buffer.empty()){
            //     addr_queue.push(addr_buffer.front());
            //     addr_buffer.pop();
            // }
        }
        // printf("done 3\n");
        call_back_called_0 = false;
        call_back_called_1 = false;
        dramsys_0.ClockTick();
        // dramsys_1.ClockTick();
        // dramsys1.ClockTick();
        // dramsys2.ClockTick();
        // dramsys3.ClockTick();
        clk++;
        // printf("done 4\n");
        if(clk % 1000 == 0)
            printf("clk:%d rem:%d total:%d count:%d addr_0_empty:%d addr_1_empty:%d \n",clk,rem,total,_count,addr_queue_0.empty(),addr_queue_1.empty());
        if(clk > 50000) break;
    }
    // for(int i = 0 ; i<8 ; i++){
    //     for(int j = 0 ; j<200 ; j++){
    //         if(Map[i][j]){
    //             uint64_t tmp_address = ((uint64_t(i)<<11)+((uint64_t(j)&((1<<5)-1))<<6)+((uint64_t(j)>>5)<<16));
    //             printf("channel:%d , addr:%d, tmp_address:%ld\n",i,j,tmp_address);
    //         }
    //     }
    // }

}