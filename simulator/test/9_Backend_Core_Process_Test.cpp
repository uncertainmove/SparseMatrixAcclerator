#include "gtest/gtest.h"
#include "Accelerator.h"
#include "debug.h"

#include <iostream>

using namespace std;

#if DEBUG

extern int rst_rd;
extern int ROOT_ID;
extern debug_Backend_Core_Process debug_M9;

class Backend_Core_Process_Test : public testing::Test {
public:
    // input signal
    int om1_recv_push_flag[CORE_NUM] = {0};
    int om1_recv_update_v_id[CORE_NUM] = {0};
    int om1_recv_update_v_value[CORE_NUM] = {0};
    int om1_recv_update_v_pull_first_flag[CORE_NUM] = {0};
    int om1_recv_update_v_dvalid[CORE_NUM] = {0};
    int om1_recv_iteration_end[CORE_NUM] = {0};
    int om1_recv_iteration_end_dvalid[CORE_NUM] = {0};
    int om2_recv_push_flag[CORE_NUM] = {0};
    int om2_recv_update_v_id[CORE_NUM] = {0};
    int om2_recv_update_v_value[CORE_NUM] = {0};
    int om2_recv_update_v_pull_first_flag[CORE_NUM] = {0};
    int om2_recv_update_v_dvalid[CORE_NUM] = {0};
    int om2_recv_iteration_end[CORE_NUM] = {0};
    int om2_recv_iteration_end_dvalid[CORE_NUM] = {0};
    int vertex_bram_data[CORE_NUM] = {0};
    int vertex_bram_dvalid[CORE_NUM] = {0};
    int om_global_iteration_id[CORE_NUM] = {0};
    // output signal
    int source_core_full[CORE_NUM] = {0};
    int rd_vertex_bram_addr[CORE_NUM] = {0};
    int rd_vertex_bram_valid[CORE_NUM] = {0};
    int wr_vertex_bram_push_flag[CORE_NUM] = {0};
    int wr_vertex_bram_addr[CORE_NUM] = {0};
    int wr_vertex_bram_data[CORE_NUM] = {0};
    int wr_vertex_bram_pull_first_flag[CORE_NUM] = {0};
    int wr_vertex_bram_valid[CORE_NUM] = {0};
    int wr_vertex_bram_iteration_end[CORE_NUM] = {0};
    int wr_vertex_bram_iteration_end_dvalid[CORE_NUM] = {0};
    int om2_send_push_flag[CORE_NUM] = {0};
    int om2_send_update_v_id[CORE_NUM] = {0};
    int om2_send_update_v_value[CORE_NUM] = {0};
    int om2_send_update_v_pull_first_flag[CORE_NUM] = {0};
    int om2_send_update_v_dvalid[CORE_NUM] = {0};
    int om2_send_iteration_end[CORE_NUM] = {0};
    int om2_send_iteration_end_dvalid[CORE_NUM] = {0};

    static void SetUpTestCase() {
    }
    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            om1_recv_push_flag[i] = {0};
            om1_recv_update_v_id[i] = {0};
            om1_recv_update_v_value[i] = {0};
            om1_recv_update_v_pull_first_flag[i] = {0};
            om1_recv_update_v_dvalid[i] = {0};
            om1_recv_iteration_end[i] = {0};
            om1_recv_iteration_end_dvalid[i] = {0};
            om2_recv_push_flag[i] = {0};
            om2_recv_update_v_id[i] = {0};
            om2_recv_update_v_value[i] = {0};
            om2_recv_update_v_pull_first_flag[i] = {0};
            om2_recv_update_v_dvalid[i] = {0};
            om2_recv_iteration_end[i] = {0};
            om2_recv_iteration_end_dvalid[i] = {0};
            vertex_bram_data[i] = {0};
            vertex_bram_dvalid[i] = {0};
            om_global_iteration_id[i] = {0};
        }
    }
    void init_local_reg() {
        for (int i = 0; i < CORE_NUM; i++) {
            queue<int> empty1, empty2;
            swap(debug_M9._update_v_id_buffer[i], empty1);
            swap(debug_M9._pull_first_flag_buffer[i], empty2);
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
            Backend_Core (
                om1_recv_push_flag, om1_recv_update_v_id, om1_recv_update_v_value, om1_recv_update_v_pull_first_flag, om1_recv_update_v_dvalid, om1_recv_iteration_end, om1_recv_iteration_end_dvalid,
                om2_recv_push_flag, om2_recv_update_v_id, om2_recv_update_v_value, om2_recv_update_v_pull_first_flag, om2_recv_update_v_dvalid, om2_recv_iteration_end, om2_recv_iteration_end_dvalid,
                vertex_bram_data, vertex_bram_dvalid,
                om_global_iteration_id,

                source_core_full,
                rd_vertex_bram_addr, rd_vertex_bram_valid,
                wr_vertex_bram_push_flag, wr_vertex_bram_addr, wr_vertex_bram_data, wr_vertex_bram_pull_first_flag, wr_vertex_bram_valid, wr_vertex_bram_iteration_end, wr_vertex_bram_iteration_end_dvalid,
                om2_send_push_flag, om2_send_update_v_id, om2_send_update_v_value, om2_send_update_v_pull_first_flag, om2_send_update_v_dvalid, om2_send_iteration_end, om2_send_iteration_end_dvalid
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
TEST_F(Backend_Core_Process_Test, test_Pull_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M9._update_v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M9._pull_first_flag_buffer[i].size(), size_t(0));

        ASSERT_EQ(source_core_full[i], 0);
        ASSERT_EQ(rd_vertex_bram_addr[i], 0);
        ASSERT_EQ(rd_vertex_bram_valid[i], 0);
        ASSERT_EQ(wr_vertex_bram_push_flag[i], 0);
        ASSERT_EQ(wr_vertex_bram_addr[i], 0);
        ASSERT_EQ(wr_vertex_bram_data[i], 0);
        ASSERT_EQ(wr_vertex_bram_pull_first_flag[i], 0);
        ASSERT_EQ(wr_vertex_bram_valid[i], 0);
        ASSERT_EQ(wr_vertex_bram_iteration_end[i], 0);
        ASSERT_EQ(wr_vertex_bram_iteration_end_dvalid[i], 0);
        ASSERT_EQ(om2_send_push_flag[i], 0);
        ASSERT_EQ(om2_send_update_v_id[i], 0);
        ASSERT_EQ(om2_send_update_v_value[i], 0);
        ASSERT_EQ(om2_send_update_v_pull_first_flag[i], 0);
        ASSERT_EQ(om2_send_update_v_dvalid[i], 0);
        ASSERT_EQ(om2_send_iteration_end[i], 0);
        ASSERT_EQ(om2_send_iteration_end_dvalid[i], 0);
    }
}

// test: normal mode
// given: rst=0, ROOT_ID, om1 input valid
// while: no control singal set to high
// then: store and output
TEST_F(Backend_Core_Process_Test, test_Pull_given_OM1Valid_then_StoreAndOutput) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        om1_recv_push_flag[i] = 0;
        om1_recv_update_v_id[i] = i;
        om1_recv_update_v_value[i] = i + 1;
        om1_recv_update_v_pull_first_flag[i] = 1;
        om1_recv_update_v_dvalid[i] = 1;
        om1_recv_iteration_end[i] = 0;
        om1_recv_iteration_end_dvalid[i] = 0;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M9._update_v_id_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M9._pull_first_flag_buffer[i].size(), size_t(1));

        ASSERT_EQ(source_core_full[i], 0);
        ASSERT_EQ(rd_vertex_bram_addr[i], i + 1);
        ASSERT_EQ(rd_vertex_bram_valid[i], 1);
        ASSERT_EQ(wr_vertex_bram_push_flag[i], 0);
        ASSERT_EQ(wr_vertex_bram_addr[i], 0);
        ASSERT_EQ(wr_vertex_bram_data[i], 0);
        ASSERT_EQ(wr_vertex_bram_pull_first_flag[i], 0);
        ASSERT_EQ(wr_vertex_bram_valid[i], 0);
        ASSERT_EQ(wr_vertex_bram_iteration_end[i], 0);
        ASSERT_EQ(wr_vertex_bram_iteration_end_dvalid[i], 0);
        ASSERT_EQ(om2_send_push_flag[i], 0);
        ASSERT_EQ(om2_send_update_v_id[i], 0);
        ASSERT_EQ(om2_send_update_v_value[i], 0);
        ASSERT_EQ(om2_send_update_v_pull_first_flag[i], 0);
        ASSERT_EQ(om2_send_update_v_dvalid[i], 0);
        ASSERT_EQ(om2_send_iteration_end[i], 0);
        ASSERT_EQ(om2_send_iteration_end_dvalid[i], 0);
    }
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        vertex_bram_data[i] = i + 2;
        vertex_bram_dvalid[i] = 1;
    }
    // run to output
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M9._update_v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M9._pull_first_flag_buffer[i].size(), size_t(0));

        ASSERT_EQ(source_core_full[i], 0);
        ASSERT_EQ(rd_vertex_bram_addr[i], 0);
        ASSERT_EQ(rd_vertex_bram_valid[i], 0);
        ASSERT_EQ(wr_vertex_bram_push_flag[i], 0);
        ASSERT_EQ(wr_vertex_bram_addr[i], 0);
        ASSERT_EQ(wr_vertex_bram_data[i], 0);
        ASSERT_EQ(wr_vertex_bram_pull_first_flag[i], 0);
        ASSERT_EQ(wr_vertex_bram_valid[i], 0);
        ASSERT_EQ(wr_vertex_bram_iteration_end[i], 0);
        ASSERT_EQ(wr_vertex_bram_iteration_end_dvalid[i], 0);
        ASSERT_EQ(om2_send_push_flag[i], 0);
        ASSERT_EQ(om2_send_update_v_id[i], i);
        ASSERT_EQ(om2_send_update_v_value[i], i + 2);
        ASSERT_EQ(om2_send_update_v_pull_first_flag[i], 1);
        ASSERT_EQ(om2_send_update_v_dvalid[i], 1);
        ASSERT_EQ(om2_send_iteration_end[i], 0);
        ASSERT_EQ(om2_send_iteration_end_dvalid[i], 0);
    }
}

// test: normal mode
// given: rst=0, ROOT_ID, om2 input valid
// while: no control singal set to high
// then: store and output
TEST_F(Backend_Core_Process_Test, test_Pull_given_OM2Valid_then_StoreAndOutput) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        om2_recv_push_flag[i] = 0;
        om2_recv_update_v_id[i] = i;
        om2_recv_update_v_value[i] = i + 1;
        om2_recv_update_v_pull_first_flag[i] = 1;
        om2_recv_update_v_dvalid[i] = 1;
        om2_recv_iteration_end[i] = 0;
        om2_recv_iteration_end_dvalid[i] = 0;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M9._update_v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M9._pull_first_flag_buffer[i].size(), size_t(0));

        ASSERT_EQ(source_core_full[i], 0);
        ASSERT_EQ(rd_vertex_bram_addr[i], 0);
        ASSERT_EQ(rd_vertex_bram_valid[i], 0);
        ASSERT_EQ(wr_vertex_bram_push_flag[i], 0);
        ASSERT_EQ(wr_vertex_bram_addr[i], i);
        ASSERT_EQ(wr_vertex_bram_data[i], i + 1);
        ASSERT_EQ(wr_vertex_bram_pull_first_flag[i], 1);
        ASSERT_EQ(wr_vertex_bram_valid[i], 1);
        ASSERT_EQ(wr_vertex_bram_iteration_end[i], 0);
        ASSERT_EQ(wr_vertex_bram_iteration_end_dvalid[i], 0);
        ASSERT_EQ(om2_send_push_flag[i], 0);
        ASSERT_EQ(om2_send_update_v_id[i], 0);
        ASSERT_EQ(om2_send_update_v_value[i], 0);
        ASSERT_EQ(om2_send_update_v_pull_first_flag[i], 0);
        ASSERT_EQ(om2_send_update_v_dvalid[i], 0);
        ASSERT_EQ(om2_send_iteration_end[i], 0);
        ASSERT_EQ(om2_send_iteration_end_dvalid[i], 0);
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