#include "gtest/gtest.h"
#include "Accelerator.h"
#include "debug.h"

#include <iostream>

using namespace std;

#if DEBUG

extern int rst_rd;
extern int ROOT_ID;
extern debug_Schedule debug_M7;

class Schedule_Test : public testing::Test {
public:
    // input signal
    int front_push_flag[CORE_NUM] = {0};
    int front_active_v_id[CORE_NUM] = {0};
    int front_active_v_value[CORE_NUM] = {0};
    int front_active_v_dvalid[CORE_NUM] = {0};
    int front_iteration_end[CORE_NUM] = {0};
    int front_iteration_end_dvalid[CORE_NUM] = {0};
    int hbm_interface_active_v_edge[CORE_NUM] = {0};
    int active_v_edge_valid[CORE_NUM] = {0};
    int front2_push_flag[CORE_NUM] = {0};
    int front2_active_v_id[CORE_NUM] = {0};
    int front2_active_v_value[CORE_NUM] = {0};
    int front2_active_v_edge[CORE_NUM] = {0};
    int front2_active_v_dvalid[CORE_NUM] = {0};
    int front2_iteration_end[CORE_NUM] = {0};
    int front2_iteration_end_dvalid[CORE_NUM] = {0};
    Powerlaw_Vid_Set front_global_powerlaw_v_id[CORE_NUM];
    Powerlaw_Vvisit_Set front_global_powerlaw_v_visit[CORE_NUM];
    int nextstage_full[CORE_NUM] = {0};
    // output signal
    int stage_full1[CORE_NUM];
    int stage_full2[CORE_NUM];
    int push_flag[CORE_NUM];
    int update_v_id[CORE_NUM];
    int update_v_value[CORE_NUM];
    int pull_first_flag[CORE_NUM];
    int update_v_dvalid[CORE_NUM];
    int iteration_end[CORE_NUM];
    int iteration_end_dvalid[CORE_NUM];

    static void SetUpTestCase() {
    }
    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            front_push_flag[i] = {0};
            front_active_v_id[i] = {0};
            front_active_v_value[i] = {0};
            front_active_v_dvalid[i] = {0};
            front_iteration_end[i] = {0};
            front_iteration_end_dvalid[i] = {0};
            hbm_interface_active_v_edge[i] = {0};
            active_v_edge_valid[i] = {0};
            front2_push_flag[i] = {0};
            front2_active_v_id[i] = {0};
            front2_active_v_value[i] = {0};
            front2_active_v_edge[i] = {0};
            front2_active_v_dvalid[i] = {0};
            front2_iteration_end[i] = {0};
            front2_iteration_end_dvalid[i] = {0};
            for (int j = 0; j < POWER_LAW_V_NUM; j++) {
                front_global_powerlaw_v_id[i].v[j] = {0};
                front_global_powerlaw_v_visit[i].visit[j] = {0};
            }
            nextstage_full[i] = {0};
        }
    }
    void init_local_reg() {
        for (int i = 0; i < CORE_NUM; i++) {
            queue<int> empty1, empty2, empty3, empty4, empty5, empty6, empty7, empty8;
            swap(debug_M7._push_flag_buffer1[i], empty1);
            swap(debug_M7._push_flag_buffer2[i], empty2);
            swap(debug_M7._v_id_buffer1[i], empty3);
            swap(debug_M7._v_id_buffer2[i], empty4);
            swap(debug_M7._v_value_buffer1[i], empty5);
            swap(debug_M7._v_value_buffer2[i], empty6);
            swap(debug_M7._edge_buffer1[i], empty7);
            swap(debug_M7._edge_buffer2[i], empty8);
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
            Schedule (
                front_push_flag, front_active_v_id, front_active_v_value, front_active_v_dvalid, front_iteration_end, front_iteration_end_dvalid,
                hbm_interface_active_v_edge, active_v_edge_valid,
                front2_push_flag, front2_active_v_id, front2_active_v_value, front2_active_v_edge, front2_active_v_dvalid, front2_iteration_end, front2_iteration_end_dvalid,
                front_global_powerlaw_v_id, front_global_powerlaw_v_visit,
                nextstage_full,

                stage_full1, stage_full2,
                push_flag, update_v_id, update_v_value, pull_first_flag, update_v_dvalid,
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
TEST_F(Schedule_Test, test_PushPull_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._push_flag_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._push_flag_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_id_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_id_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer2[i].size(), size_t(0));

        ASSERT_EQ(stage_full1[i], 0);
        ASSERT_EQ(stage_full2[i], 0);
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(update_v_id[i], 0);
        ASSERT_EQ(update_v_value[i], 0);
        ASSERT_EQ(pull_first_flag[i], 0);
        ASSERT_EQ(update_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// test: normal mode
// given: rst=0, ROOT_ID, front addr and data valid
// while: no control singal set to high
// then: store and output
TEST_F(Schedule_Test, test_Pull_given_Front2InValid_then_StoreAndOutput) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        front2_push_flag[i] = 0;
        front2_active_v_id[i] = i;
        front2_active_v_value[i] = 1;
        front2_active_v_edge[i] = i + 2;
        front2_active_v_dvalid[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._push_flag_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._push_flag_buffer2[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_id_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_id_buffer2[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_value_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer2[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._edge_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer2[i].size(), size_t(1));

        ASSERT_EQ(stage_full1[i], 0);
        ASSERT_EQ(stage_full2[i], 0);
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(update_v_id[i], 0);
        ASSERT_EQ(update_v_value[i], 0);
        ASSERT_EQ(pull_first_flag[i], 0);
        ASSERT_EQ(update_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // run to output
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._push_flag_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._push_flag_buffer2[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_id_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_id_buffer2[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_value_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer2[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._edge_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer2[i].size(), size_t(1));

        ASSERT_EQ(stage_full1[i], 0);
        ASSERT_EQ(stage_full2[i], 0);
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(update_v_id[i], i);
        ASSERT_EQ(update_v_value[i], i + 2);
        ASSERT_EQ(pull_first_flag[i], 1);
        ASSERT_EQ(update_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// test: normal mode
// given: rst=0, ROOT_ID, front addr and data valid
// while: no control singal set to high
// then: store and output
TEST_F(Schedule_Test, test_Push_given_FrontInValid_then_StoreAndOutput) {
    // reset
    reset_module();
    // run once
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
        ASSERT_EQ(debug_M7._push_flag_buffer1[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._push_flag_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_id_buffer1[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_id_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer1[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_value_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer2[i].size(), size_t(0));

        ASSERT_EQ(stage_full1[i], 0);
        ASSERT_EQ(stage_full2[i], 0);
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(update_v_id[i], 0);
        ASSERT_EQ(update_v_value[i], 0);
        ASSERT_EQ(pull_first_flag[i], 0);
        ASSERT_EQ(update_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        hbm_interface_active_v_edge[i] = i + 1;
        active_v_edge_valid[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._push_flag_buffer1[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._push_flag_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_id_buffer1[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_id_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer1[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_value_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer1[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._edge_buffer2[i].size(), size_t(0));

        ASSERT_EQ(stage_full1[i], 0);
        ASSERT_EQ(stage_full2[i], 0);
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(update_v_id[i], 0);
        ASSERT_EQ(update_v_value[i], 0);
        ASSERT_EQ(pull_first_flag[i], 0);
        ASSERT_EQ(update_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // run to output
    init_input_flag();
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._push_flag_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._push_flag_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_id_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_id_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer2[i].size(), size_t(0));

        ASSERT_EQ(stage_full1[i], 0);
        ASSERT_EQ(stage_full2[i], 0);
        ASSERT_EQ(push_flag[i], 1);
        ASSERT_EQ(update_v_id[i], i + 1);
        ASSERT_EQ(update_v_value[i], 1);
        ASSERT_EQ(pull_first_flag[i], 0);
        ASSERT_EQ(update_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// test: normal mode
// given: rst=0, ROOT_ID, front addr and data valid
// while: no control singal set to high
// then: store and output
TEST_F(Schedule_Test, test_InOut_given_DataValid_then_OutputValid) {
    // reset
    reset_module();
    // run once to store vertex
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        front_push_flag[i] = 1;
        front_active_v_id[i] = (i % 2 ? -1 : 0);
        front_active_v_value[i] = 1;
        front_active_v_dvalid[i] = 1;
        front_iteration_end[i] = 0;
        front_iteration_end_dvalid[i] = 0;
        hbm_interface_active_v_edge[i] = 0;
        active_v_edge_valid[i] = 0;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._push_flag_buffer1[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._push_flag_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_id_buffer1[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_id_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer1[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_value_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer2[i].size(), size_t(0));

        ASSERT_EQ(stage_full1[i], 0);
        ASSERT_EQ(stage_full2[i], 0);
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(update_v_id[i], 0);
        ASSERT_EQ(update_v_value[i], 0);
        ASSERT_EQ(pull_first_flag[i], 0);
        ASSERT_EQ(update_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // run twice to store edge
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        hbm_interface_active_v_edge[i] = i + 1;
        active_v_edge_valid[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._push_flag_buffer1[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._push_flag_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_id_buffer1[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_id_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer1[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._v_value_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer1[i].size(), size_t(1));
        ASSERT_EQ(debug_M7._edge_buffer2[i].size(), size_t(0));

        ASSERT_EQ(stage_full1[i], 0);
        ASSERT_EQ(stage_full2[i], 0);
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(update_v_id[i], 0);
        ASSERT_EQ(update_v_value[i], 0);
        ASSERT_EQ(pull_first_flag[i], 0);
        ASSERT_EQ(update_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // run to output
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M7._push_flag_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._push_flag_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_id_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_id_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._v_value_buffer2[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer1[i].size(), size_t(0));
        ASSERT_EQ(debug_M7._edge_buffer2[i].size(), size_t(0));

        if (i % 2) {
            ASSERT_EQ(stage_full1[i], 0);
            ASSERT_EQ(stage_full2[i], 0);
            ASSERT_EQ(push_flag[i], 0);
            ASSERT_EQ(update_v_id[i], 0);
            ASSERT_EQ(update_v_value[i], 0);
            ASSERT_EQ(pull_first_flag[i], 0);
            ASSERT_EQ(update_v_dvalid[i], 0);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
        } else {
            ASSERT_EQ(stage_full1[i], 0);
            ASSERT_EQ(stage_full2[i], 0);
            ASSERT_EQ(push_flag[i], 1);
            ASSERT_EQ(update_v_id[i], i + 1);
            ASSERT_EQ(update_v_value[i], 1);
            ASSERT_EQ(pull_first_flag[i], 0);
            ASSERT_EQ(update_v_dvalid[i], 1);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
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