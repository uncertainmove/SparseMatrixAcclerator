#include "gtest/gtest.h"
#include "debug.h"
#include "core.h"
#include "parameter.h"
#include "structure.h"
#include "util.h"

#include <iostream>

using namespace std;

#if DEBUG

extern int rst_rd;
extern debug_Schedule debug_M7;

class Schedule_Test : public testing::Test {
public:
    // input signal
    int front_active_v_id[CORE_NUM] = {0};
    V_VALUE_TP front_active_v_value[CORE_NUM] = {0};
    int front_active_v_dvalid[CORE_NUM] = {0};
    int front_iteration_end[CORE_NUM] = {0};
    int front_iteration_end_dvalid[CORE_NUM] = {0};
    int front_iteration_id[CORE_NUM] = {0};
    int hbm_interface_active_v_edge[CORE_NUM] = {0};
    int hbm_interface_active_v_edge_valid[CORE_NUM] = {0};
    int nextstage_full[CORE_NUM] = {0};
    // output signal
    int stage_full[CORE_NUM];
    int update_v_id[CORE_NUM];
    V_VALUE_TP update_v_value[CORE_NUM];
    int update_v_dvalid[CORE_NUM];
    int iteration_end[CORE_NUM];
    int iteration_end_dvalid[CORE_NUM];
    int iteration_id[CORE_NUM];

    static void SetUpTestCase() {
    }
    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            front_active_v_id[i] = {0};
            front_active_v_value[i] = {0};
            front_active_v_dvalid[i] = {0};
            front_iteration_end[i] = {0};
            front_iteration_end_dvalid[i] = {0};
            front_iteration_id[i] = {0};
            hbm_interface_active_v_edge[i] = {0};
            hbm_interface_active_v_edge_valid[i] = {0};
            nextstage_full[i] = {0};
        }
    }
    void init_local_reg() {
        for (int i = 0; i < CORE_NUM; i++) {
            clear_buffer(debug_M7._v_id_buffer[i]);
            clear_buffer(debug_M7._edge_buffer[i]);
            clear_buffer(debug_M7._v_value_buffer[i]);
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
            Schedule (
                front_active_v_id, front_active_v_value, front_active_v_dvalid,
                front_iteration_end, front_iteration_end_dvalid, front_iteration_id,
                hbm_interface_active_v_edge, hbm_interface_active_v_edge_valid,
                nextstage_full,

                stage_full,
                update_v_id, update_v_value, update_v_dvalid,
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
TEST_F(Schedule_Test, test_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer[i].size(), size_t(0));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(update_v_id[i], 0);
        ASSERT_FLOAT_EQ(update_v_value[i], 0.0);
        ASSERT_EQ(update_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
}

// test: normal mode
// given: rst=0, front addr and data valid
// while: no control singal set to high
// then: store and output
TEST_F(Schedule_Test, test_given_FrontInValid_then_StoreAndOutput) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        front_active_v_id[i] = i;
        front_active_v_value[i] = 1.0;
        front_active_v_dvalid[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._v_id_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._edge_buffer[i].size(), size_t(0));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(update_v_id[i], 0);
        ASSERT_FLOAT_EQ(update_v_value[i], 0.0);
        ASSERT_EQ(update_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        hbm_interface_active_v_edge[i] = i + 1;
        hbm_interface_active_v_edge_valid[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._v_id_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._edge_buffer[i].size(), size_t(1));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(update_v_id[i], 0);
        ASSERT_FLOAT_EQ(update_v_value[i], 0.0);
        ASSERT_EQ(update_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
    // run to output
    init_input_flag();
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer[i].size(), size_t(0));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(update_v_id[i], i + 1);
        ASSERT_FLOAT_EQ(update_v_value[i], 1.0);
        ASSERT_EQ(update_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
}

// test: normal mode
// given: rst=0, front addr and data valid
// while: no control singal set to high
// then: store and output
TEST_F(Schedule_Test, test_InOut_given_DataValid_then_OutputValid) {
    // reset
    reset_module();
    // run once to store vertex
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        front_active_v_id[i] = (i % 2 ? -1 : 0);
        front_active_v_value[i] = 1.0;
        front_active_v_dvalid[i] = 1;
        front_iteration_end[i] = 0;
        front_iteration_end_dvalid[i] = 0;
        front_iteration_id[i] = 0;
        hbm_interface_active_v_edge[i] = 0;
        hbm_interface_active_v_edge_valid[i] = 0;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._v_id_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._edge_buffer[i].size(), size_t(0));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(update_v_id[i], 0);
        ASSERT_FLOAT_EQ(update_v_value[i], 0.0);
        ASSERT_EQ(update_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
    // run twice to store edge
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        hbm_interface_active_v_edge[i] = i + 1;
        hbm_interface_active_v_edge_valid[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._v_id_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._edge_buffer[i].size(), size_t(1));

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(update_v_id[i], 0);
        ASSERT_FLOAT_EQ(update_v_value[i], 0.0);
        ASSERT_EQ(update_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
    // run to output
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer[i].size(), size_t(0));

        if (i % 2) {
            ASSERT_EQ(stage_full[i], 0);
            ASSERT_EQ(update_v_id[i], 0);
            ASSERT_FLOAT_EQ(update_v_value[i], 0.0);
            ASSERT_EQ(update_v_dvalid[i], 0);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
            ASSERT_EQ(iteration_id[i], 0);
        } else {
            ASSERT_EQ(stage_full[i], 0);
            ASSERT_EQ(update_v_id[i], i + 1);
            ASSERT_FLOAT_EQ(update_v_value[i], 1.0);
            ASSERT_EQ(update_v_dvalid[i], 1);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
            ASSERT_EQ(iteration_id[i], 0);
        }
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