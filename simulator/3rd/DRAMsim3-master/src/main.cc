#include <iostream>
#include <queue>
#include "./../ext/headers/args.hxx"
#include "cpu.h"

using namespace dramsim3;
using namespace std;
#define cn_size 16
#define task_num 2000

bool call_back_called_0 = false;
bool call_back_called_1 = false;
std::queue<uint64_t> addr_queue_0;
std::queue<uint64_t> addr_queue_1;
std::queue<uint64_t> addr_buffer;

//HBM理论带宽：16*64B*1GHz = 953.67GB/s

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


// int Map[32][200] = {0}; // 95,96
bool dram_test(uint64_t channel,uint64_t addr){
    // printf("begin add\n");
    uint64_t tmp_channel = channel >> 1;
    uint64_t tmp_address = ((uint64_t(tmp_channel)<<11)+((uint64_t(addr)&((1<<5)-1))<<6)+((uint64_t(addr)>>5)<<16));//21bit
    bool ok;
    // Map[channel][addr] += 1;
    // printf("done 0 channel:%ld address:%ld\n",tmp_channel,tmp_address);
    if(channel % 2){
        ok = dramsys_1.AddTransaction(tmp_address,false);
    }else{
        ok = dramsys_0.AddTransaction(tmp_address,false);
    }
    // printf("done 1 channel:%ld address:%ld\n",tmp_channel,tmp_address);
    return ok;
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
    }
    // printf("begin main,clk:%d\n",clk);
    int HBM_Edge_Valid[cn_size];
    uint64_t Rd_HBM_Edge_Addr[cn_size];
    int rd_hbm_channel_0[cn_size];
    int rd_hbm_channel_1[cn_size];
    uint64_t rd_hbm_edge_addr[cn_size];
    int rd_hbm_edge_valid[cn_size];
    uint64_t addr[cn_size] = {0};
    int _count = 0;
    int rem = 0, total = 0;
    int max = (1<<21) - 1, min = 0;
    srand(time(nullptr));
    int finish_fg[cn_size] = {0};
    int finish_channel[cn_size] = {0};
    // printf("begin main done 0\n");
    while(true){
        for(int i = 0 ; i<cn_size; i++){
            if(clk == 0){
                HBM_Edge_Valid[i] = 1;
            }
            if(clk < task_num || addr[i] < task_num ){//95,6
                // printf("begin main done 0.5\n");
                if(HBM_Edge_Valid[i] == 1){
                    Rd_HBM_Edge_Addr[i] = addr[i];
                    // int random_value = (rand()%max);
                    // Rd_HBM_Edge_Addr[i] = random_value;
                    
                    // addr[i]++;//sequential access:7.38  // out-of-order access:1.
                }else{
                    HBM_Edge_Valid[i] = 1;
                }
                /*addr generate*/
                // addr++;
            }else{
                HBM_Edge_Valid[i] = 0;
            }
        }
        // printf("begin main done 1\n");

        for(int i = 0 ; i<cn_size ; i++){
            if(HBM_Edge_Valid[i]){
                if(dram_test(i,Rd_HBM_Edge_Addr[i])){
                    HBM_Edge_Valid[i] = 1;
                    _count ++;
                    total ++;
                    addr[i] ++;
                }else{
                    HBM_Edge_Valid[i] = 0;
                }

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
                    finish_channel[tmp_channel]++;
                    rd_hbm_edge_addr[tmp_channel] = tmp_addr;
                    rd_hbm_edge_valid[tmp_channel] = 1;
                    // Map[tmp_channel][tmp_addr] -= 1;
                    // printf("output channel:%ld addr:%ld tmp_address:%ld\n",tmp_channel,tmp_addr,addr_queue.front());
                    addr_queue_0.pop();
                    _count --;
                    rem ++;
                }else{
                    addr_buffer.push(addr_queue_0.front());
                    addr_queue_0.pop();
                }
            }
            while(!addr_buffer.empty()){
                addr_queue_0.push(addr_buffer.front());
                addr_buffer.pop();
            }
        }

        if( call_back_called_1 || !addr_queue_1.empty() ){
            while(!addr_queue_1.empty()){
                // assert(addr_queue.front() != 786432);
                // uint64_t tmp_address = ((uint64_t(channel)<<11)+((uint64_t(addr)&((1<<5)-1))<<6)+((uint64_t(addr)>>5)<<16));

                uint64_t tmp_channel = (((addr_queue_1.front() >> 11) & ((1<<3)-1))<<1) + 1;
                uint64_t tmp_addr = ((addr_queue_1.front() >> 6) & ((1 << 5) - 1))+(((addr_queue_1.front() >> 16) & ((1 << 16) - 1))<<5);
                if(!rd_hbm_channel_1[tmp_channel]){
                    rd_hbm_channel_1[tmp_channel] = 1;
                    finish_channel[tmp_channel]++;
                    rd_hbm_edge_addr[tmp_channel] = tmp_addr;
                    rd_hbm_edge_valid[tmp_channel] = 1;
                    // Map[tmp_channel][tmp_addr] -= 1;
                    // printf("output channel:%ld addr:%ld tmp_address:%ld\n",tmp_channel,tmp_addr,addr_queue.front());
                    addr_queue_1.pop();
                    _count --;
                    rem ++;
                }else{
                    addr_buffer.push(addr_queue_1.front());
                    addr_queue_1.pop();
                }
            }
            while(!addr_buffer.empty()){
                addr_queue_1.push(addr_buffer.front());
                addr_buffer.pop();
            }
        }
        // printf("done 3\n");
        call_back_called_0 = false;
        call_back_called_1 = false;
        dramsys_0.ClockTick();
        dramsys_1.ClockTick();
        clk++;
        // printf("done 4\n");
        if(clk % 1000 == 0){
            printf("clk:%d rem:%d total:%d count:%d addr_0_empty:%d addr_1_empty:%d \n",clk,rem,total,_count,addr_queue_0.empty(),addr_queue_1.empty());
        }
            
        int finish_flag = 1;
        for(int i = 0  ; i<cn_size ; i++){
            finish_flag &= (finish_channel[i] == task_num);
            if(finish_channel[i] == task_num && !finish_fg[i]){
                finish_fg[i] = true;
                printf("channel %d finish in clk:%d\n",i,clk);
            }
        }

        if(clk > 50000 || (finish_flag && _count == 0)){
            printf("end at clk:%d\n",clk);
            break;
        }

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