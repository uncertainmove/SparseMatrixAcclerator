#include "gtest/gtest.h"
#include "Accelerator.h"
#include "debug.h"

#include <iostream>

using namespace std;

#if DEBUG

extern int rst_rd;
extern int ROOT_ID;
extern debug_Rd_First_Edge_Bram debug_M5_2;
extern int First_Edge_BRAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1];

class Rd_First_Edge_Bram_Test : public testing::Test {
public:
    // input signal
    int front_rd_edge_addr[CORE_NUM] = {0};
    int front_push_flag[CORE_NUM] = {0};
    int front_active_v_id[CORE_NUM] = {0};
    int front_active_v_value[CORE_NUM] = {0};
    int front_rd_edge_valid[CORE_NUM] = {0};
    int front_iteration_end[CORE_NUM] = {0};
    int front_iteration_end_dvalid[CORE_NUM] = {0};
    int nextstage_full[CORE_NUM] = {0};
    // output signal
    int stage_full[CORE_NUM];
    int push_flag[CORE_NUM];
    int active_v_id[CORE_NUM];
    int active_v_value[CORE_NUM];
    int active_v_edge[CORE_NUM];
    int active_v_dvalid[CORE_NUM];
    int iteration_end[CORE_NUM];
    int iteration_end_dvalid[CORE_NUM];

    static void SetUpTestCase() {
        for (int i = 0; i < CORE_NUM; i++) {
            for (int j = 0; j < MAX_VERTEX_NUM / CORE_NUM + 1; j++) {
                First_Edge_BRAM[i][j] = i + j;
            }
        }
    }
    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            front_rd_edge_addr[i] = {0};
            front_push_flag[i] = {0};
            front_active_v_id[i] = {0};
            front_active_v_value[i] = {0};
            front_rd_edge_valid[i] = {0};
            front_iteration_end[i] = {0};
            front_iteration_end_dvalid[i] = {0};
            nextstage_full[i] = {0};
        }
    }
    void init_local_reg() {
        for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
            queue<int> empty1, empty2, empty3;
            swap(debug_M5_2._push_flag_buffer[i], empty1);
            swap(debug_M5_2._v_id_buffer[i], empty2);
            swap(debug_M5_2._v_value_buffer[i], empty3);
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
            Rd_First_Edge_Bram (
                front_rd_edge_addr, front_push_flag, front_active_v_id, front_active_v_value, front_rd_edge_valid,
                front_iteration_end, front_iteration_end_dvalid,
                nextstage_full,

                stage_full,
                push_flag, active_v_id, active_v_value, active_v_edge, active_v_dvalid,
                iteration_end, iteration_end_dvalid
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
TEST_F(Rd_First_Edge_Bram_Test, test_Pull_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M5_2._push_flag_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M5_2._v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M5_2._v_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M5_2._edge_buffer[i].size(), size_t(0));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_value[i], 0);
        ASSERT_EQ(active_v_edge[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// test: normal mode
// given: rst=0, ROOT_ID, front addr valid
// while: no control singal set to high
// then: store and output
TEST_F(Rd_First_Edge_Bram_Test, test_AddrInOut_given_AddrValid_then_OutputAddr) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        front_rd_edge_addr[i] = i;
        front_active_v_id[i] = i;
        front_active_v_value[i] = 1;
        front_rd_edge_valid[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M5_2._push_flag_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M5_2._v_id_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M5_2._v_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M5_2._edge_buffer[i].size(), size_t(0));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_value[i], 0);
        ASSERT_EQ(active_v_edge[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // run twice
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M5_2._push_flag_buffer[i].size(), size_t(2));
        ASSERT_EQ(debug_M5_2._v_id_buffer[i].size(), size_t(2));
        ASSERT_EQ(debug_M5_2._v_value_buffer[i].size(), size_t(2));
        ASSERT_EQ(debug_M5_2._edge_buffer[i].size(), size_t(1));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_value[i], 0);
        ASSERT_EQ(active_v_edge[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // run three times
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M5_2._push_flag_buffer[i].size(), size_t(2));
        ASSERT_EQ(debug_M5_2._v_id_buffer[i].size(), size_t(2));
        ASSERT_EQ(debug_M5_2._v_value_buffer[i].size(), size_t(2));
        ASSERT_EQ(debug_M5_2._edge_buffer[i].size(), size_t(1));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(active_v_id[i], i);
        ASSERT_EQ(active_v_value[i], 1);
        ASSERT_EQ(active_v_edge[i], First_Edge_BRAM[i][i]);
        ASSERT_EQ(active_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
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