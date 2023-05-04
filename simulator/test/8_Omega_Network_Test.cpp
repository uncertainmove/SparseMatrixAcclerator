#include "gtest/gtest.h"
#include "Accelerator.h"
#include "debug.h"

#include <iostream>

using namespace std;

#if DEBUG

extern int rst_rd;
extern int ROOT_ID;
extern debug_Omega_Network debug_M8;

class Omega_Network_Test : public testing::Test {
public:
    // input signal
    int om1_front_push_flag[CORE_NUM] = {0};
    int om1_front_update_v_id[CORE_NUM] = {0};
    int om1_front_update_v_value[CORE_NUM] = {0};
    int om1_front_pull_first_flag[CORE_NUM] = {0};
    int om1_front_update_v_dvalid[CORE_NUM] = {0};
    int om1_front_iteration_end[CORE_NUM] = {0};
    int om1_front_iteration_end_dvalid[CORE_NUM] = {0};
    int om2_front_push_flag[CORE_NUM] = {0};
    int om2_front_update_v_id[CORE_NUM] = {0};
    int om2_front_update_v_value[CORE_NUM] = {0};
    int om2_front_pull_first_flag[CORE_NUM] = {0};
    int om2_front_update_v_dvalid[CORE_NUM] = {0};
    int om2_front_iteration_end[CORE_NUM] = {0};
    int om2_front_iteration_end_dvalid[CORE_NUM] = {0};
    int source_core_full[CORE_NUM] = {0};
    // output signal
    int stage_full_om1[CORE_NUM] = {0};
    int stage_full_om2[CORE_NUM] = {0};
    int om1_push_flag[CORE_NUM] = {0};
    int om1_update_v_id[CORE_NUM] = {0};
    int om1_update_v_value[CORE_NUM] = {0};
    int om1_pull_first_flag[CORE_NUM] = {0};
    int om1_update_v_dvalid[CORE_NUM] = {0};
    int om1_iteration_end[CORE_NUM] = {0};
    int om1_iteration_end_dvalid[CORE_NUM] = {0};
    int om2_push_flag[CORE_NUM] = {0};
    int om2_update_v_id[CORE_NUM] = {0};
    int om2_update_v_value[CORE_NUM] = {0};
    int om2_pull_first_flag[CORE_NUM] = {0};
    int om2_update_v_dvalid[CORE_NUM] = {0};
    int om2_iteration_end[CORE_NUM] = {0};
    int om2_iteration_end_dvalid[CORE_NUM] = {0};

    static void SetUpTestCase() {
    }
    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            om1_front_push_flag[i] = {0};
            om1_front_update_v_id[i] = {0};
            om1_front_update_v_value[i] = {0};
            om1_front_pull_first_flag[i] = {0};
            om1_front_update_v_dvalid[i] = {0};
            om1_front_iteration_end[i] = {0};
            om1_front_iteration_end_dvalid[i] = {0};
            om2_front_push_flag[i] = {0};
            om2_front_update_v_id[i] = {0};
            om2_front_update_v_value[i] = {0};
            om2_front_pull_first_flag[i] = {0};
            om2_front_update_v_dvalid[i] = {0};
            om2_front_iteration_end[i] = {0};
            om2_front_iteration_end_dvalid[i] = {0};
            source_core_full[i] = {0};
        }
    }
    void clear_fifo(queue<int>& fifo) {
        while (!fifo.empty()) {
            fifo.pop();
        }
    }
    void init_local_reg() {
        for (int i = 0; i < OMEGA_DEPTH; i++) {
            for (int j = 0; j < OMEGA_SWITCH_NUM; j++) {
                clear_fifo(debug_M8._om1_push_flag_buffer_in1_out1[i][j]);
                clear_fifo(debug_M8._om1_push_flag_buffer_in1_out2[i][j]);
                clear_fifo(debug_M8._om1_push_flag_buffer_in2_out1[i][j]);
                clear_fifo(debug_M8._om1_push_flag_buffer_in2_out2[i][j]);
                clear_fifo(debug_M8._om1_id_buffer_in1_out1[i][j]);
                clear_fifo(debug_M8._om1_id_buffer_in1_out2[i][j]);
                clear_fifo(debug_M8._om1_id_buffer_in2_out1[i][j]);
                clear_fifo(debug_M8._om1_id_buffer_in2_out2[i][j]);
                clear_fifo(debug_M8._om1_value_buffer_in1_out1[i][j]);
                clear_fifo(debug_M8._om1_value_buffer_in1_out2[i][j]);
                clear_fifo(debug_M8._om1_value_buffer_in2_out1[i][j]);
                clear_fifo(debug_M8._om1_value_buffer_in2_out2[i][j]);

                clear_fifo(debug_M8._om2_push_flag_buffer_in1_out1[i][j]);
                clear_fifo(debug_M8._om2_push_flag_buffer_in1_out2[i][j]);
                clear_fifo(debug_M8._om2_push_flag_buffer_in2_out1[i][j]);
                clear_fifo(debug_M8._om2_push_flag_buffer_in2_out2[i][j]);
                clear_fifo(debug_M8._om2_id_buffer_in1_out1[i][j]);
                clear_fifo(debug_M8._om2_id_buffer_in1_out2[i][j]);
                clear_fifo(debug_M8._om2_id_buffer_in2_out1[i][j]);
                clear_fifo(debug_M8._om2_id_buffer_in2_out2[i][j]);
                clear_fifo(debug_M8._om2_value_buffer_in1_out1[i][j]);
                clear_fifo(debug_M8._om2_value_buffer_in1_out2[i][j]);
                clear_fifo(debug_M8._om2_value_buffer_in2_out1[i][j]);
                clear_fifo(debug_M8._om2_value_buffer_in2_out2[i][j]);
            }
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
            Omega_Network (
                om1_front_push_flag, om1_front_update_v_id, om1_front_update_v_value, om1_front_pull_first_flag, om1_front_update_v_dvalid, om1_front_iteration_end, om1_front_iteration_end_dvalid,
                om2_front_push_flag, om2_front_update_v_id, om2_front_update_v_value, om2_front_pull_first_flag, om2_front_update_v_dvalid, om2_front_iteration_end, om2_front_iteration_end_dvalid,
                source_core_full,

                stage_full_om1, stage_full_om2,
                om1_push_flag, om1_update_v_id, om1_update_v_value, om1_pull_first_flag, om1_update_v_dvalid, om1_iteration_end, om1_iteration_end_dvalid,
                om2_push_flag, om2_update_v_id, om2_update_v_value, om2_pull_first_flag, om2_update_v_dvalid, om2_iteration_end, om2_iteration_end_dvalid
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
TEST_F(Omega_Network_Test, DISABLED_test_Trace) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    FILE* fp = fopen("../data/debug_trace.txt", "r");
    ASSERT_TRUE(fp != nullptr);
    bool break_flag = false;
    int data_in_count = 0;
    int clk = 0;
    while (!break_flag || data_in_count > 0) {
        for (int i = 0; i < CORE_NUM; i++) {
            if (fscanf(fp, "%d", &om1_front_update_v_id[i]) == EOF) {
                break_flag = true;
                break;
            }
            om1_front_push_flag[i] = 1;
            if (om1_front_update_v_id[i] == -1) {
                om1_front_update_v_id[i] = 0;
                om1_front_update_v_dvalid[i] = 0;
            } else {
                om1_front_update_v_dvalid[i] = 1;
                data_in_count++;
            }
            om1_front_update_v_value[i] = 1;
            om1_front_pull_first_flag[i] = 0;
            om1_front_iteration_end[i] = 0;
            om1_front_iteration_end_dvalid[i] = 0;
        }
        if (break_flag) init_input_flag();
        // printf("data_in_count=%d\n", data_in_count);
        run_module(1);
        if (clk % 10000 == 0) printf("clk=%d\n", clk);
        clk++;
        for (int i = 0; i < CORE_NUM; i++) {
            if (om1_update_v_dvalid[i] == 1 && om1_update_v_id[i] % CORE_NUM == i) {
                data_in_count--;
            }
        }
    }
    printf("clk=%d\n", clk);
    run_module(1);
    // check
    for (int i = 0; i < OMEGA_DEPTH; i++) {
        for (int j = 0; j < OMEGA_SWITCH_NUM; j++) {
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in2_out2[i][j].size(), size_t(0));

            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in2_out2[i][j].size(), size_t(0));
        }
    }
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(stage_full_om1[i], 0);
        ASSERT_EQ(stage_full_om2[i], 0);
        ASSERT_EQ(om1_push_flag[i], 0);
        ASSERT_EQ(om1_update_v_id[i], 0);
        ASSERT_EQ(om1_update_v_value[i], 0);
        ASSERT_EQ(om1_pull_first_flag[i], 0);
        ASSERT_EQ(om1_update_v_dvalid[i], 0);
        ASSERT_EQ(om1_iteration_end[i], 0);
        ASSERT_EQ(om1_iteration_end_dvalid[i], 0);

        ASSERT_EQ(stage_full_om1[i], 0);
        ASSERT_EQ(stage_full_om2[i], 0);
        ASSERT_EQ(om1_push_flag[i], 0);
        ASSERT_EQ(om1_update_v_id[i], 0);
        ASSERT_EQ(om1_update_v_value[i], 0);
        ASSERT_EQ(om1_pull_first_flag[i], 0);
        ASSERT_EQ(om1_update_v_dvalid[i], 0);
        ASSERT_EQ(om1_iteration_end[i], 0);
        ASSERT_EQ(om1_iteration_end_dvalid[i], 0);
    }
}

// given: rst=1, ROOT_ID
// while: no control singal set to high
// then: local buffer empty
TEST_F(Omega_Network_Test, test_Push_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < OMEGA_DEPTH; i++) {
        for (int j = 0; j < OMEGA_SWITCH_NUM; j++) {
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in2_out2[i][j].size(), size_t(0));

            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in2_out2[i][j].size(), size_t(0));
        }
    }
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(stage_full_om1[i], 0);
        ASSERT_EQ(stage_full_om2[i], 0);
        ASSERT_EQ(om1_push_flag[i], 0);
        ASSERT_EQ(om1_update_v_id[i], 0);
        ASSERT_EQ(om1_update_v_value[i], 0);
        ASSERT_EQ(om1_pull_first_flag[i], 0);
        ASSERT_EQ(om1_update_v_dvalid[i], 0);
        ASSERT_EQ(om1_iteration_end[i], 0);
        ASSERT_EQ(om1_iteration_end_dvalid[i], 0);

        ASSERT_EQ(stage_full_om1[i], 0);
        ASSERT_EQ(stage_full_om2[i], 0);
        ASSERT_EQ(om1_push_flag[i], 0);
        ASSERT_EQ(om1_update_v_id[i], 0);
        ASSERT_EQ(om1_update_v_value[i], 0);
        ASSERT_EQ(om1_pull_first_flag[i], 0);
        ASSERT_EQ(om1_update_v_dvalid[i], 0);
        ASSERT_EQ(om1_iteration_end[i], 0);
        ASSERT_EQ(om1_iteration_end_dvalid[i], 0);
    }
}

// test: normal mode
// given: rst=0, ROOT_ID, front data valid
// while: no control singal set to high
// then: store and output
TEST_F(Omega_Network_Test, test_PushInOut_given_DataValid_then_OutputValid) {
    // reset
    reset_module();
    // run once to store routing info
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        om1_front_push_flag[i] = 1;
        om1_front_update_v_id[i] = i + 1;
        om1_front_update_v_value[i] = 1;
        om1_front_pull_first_flag[i] = 0;
        om1_front_update_v_dvalid[i] = 1;
        om1_front_iteration_end[i] = 0;
        om1_front_iteration_end_dvalid[i] = 0;
        om2_front_push_flag[i] = 1;
        om2_front_update_v_id[i] = i + 2;
        om2_front_update_v_value[i] = 2;
        om2_front_pull_first_flag[i] = 0;
        om2_front_update_v_dvalid[i] = 1;
        om2_front_iteration_end[i] = 0;
        om2_front_iteration_end_dvalid[i] = 0;
    }
    run_module(1);
    init_input_flag();
    // run 1000 cycle for omega routing
    vector<bool> om1_res_ready(CORE_NUM, false);
    vector<bool> om2_res_ready(CORE_NUM, false);
    for (int i = 0; i < 500; i++) {
        run_module(1);
        for (int j = 0; j < CORE_NUM; j++) {
            if (om1_update_v_dvalid[j] == 1 && om1_update_v_id[j] % CORE_NUM == j && om1_push_flag[j] == 1 && om1_update_v_value[j] == 1) {
                om1_res_ready[j] = true;
            }
            if (om2_update_v_dvalid[j] == 1 && om2_update_v_id[j] % CORE_NUM == j && om2_push_flag[j] == 1 && om2_update_v_value[j] == 2) {
                om2_res_ready[j] = true;
            }
        }
    }
    bool om1_res_ready_all = true;
    bool om2_res_ready_all = true;
    for (int i = 0; i < CORE_NUM; i++) {
        om1_res_ready_all &= om1_res_ready[i];
        om2_res_ready_all &= om2_res_ready[i];
    }
    ASSERT_EQ(om1_res_ready_all, true);
    ASSERT_EQ(om2_res_ready_all, true);
    // check final status
    for (int i = 0; i < OMEGA_DEPTH; i++) {
        for (int j = 0; j < OMEGA_SWITCH_NUM; j++) {
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in2_out2[i][j].size(), size_t(0));

            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in2_out2[i][j].size(), size_t(0));
        }
    }
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(stage_full_om1[i], 0);
        ASSERT_EQ(stage_full_om2[i], 0);
        ASSERT_EQ(om1_push_flag[i], 0);
        ASSERT_EQ(om1_update_v_id[i], 0);
        ASSERT_EQ(om1_update_v_value[i], 0);
        ASSERT_EQ(om1_pull_first_flag[i], 0);
        ASSERT_EQ(om1_update_v_dvalid[i], 0);
        ASSERT_EQ(om1_iteration_end[i], 0);
        ASSERT_EQ(om1_iteration_end_dvalid[i], 0);

        ASSERT_EQ(stage_full_om1[i], 0);
        ASSERT_EQ(stage_full_om2[i], 0);
        ASSERT_EQ(om1_push_flag[i], 0);
        ASSERT_EQ(om1_update_v_id[i], 0);
        ASSERT_EQ(om1_update_v_value[i], 0);
        ASSERT_EQ(om1_pull_first_flag[i], 0);
        ASSERT_EQ(om1_update_v_dvalid[i], 0);
        ASSERT_EQ(om1_iteration_end[i], 0);
        ASSERT_EQ(om1_iteration_end_dvalid[i], 0);
    }
}

// test: pull mode
// given: rst=0, ROOT_ID, front data valid
// while: no control singal set to high
// then: store and output
TEST_F(Omega_Network_Test, test_PullInOut_given_DataValid_then_OutputValid) {
    // reset
    reset_module();
    // run once to store routing info
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        om1_front_push_flag[i] = 0;
        om1_front_update_v_id[i] = 2;
        om1_front_update_v_value[i] = i + 2;
        om1_front_pull_first_flag[i] = 0;
        om1_front_update_v_dvalid[i] = 1;
        om1_front_iteration_end[i] = 0;
        om1_front_iteration_end_dvalid[i] = 0;
        om2_front_push_flag[i] = 0;
        om2_front_update_v_id[i] = i + 2;
        om2_front_update_v_value[i] = 3;
        om2_front_pull_first_flag[i] = 0;
        om2_front_update_v_dvalid[i] = 1;
        om2_front_iteration_end[i] = 0;
        om2_front_iteration_end_dvalid[i] = 0;
    }
    run_module(1);
    init_input_flag();
    // run 1000 cycle for omega routing
    vector<bool> om1_res_ready(CORE_NUM, false);
    vector<bool> om2_res_ready(CORE_NUM, false);
    for (int i = 0; i < 500; i++) {
        run_module(1);
        for (int j = 0; j < CORE_NUM; j++) {
            if (om1_update_v_dvalid[j] == 1 && om1_update_v_value[j] % CORE_NUM == j && om1_push_flag[j] == 0 && om1_update_v_id[j] == 2) {
                om1_res_ready[j] = true;
            }
            if (om2_update_v_dvalid[j] == 1 && om2_update_v_id[j] % CORE_NUM == j && om2_push_flag[j] == 0 && om2_update_v_value[j] == 3) {
                om2_res_ready[j] = true;
            }
        }
    }
    bool om1_res_ready_all = true;
    bool om2_res_ready_all = true;
    for (int i = 0; i < CORE_NUM; i++) {
        om1_res_ready_all &= om1_res_ready[i];
        om2_res_ready_all &= om2_res_ready[i];
    }
    ASSERT_EQ(om1_res_ready_all, true);
    ASSERT_EQ(om2_res_ready_all, true);
    // check final status
    for (int i = 0; i < OMEGA_DEPTH; i++) {
        for (int j = 0; j < OMEGA_SWITCH_NUM; j++) {
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_push_flag_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_id_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om1_value_buffer_in2_out2[i][j].size(), size_t(0));

            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_push_flag_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_id_buffer_in2_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in1_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in1_out2[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in2_out1[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._om2_value_buffer_in2_out2[i][j].size(), size_t(0));
        }
    }
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(stage_full_om1[i], 0);
        ASSERT_EQ(stage_full_om2[i], 0);
        ASSERT_EQ(om1_push_flag[i], 0);
        ASSERT_EQ(om1_update_v_id[i], 0);
        ASSERT_EQ(om1_update_v_value[i], 0);
        ASSERT_EQ(om1_pull_first_flag[i], 0);
        ASSERT_EQ(om1_update_v_dvalid[i], 0);
        ASSERT_EQ(om1_iteration_end[i], 0);
        ASSERT_EQ(om1_iteration_end_dvalid[i], 0);

        ASSERT_EQ(stage_full_om1[i], 0);
        ASSERT_EQ(stage_full_om2[i], 0);
        ASSERT_EQ(om1_push_flag[i], 0);
        ASSERT_EQ(om1_update_v_id[i], 0);
        ASSERT_EQ(om1_update_v_value[i], 0);
        ASSERT_EQ(om1_pull_first_flag[i], 0);
        ASSERT_EQ(om1_update_v_dvalid[i], 0);
        ASSERT_EQ(om1_iteration_end[i], 0);
        ASSERT_EQ(om1_iteration_end_dvalid[i], 0);
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