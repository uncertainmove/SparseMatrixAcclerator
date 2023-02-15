#include "gtest/gtest.h"
#include "Accelerator.h"
#include "debug.h"

#include <iostream>

using namespace std;

extern int rst_rd;
extern int ROOT_ID;
extern debug_RD_ACTIVE_VERTEX_OFFSET debug_M2;

#if DEBUG

class Read_Active_Vertex_Offset_Test : public testing::Test {
public:
    // input signal
    int front_push_flag[CORE_NUM] = {0};
    int front_active_v_id[CORE_NUM] = {0};
    int front_active_v_value[CORE_NUM] = {0};
    int front_active_v_pull_first_flag[CORE_NUM] = {0};
    int front_active_v_dvalid[CORE_NUM] = {0};
    int front_iteration_end[CORE_NUM] = {0};
    int front_iteration_end_dvalid[CORE_NUM] = {0};
    int nextstage_full[CORE_NUM] = {0};
    // output signal
    int stage_full[CORE_NUM];
    int push_flag[CORE_NUM];
    int active_v_id[CORE_NUM];
    int active_v_value[CORE_NUM];
    int rd_active_v_offset_addr[CORE_NUM];
    int active_v_pull_first_flag[CORE_NUM];
    int active_v_dvalid[CORE_NUM];
    int iteration_end[CORE_NUM];
    int iteration_end_dvalid[CORE_NUM];

    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            front_push_flag[i] = 0;
            front_active_v_id[i] = 0;
            front_active_v_value[i] = 0;
            front_active_v_pull_first_flag[i] = 0;
            front_active_v_dvalid[i] = 0;
            front_iteration_end[i] = 0;
            front_iteration_end_dvalid[i] = 0;
            nextstage_full[i] = 0;
        }
    }
    void init_local_reg() {
        for (int i = 0; i < CORE_NUM; i++) {
            queue<int> empty1, empty2, empty3, empty4;
            swap(debug_M2._push_flag_buffer[i], empty1);
            swap(debug_M2._v_id_buffer[i], empty2);
            swap(debug_M2._v_value_buffer[i], empty3);
            swap(debug_M2._pull_first_flag_buffer[i], empty4);
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
        for (int i = 0; i < CORE_NUM; i++) {
            ASSERT_EQ(debug_M2._push_flag_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M2._v_id_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M2._v_value_buffer[i].size(), size_t(0));
            ASSERT_EQ(stage_full[i], 0);
            ASSERT_EQ(push_flag[i], -1);
            ASSERT_EQ(active_v_id[i], 0);
            ASSERT_EQ(active_v_value[i], 0);
            ASSERT_EQ(rd_active_v_offset_addr[i], 0);
            ASSERT_EQ(active_v_dvalid[i], 0);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
        }
    }
    // Warning: you must set signal at first, then call this function
    void run_module(int cycle) {
        for (int i = 0; i < cycle; i++) {
            Read_Active_Vertex_Offset (
                front_push_flag, front_active_v_id, front_active_v_value, front_active_v_pull_first_flag, front_active_v_dvalid,
                front_iteration_end, front_iteration_end_dvalid,
                nextstage_full,

                stage_full,
                push_flag, active_v_id, active_v_value, rd_active_v_offset_addr, active_v_pull_first_flag, active_v_dvalid,
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
TEST_F(Read_Active_Vertex_Offset_Test, test_PushPull_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M2._push_flag_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M2._v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M2._v_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M2._pull_first_flag_buffer[i].size(), size_t(0));
        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(push_flag[i], -1);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_value[i], 0);
        ASSERT_EQ(rd_active_v_offset_addr[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(active_v_pull_first_flag[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// given: rst=0, ROOT_ID, front active id is valid
// while: no control singal set to high
// then: normal input and output
TEST_F(Read_Active_Vertex_Offset_Test, test_Push_given_InputActiveId_then_OutputActiveId) {
    // reset
    reset_module();
    // write queue once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        front_push_flag[i] = 1;
        front_active_v_id[i] = i;
        front_active_v_value[i] = 1;
        front_active_v_dvalid[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M2._push_flag_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M2._v_id_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M2._v_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(push_flag[i], -1);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_value[i], 0);
        ASSERT_EQ(rd_active_v_offset_addr[i], 0);
        ASSERT_EQ(active_v_pull_first_flag[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    init_input_flag();
    // run once to output
    run_module(1);
    // check output
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal reg
        ASSERT_EQ(debug_M2._push_flag_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M2._v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M2._v_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(push_flag[i], 1);
        ASSERT_EQ(active_v_id[i], i);
        ASSERT_EQ(active_v_value[i], 1);
        ASSERT_EQ(rd_active_v_offset_addr[i], i / CORE_NUM);
        ASSERT_EQ(active_v_pull_first_flag[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// given: rst=0, ROOT_ID, front active id is valid
// while: no control singal set to high
// then: normal input and output
TEST_F(Read_Active_Vertex_Offset_Test, test_FirstEdgePull_given_InputActiveId_then_OutputActiveId) {
    // reset
    reset_module();
    // write queue once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        front_push_flag[i] = 0;
        front_active_v_id[i] = i;
        front_active_v_value[i] = 1;
        front_active_v_dvalid[i] = 1;
        front_active_v_pull_first_flag[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M2._push_flag_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M2._v_id_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M2._v_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M2._pull_first_flag_buffer[i].size(), size_t(1));
        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(push_flag[i], -1);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_value[i], 0);
        ASSERT_EQ(rd_active_v_offset_addr[i], 0);
        ASSERT_EQ(active_v_pull_first_flag[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    init_input_flag();
    // run once to output
    run_module(1);
    // check output
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal reg
        ASSERT_EQ(debug_M2._push_flag_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M2._v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M2._v_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(active_v_id[i], i);
        ASSERT_EQ(active_v_value[i], 1);
        ASSERT_EQ(rd_active_v_offset_addr[i], i / CORE_NUM);
        ASSERT_EQ(active_v_pull_first_flag[i], 1);
        ASSERT_EQ(active_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// given: rst=0, ROOT_ID, front active id is valid
// while: no control singal set to high
// then: normal input and output
TEST_F(Read_Active_Vertex_Offset_Test, test_NotFirstEdgePull_given_InputActiveId_then_OutputActiveId) {
    // reset
    reset_module();
    // write queue once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        front_push_flag[i] = 0;
        front_active_v_id[i] = i;
        front_active_v_value[i] = 1;
        front_active_v_dvalid[i] = 1;
        front_active_v_pull_first_flag[i] = 0;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M2._push_flag_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M2._v_id_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M2._v_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M2._pull_first_flag_buffer[i].size(), size_t(1));
        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(push_flag[i], -1);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_value[i], 0);
        ASSERT_EQ(rd_active_v_offset_addr[i], 0);
        ASSERT_EQ(active_v_pull_first_flag[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    init_input_flag();
    // run once to output
    run_module(1);
    // check output
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal reg
        ASSERT_EQ(debug_M2._push_flag_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M2._v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M2._v_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(active_v_id[i], i);
        ASSERT_EQ(active_v_value[i], 1);
        ASSERT_EQ(rd_active_v_offset_addr[i], i / CORE_NUM);
        ASSERT_EQ(active_v_pull_first_flag[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// given: rst=0, ROOT_ID, front iteration end is valid
// while: local buffer is empty
// then: output iteration end
TEST_F(Read_Active_Vertex_Offset_Test, test_Push_given_IterationEnd_while_BufferEmpty_then_OutputIterationEnd) {
    // reset
    reset_module();
    // write queue once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        front_iteration_end[i] = 1;
        front_iteration_end_dvalid[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M2._push_flag_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M2._v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M2._v_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(push_flag[i], -1);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_value[i], 0);
        ASSERT_EQ(rd_active_v_offset_addr[i], 0);
        ASSERT_EQ(active_v_pull_first_flag[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 1);
        ASSERT_EQ(iteration_end_dvalid[i], 1);
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