#include "gtest/gtest.h"
#include "Accelerator.h"
#include "debug.h"

#include <iostream>

using namespace std;

#if DEBUG

extern int rst_rd;
extern int ROOT_ID;
extern debug_HBM_Interface debug_M6;
extern int Edge_MEM[PSEUDO_CHANNEL_NUM][MAX_EDGE_ADDR];

class HBM_Interface_Test : public testing::Test {
public:
    // input signal
    int front_rd_hbm_edge_addr[PSEUDO_CHANNEL_NUM] = {0};
    int front_rd_hbm_edge_valid[PSEUDO_CHANNEL_NUM] = {0};
    Cacheline_16 hbm_controller_edge[PSEUDO_CHANNEL_NUM] = {0};
    int hbm_controller_dvalid[PSEUDO_CHANNEL_NUM] = {0};
    int hbm_controller_full[PSEUDO_CHANNEL_NUM] = {0};
    // output signal
    int stage_full[PSEUDO_CHANNEL_NUM];
    int rd_hbm_edge_addr[PSEUDO_CHANNEL_NUM];
    int rd_hbm_edge_valid[PSEUDO_CHANNEL_NUM];
    int active_v_edge[CORE_NUM];
    int active_v_edge_valid[CORE_NUM];

    static void SetUpTestCase() {
    }
    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
            front_rd_hbm_edge_addr[i] = {0};
            front_rd_hbm_edge_valid[i] = {0};
            hbm_controller_edge[i] = {0};
            hbm_controller_dvalid[i] = {0};
            hbm_controller_full[i] = {0};
        }
    }
    /*
    void clear_fifo(queue<int> &q) {
        while (!q.empty()) {
            q.pop();
        }
    }
    void clear_fifo(queue<Cacheline_16> &q) {
        while (!q.empty()) {
            q.pop();
        }
    }
    */
    void init_local_reg() {
        for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
            queue<int> empty1;
            queue<Cacheline_16> empty2;
            swap(debug_M6._edge_addr_buffer[i], empty1);
            swap(debug_M6._edge_buffer[i], empty2);
            // clear_fifo(debug_M6._edge_addr_buffer[i]);
            // clear_fifo(debug_M6._edge_buffer[i]);
        }
    }
    void reset_module() {
        // reset
        rst_rd = 1;
        int cycle = 10;
        init_input_flag();
        run_module(1); // run module once to bind debug port
        init_local_reg();
        run_module(cycle);
        rst_rd = 0;
    }
    // Warning: you must set signal at first, then call this function
    void run_module(int cycle) {
        for (int i = 0; i < cycle; i++) {
            HBM_Interface (
                front_rd_hbm_edge_addr, front_rd_hbm_edge_valid,
                hbm_controller_edge, hbm_controller_dvalid,
                hbm_controller_full,

                stage_full,
                rd_hbm_edge_addr, rd_hbm_edge_valid,
                active_v_edge, active_v_edge_valid
            );
        }
    }
    virtual void TearDown() {
        reset_module();
    }
};

// given: rst=1, ROOT_ID
// while: no control singal set to high
// then: local buffer empty
TEST_F(HBM_Interface_Test, test_Push_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        ASSERT_EQ(debug_M6._edge_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M6._edge_buffer[i].size(), size_t(0));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(rd_hbm_edge_addr[i], 0);
        ASSERT_EQ(rd_hbm_edge_valid[i], 0);
    }
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(active_v_edge[i], 0);
        ASSERT_EQ(active_v_edge_valid[i], 0);
    }
}

// test: normal mode
// given: rst=0, ROOT_ID, front addr valid
// while: no control singal set to high
// then: store and output
TEST_F(HBM_Interface_Test, test_AddrInOut_given_AddrValid_then_OutputAddr) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        front_rd_hbm_edge_addr[i] = i;
        front_rd_hbm_edge_valid[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        ASSERT_EQ(debug_M6._edge_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M6._edge_buffer[i].size(), size_t(0));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(rd_hbm_edge_addr[i], 0);
        ASSERT_EQ(rd_hbm_edge_valid[i], 0);
    }
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(active_v_edge[i], 0);
        ASSERT_EQ(active_v_edge_valid[i], 0);
    }
    // run twice
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        ASSERT_EQ(debug_M6._edge_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M6._edge_buffer[i].size(), size_t(0));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(rd_hbm_edge_addr[i], i);
        ASSERT_EQ(rd_hbm_edge_valid[i], 1);
    }
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(active_v_edge[i], 0);
        ASSERT_EQ(active_v_edge_valid[i], 0);
    }
}

// test: normal mode
// given: rst=0, ROOT_ID, front addr valid
// while: no control singal set to high
// then: store and output
TEST_F(HBM_Interface_Test, test_DataInOut_given_DataValid_then_OutputData) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        hbm_controller_edge[i] = Cacheline_16(7);
        hbm_controller_dvalid[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        ASSERT_EQ(debug_M6._edge_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M6._edge_buffer[i].size(), size_t(1));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(rd_hbm_edge_addr[i], 0);
        ASSERT_EQ(rd_hbm_edge_valid[i], 0);
    }
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(active_v_edge[i], 0);
        ASSERT_EQ(active_v_edge_valid[i], 0);
    }
    // run twice
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        ASSERT_EQ(debug_M6._edge_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M6._edge_buffer[i].size(), size_t(0));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(rd_hbm_edge_addr[i], 0);
        ASSERT_EQ(rd_hbm_edge_valid[i], 0);
    }
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(active_v_edge[i], 7);
        ASSERT_EQ(active_v_edge_valid[i], 1);
    }
}

int main(int argc,char* argv[])
{ 
    //testing::GTEST_FLAG(output) = "xml:"; //若要生成xml结果文件
    testing::InitGoogleTest(&argc,argv);
    RUN_ALL_TESTS();
    return 0;
}

#endif