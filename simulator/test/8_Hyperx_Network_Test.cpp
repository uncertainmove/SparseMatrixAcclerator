#include "gtest/gtest.h"
#include "debug.h"
#include "network.h"
#include "parameter.h"
#include "structure.h"
#include "util.h"

#include <iostream>

using namespace std;

#if DEBUG

extern int rst_rd;
extern debug_Noc debug_M8;

class Hyperx_Network_Test : public testing::Test {
public:
    // input signal
    int hy1_front_update_v_id[CORE_NUM] = {0};
    V_VALUE_TP hy1_front_update_v_value[CORE_NUM] = {0};
    int hy1_front_update_v_dvalid[CORE_NUM] = {0};
    int hy1_front_iteration_end[CORE_NUM] = {0};
    int hy1_front_iteration_end_dvalid[CORE_NUM] = {0};
    int hy1_front_iteration_id[CORE_NUM] = {0};
    int dest_core_full[CORE_NUM] = {0};
    // output signal
    int hy1_update_v_id[CORE_NUM] = {0};
    V_VALUE_TP hy1_update_v_value[CORE_NUM] = {0};
    int hy1_update_v_dvalid[CORE_NUM] = {0};
    int hy1_iteration_end[CORE_NUM] = {0};
    int hy1_iteration_end_dvalid[CORE_NUM] = {0};
    int hy1_iteration_id[CORE_NUM] = {0};
    int hy1_stage_full[CORE_NUM] = {0};

    static void SetUpTestCase() {
    }
    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            hy1_front_update_v_id[i] = {0};
            hy1_front_update_v_value[i] = {0.0};
            hy1_front_update_v_dvalid[i] = {0};
            hy1_front_iteration_end[i] = {0};
            hy1_front_iteration_end_dvalid[i] = {0};
            hy1_front_iteration_id[i] = {0};
            dest_core_full[i] = {0};
        }
    }
    void init_local_reg() {
        for (int i = 0; i < HPX_ROW_NUM; i++) {
            for (int j = 0; j < HPX_COLUMN_NUM; j++) {
                clear_buffer(debug_M8._column_v_id_buffer[i][j]);
                clear_buffer(debug_M8._column_v_value_buffer[i][j]);
                clear_buffer(debug_M8._row_v_id_buffer[i][j]);
                clear_buffer(debug_M8._row_v_value_buffer[i][j]);
                clear_buffer(debug_M8._v_id_buffer[i][j]);
                clear_buffer(debug_M8._v_value_buffer[i][j]);
            }
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
            Hyperx_Network (
                hy1_front_update_v_id, hy1_front_update_v_value, hy1_front_update_v_dvalid,
                hy1_front_iteration_end, hy1_front_iteration_end_dvalid, hy1_front_iteration_id,
                dest_core_full,

                hy1_update_v_id, hy1_update_v_value, hy1_update_v_dvalid,
                hy1_iteration_end, hy1_iteration_end_dvalid, hy1_iteration_id,
                hy1_stage_full
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
TEST_F(Hyperx_Network_Test, DISABLED_test_Trace) {
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
            if (fscanf(fp, "%d", &hy1_front_update_v_id[i]) == EOF) {
                break_flag = true;
                break;
            }
            if (hy1_front_update_v_id[i] == -1) {
                hy1_front_update_v_id[i] = 0;
                hy1_front_update_v_dvalid[i] = 0;
            } else {
                hy1_front_update_v_dvalid[i] = 1;
                data_in_count++;
            }
            hy1_front_update_v_value[i] = 1.0;
            hy1_front_iteration_end[i] = 0;
            hy1_front_iteration_end_dvalid[i] = 0;
            hy1_front_iteration_id[i] = 0;
        }
        if (break_flag) init_input_flag();
        // printf("data_in_count=%d\n", data_in_count);
        run_module(1);
        if (clk % 10000 == 0) printf("clk=%d\n", clk);
        clk++;
        for (int i = 0; i < CORE_NUM; i++) {
            if (hy1_update_v_dvalid[i] == 1 && hy1_update_v_id[i] % CORE_NUM == i) {
                data_in_count--;
            }
        }
    }
    printf("clk=%d\n", clk);
    run_module(1);
    // check
    for (int i = 0; i < HPX_ROW_NUM; i++) {
        for (int j = 0; j < HPX_COLUMN_NUM; j++) {
            ASSERT_EQ(debug_M8._column_v_id_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._column_v_value_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._row_v_id_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._row_v_value_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._v_id_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._v_value_buffer[i][j].size(), size_t(0));
        }
    }
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(hy1_stage_full[i], 0);
        ASSERT_EQ(hy1_update_v_id[i], 0);
        ASSERT_FLOAT_EQ(hy1_update_v_value[i], 0.0);
        ASSERT_EQ(hy1_update_v_dvalid[i], 0);
        ASSERT_EQ(hy1_iteration_end[i], 0);
        ASSERT_EQ(hy1_iteration_end_dvalid[i], 0);
        ASSERT_EQ(hy1_iteration_id[i], 0);
    }
}

// given: rst=1
// while: no control singal set to high
// then: local buffer empty
TEST_F(Hyperx_Network_Test, test_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < HPX_ROW_NUM; i++) {
        for (int j = 0; j < HPX_COLUMN_NUM; j++) {
            ASSERT_EQ(debug_M8._column_v_id_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._column_v_value_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._row_v_id_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._row_v_value_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._v_id_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._v_value_buffer[i][j].size(), size_t(0));
        }
    }
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(hy1_stage_full[i], 0);
        ASSERT_EQ(hy1_update_v_id[i], 0);
        ASSERT_FLOAT_EQ(hy1_update_v_value[i], 0.0);
        ASSERT_EQ(hy1_update_v_dvalid[i], 0);
        ASSERT_EQ(hy1_iteration_end[i], 0);
        ASSERT_EQ(hy1_iteration_end_dvalid[i], 0);
        ASSERT_EQ(hy1_iteration_id[i], 0);
    }
}

// test: normal mode
// given: rst=0, front data valid
// while: no control singal set to high
// then: store and output
TEST_F(Hyperx_Network_Test, test_InOut_given_DataValid_then_OutputValid) {
    // reset
    reset_module();
    // run once to store routing info
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        hy1_front_update_v_id[i] = i + 1;
        hy1_front_update_v_value[i] = 0.1;
        hy1_front_update_v_dvalid[i] = 1;
        hy1_front_iteration_end[i] = 0;
        hy1_front_iteration_end_dvalid[i] = 0;
        hy1_front_iteration_id[i] = 0;
    }
    run_module(1);
    init_input_flag();
    int transfer_count = 0;
    // run 1000 cycle for hyperx routing
    vector<bool> hy1_res_ready(CORE_NUM, false);
    for (int i = 0; i < 500; i++) {
        run_module(1);
        for (int j = 0; j < CORE_NUM; j++) {
            if (hy1_update_v_dvalid[j] == 1 && hy1_update_v_id[j] % CORE_NUM == j) {
                ASSERT_FLOAT_EQ(hy1_update_v_value[j], 0.1);
                hy1_res_ready[j] = true;
                transfer_count++;
            }
        }
    }
    ASSERT_EQ(transfer_count, CORE_NUM);
    bool hy1_res_ready_all = true;
    for (int i = 0; i < CORE_NUM; i++) {
        hy1_res_ready_all &= hy1_res_ready[i];
        cout << hy1_res_ready[i] << " ";
    }
    printf("\n");
    ASSERT_EQ(hy1_res_ready_all, true);
    // check final status
    for (int i = 0; i < HPX_ROW_NUM; i++) {
        for (int j = 0; j < HPX_COLUMN_NUM; j++) {
            ASSERT_EQ(debug_M8._column_v_id_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._column_v_value_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._row_v_id_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._row_v_value_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._v_id_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M8._v_value_buffer[i][j].size(), size_t(0));
        }
    }
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(hy1_stage_full[i], 0);
        ASSERT_EQ(hy1_update_v_id[i], 0);
        ASSERT_FLOAT_EQ(hy1_update_v_value[i], 0.0);
        ASSERT_EQ(hy1_update_v_dvalid[i], 0);
        ASSERT_EQ(hy1_iteration_end[i], 0);
        ASSERT_EQ(hy1_iteration_end_dvalid[i], 0);
        ASSERT_EQ(hy1_iteration_id[i], 0);
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