#include "gtest/gtest.h"
#include "debug.h"
#include "core.h"
#include "parameter.h"
#include "structure.h"
#include "util.h"

#include <iostream>

using namespace std;

#if DEBUG

extern int clk;
extern int rst_rd;
extern debug_RD_ACTIVE_VERTEX_OFFSET debug_M2;

class Read_Active_Vertex_Offset_Test : public testing::Test {
public:
    // input signal
    int front_active_v_id[CORE_NUM] = {0};
    int front_active_v_dvalid[CORE_NUM] = {0};
    int front_iteration_end[CORE_NUM] = {0};
    int front_iteration_end_dvalid[CORE_NUM] = {0};
    int front_iteration_id[CORE_NUM] = {0};
    int nextstage_full[CORE_NUM] = {0};
    // output signal
    int stage_full[CORE_NUM];
    int active_v_id[CORE_NUM];
    int rd_active_v_offset_addr[CORE_NUM];
    int rd_active_v_value_addr[CORE_NUM];
    int active_v_dvalid[CORE_NUM];
    int iteration_end[CORE_NUM];
    int iteration_end_dvalid[CORE_NUM];
    int iteration_id[CORE_NUM];

    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            front_active_v_id[i] = 0;
            front_active_v_dvalid[i] = 0;
            front_iteration_end[i] = 0;
            front_iteration_end_dvalid[i] = 0;
            front_iteration_id[i] = 0;
            nextstage_full[i] = 0;
        }
    }
    void init_local_reg() {
        for (int i = 0; i < CORE_NUM; i++) {
            clear_buffer(debug_M2._v_id_buffer[i]);
        }
    }
    void reset_module() {
        // reset
        rst_rd = 1;
        init_input_flag();
        run_module(1); // run module once to bind debug port
        init_local_reg();
        run_module(10);
        rst_rd = 0;
    }
    // Warning: you must set signal at first, then call this function
    void run_module(int cycle) {
        for (int i = 0; i < cycle; i++) {
            Read_Active_Vertex_Offset (
                front_active_v_id, front_active_v_dvalid,
                front_iteration_end, front_iteration_end_dvalid, front_iteration_id,
                nextstage_full,

                stage_full,
                active_v_id, rd_active_v_offset_addr, rd_active_v_value_addr, active_v_dvalid,
                iteration_end, iteration_end_dvalid, iteration_id
            );
        }
    }
    virtual void TearDown() {
        reset_module();
    }
};

// given: rst=1
// while: no control singal set to high
// then: local buffer empty
TEST_F(Read_Active_Vertex_Offset_Test, test_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M2._v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(rd_active_v_offset_addr[i], 0);
        ASSERT_EQ(rd_active_v_value_addr[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
}

// given: rst=0, front active id is valid
// while: no control singal set to high
// then: normal input and output
TEST_F(Read_Active_Vertex_Offset_Test, test_given_InputActiveId_then_OutputActiveId) {
    // reset
    reset_module();
    // write queue once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        front_active_v_id[i] = i;
        front_active_v_dvalid[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M2._v_id_buffer[i].size(), size_t(1));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(rd_active_v_offset_addr[i], 0);
        ASSERT_EQ(rd_active_v_value_addr[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
    init_input_flag();
    // run once to output
    run_module(1);
    // check output
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal reg
        ASSERT_EQ(debug_M2._v_id_buffer[i].size(), size_t(0));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(active_v_id[i], i);
        ASSERT_EQ(rd_active_v_offset_addr[i], i / CORE_NUM);
        ASSERT_EQ(rd_active_v_value_addr[i], i / CORE_NUM);
        ASSERT_EQ(active_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
}

// given: rst=0, front iteration end is valid
// while: local buffer is empty
// then: output iteration end
TEST_F(Read_Active_Vertex_Offset_Test, test_given_IterationEnd_while_BufferEmpty_then_OutputIterationEnd) {
    // reset
    reset_module();
    // write queue once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        front_iteration_end[i] = 1;
        front_iteration_end_dvalid[i] = 1;
        front_iteration_id[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M2._v_id_buffer[i].size(), size_t(0));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(rd_active_v_offset_addr[i], 0);
        ASSERT_EQ(rd_active_v_value_addr[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 1);
        ASSERT_EQ(iteration_end_dvalid[i], 1);
        ASSERT_EQ(iteration_id[i], 1);
    }
}

// given: rst=0, front iteration end is valid
// while: local buffer is not empty
// then: do not output iteration end
TEST_F(Read_Active_Vertex_Offset_Test, test_given_IterationEnd_while_BufferNotEmpty_then_OutputZero) {
    // reset
    reset_module();
    // write queue once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        front_active_v_id[i] = i;
        front_active_v_dvalid[i] = 1;
        front_iteration_end[i] = 1;
        front_iteration_end_dvalid[i] = 1;
        front_iteration_id[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M2._v_id_buffer[i].size(), size_t(1));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(rd_active_v_offset_addr[i], 0);
        ASSERT_EQ(rd_active_v_value_addr[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 1);
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