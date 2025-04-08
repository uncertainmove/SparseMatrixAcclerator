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
extern debug_Backend_Core_Process debug_M9;

class Backend_Core_Process_Test : public testing::Test {
public:
    // input signal
    int hpx_recv_update_v_id[CORE_NUM] = {0};
    V_VALUE_TP hpx_recv_update_v_value[CORE_NUM] = {0};
    int hpx_recv_update_v_dvalid[CORE_NUM] = {0};
    int hpx_recv_iteration_end[CORE_NUM] = {0};
    int hpx_recv_iteration_end_dvalid[CORE_NUM] = {0};
    int hpx_recv_iteration_id[CORE_NUM] = {0};
    int next_stagefull[CORE_NUM] = {0};
    // output signal
    int dest_core_full[CORE_NUM] = {0};
    int wr_vertex_bram_addr[CORE_NUM] = {0};
    V_VALUE_TP wr_vertex_bram_data[CORE_NUM] = {0};
    int wr_vertex_bram_valid[CORE_NUM] = {0};
    int wr_vertex_bram_iteration_end[CORE_NUM] = {0};
    int wr_vertex_bram_iteration_end_dvalid[CORE_NUM] = {0};
    int wr_vertex_bram_iteration_id[CORE_NUM] = {0};

    static void SetUpTestCase() {
    }
    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            hpx_recv_update_v_id[i] = {0};
            hpx_recv_update_v_value[i] = {0.0};
            hpx_recv_update_v_dvalid[i] = {0};
            hpx_recv_iteration_end[i] = {0};
            hpx_recv_iteration_end_dvalid[i] = {0};
            hpx_recv_iteration_id[i] = {0};
            next_stagefull[i] = {0};
        }
    }
    void init_local_reg() {
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
            Backend_Core (
                hpx_recv_update_v_id, hpx_recv_update_v_value, hpx_recv_update_v_dvalid,
                hpx_recv_iteration_end, hpx_recv_iteration_end_dvalid, hpx_recv_iteration_id,
                next_stagefull,

                dest_core_full,
                wr_vertex_bram_addr, wr_vertex_bram_data, wr_vertex_bram_valid,
                wr_vertex_bram_iteration_end, wr_vertex_bram_iteration_end_dvalid, wr_vertex_bram_iteration_id
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
TEST_F(Backend_Core_Process_Test, test_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(dest_core_full[i], 0);
        ASSERT_EQ(wr_vertex_bram_addr[i], 0);
        ASSERT_EQ(wr_vertex_bram_data[i], 0);
        ASSERT_EQ(wr_vertex_bram_valid[i], 0);
        ASSERT_EQ(wr_vertex_bram_iteration_end[i], 0);
        ASSERT_EQ(wr_vertex_bram_iteration_end_dvalid[i], 0);
        ASSERT_EQ(wr_vertex_bram_iteration_id[i], 0);
    }
}

// test: normal mode
// given: rst=0, input valid
// while: no control singal set to high
// then: store and output
TEST_F(Backend_Core_Process_Test, test_given_InputValid_then_StoreAndOutput) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        hpx_recv_update_v_id[i] = i;
        hpx_recv_update_v_value[i] = 1.0;
        hpx_recv_update_v_dvalid[i] = 1;
        hpx_recv_iteration_end[i] = 0;
        hpx_recv_iteration_end_dvalid[i] = 0;
        hpx_recv_iteration_id[i] = 0;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(dest_core_full[i], 0);
        ASSERT_EQ(wr_vertex_bram_addr[i], i);
        ASSERT_FLOAT_EQ(wr_vertex_bram_data[i], 1.0);
        ASSERT_EQ(wr_vertex_bram_valid[i], 1);
        ASSERT_EQ(wr_vertex_bram_iteration_end[i], 0);
        ASSERT_EQ(wr_vertex_bram_iteration_end_dvalid[i], 0);
        ASSERT_EQ(wr_vertex_bram_iteration_id[i], 0);
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