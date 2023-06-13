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
extern debug_Vertex_RAM_Controller debug_M10;

class Vertex_RAM_Controller_Test : public testing::Test {
public:
    // input signal
    int rd_addr_src[CORE_NUM] = {0};
    int rd_valid_src[CORE_NUM] = {0};
    int wr_addr_dst[CORE_NUM] = {0};
    V_VALUE_TP wr_v_value_dst[CORE_NUM] = {0};
    int wr_valid_dst[CORE_NUM] = {0};
    int front_iteration_end[CORE_NUM] = {0};
    int front_iteration_end_dvalid[CORE_NUM] = {0};
    int front_iteration_id[CORE_NUM] = {0};

    V_VALUE_TP delta1_bram_data[CORE_NUM] = {0};
    int delta1_bram_data_valid[CORE_NUM] = {0};
    V_VALUE_TP delta2_bram_data[CORE_NUM] = {0};
    int delta2_bram_data_valid[CORE_NUM] = {0};
    V_VALUE_TP pr_uram_data[CORE_NUM] = {0};
    int pr_uram_data_valid[CORE_NUM] = {0};
    // output signal
    int delta1_bram_rd_addr[CORE_NUM] = {0};
    int delta1_bram_rd_valid[CORE_NUM] = {0};
    int delta2_bram_rd_addr[CORE_NUM] = {0};
    int delta2_bram_rd_valid[CORE_NUM] = {0};
    int pr_uram_rd_addr[CORE_NUM] = {0};
    int pr_uram_rd_valid[CORE_NUM] = {0};
    int delta1_bram_wr_addr[CORE_NUM] = {0};
    V_VALUE_TP delta1_bram_wr_value[CORE_NUM] = {0};
    int delta1_bram_wr_valid[CORE_NUM] = {0};
    int delta2_bram_wr_addr[CORE_NUM] = {0};
    V_VALUE_TP delta2_bram_wr_value[CORE_NUM] = {0};
    int delta2_bram_wr_valid[CORE_NUM] = {0};
    int pr_uram_wr_addr[CORE_NUM] = {0};
    V_VALUE_TP pr_uram_wr_value[CORE_NUM] = {0};
    int pr_uram_wr_valid[CORE_NUM] = {0};

    int stage_full[CORE_NUM] = {0};
    V_VALUE_TP src_recv_update_v_value[CORE_NUM] = {0};
    int src_recv_update_v_dvalid[CORE_NUM] = {0};
    int backend_active_v_id[CORE_NUM] = {0};
    int backend_active_v_updated[CORE_NUM] = {0};
    int backend_active_v_dvalid[CORE_NUM] = {0};
    int iteration_end[CORE_NUM] = {0};
    int iteration_end_dvalid[CORE_NUM] = {0};

    static void SetUpTestCase() {
    }
    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            rd_addr_src[i] = {0};
            rd_valid_src[i] = {0};
            wr_addr_dst[i] = {0};
            wr_v_value_dst[i] = {0.0};
            wr_valid_dst[i] = {0};
            front_iteration_end[i] = {0};
            front_iteration_end_dvalid[i] = {0};
            front_iteration_id[i] = {0};

            delta1_bram_data[i] = {0.0};
            delta1_bram_data_valid[i] = {0};
            delta2_bram_data[i] = {0.0};
            delta2_bram_data_valid[i] = {0};
            pr_uram_data[i] = {0.0};
            pr_uram_data_valid[i] = {0};
        }
    }
    void init_local_reg() {
        for (int i = 0; i < CORE_NUM; i++) {
            clear_buffer(debug_M10._wr_delta_addr_buffer[i]);
            clear_buffer(debug_M10._wr_delta_value_buffer[i]);
            clear_buffer(debug_M10._wr_pr_addr_buffer[i]);
            clear_buffer(debug_M10._wr_pr_value_buffer[i]);
            clear_buffer(debug_M10._active_addr_buffer[i]);
            clear_buffer(debug_M10._active_delta_buffer[i]);
            clear_buffer(debug_M10._active_pr_buffer[i]);
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
            Vertex_RAM_Controller (
                rd_addr_src, rd_valid_src,
                wr_addr_dst, wr_v_value_dst, wr_valid_dst,
                front_iteration_end, front_iteration_end_dvalid,
                front_iteration_id,

                delta1_bram_data, delta1_bram_data_valid,
                delta2_bram_data, delta2_bram_data_valid,
                pr_uram_data, pr_uram_data_valid,
                delta1_bram_rd_addr, delta1_bram_rd_valid,
                delta2_bram_rd_addr, delta2_bram_rd_valid,
                delta1_bram_wr_addr, delta1_bram_wr_value, delta1_bram_wr_valid,
                delta2_bram_wr_addr, delta2_bram_wr_value, delta2_bram_wr_valid,
                pr_uram_rd_addr, pr_uram_rd_valid,
                pr_uram_wr_addr, pr_uram_wr_value, pr_uram_wr_valid,

                stage_full,
                src_recv_update_v_value, src_recv_update_v_dvalid,
                backend_active_v_id, backend_active_v_updated, backend_active_v_dvalid,
                iteration_end, iteration_end_dvalid
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
TEST_F(Vertex_RAM_Controller_Test, test_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// test: normal mode
// given: rst=0, rd input valid
// while: no control singal set to high
// then: store and output
TEST_F(Vertex_RAM_Controller_Test, test_given_RdAddrValid0_then_ReadData) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        rd_addr_src[i] = i;
        rd_valid_src[i] = 1;
        front_iteration_id[i] = 0;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], i);
        ASSERT_EQ(delta1_bram_rd_valid[i], 1);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // run to output
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        delta1_bram_data[i] = 1.0;
        delta1_bram_data_valid[i] = 1;
    }
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 1.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 1);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// test: normal mode
// given: rst=0, rd input valid
// while: no control singal set to high
// then: store and output
TEST_F(Vertex_RAM_Controller_Test, test_given_RdAddrValid1_then_ReadData) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        rd_addr_src[i] = i;
        rd_valid_src[i] = 1;
        front_iteration_id[i] = 1;
    };
    cout << "clk 1" << endl;
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], i);
        ASSERT_EQ(delta2_bram_rd_valid[i], 1);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // run to output
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        delta2_bram_data[i] = 1.0;
        delta2_bram_data_valid[i] = 1;
        front_iteration_id[i] = 1;
    }
    cout << "clk 2" << endl;
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 1.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 1);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// test: normal mode
// given: rst=0, wr input valid
// while: no control singal set to high
// then: store and output
TEST_F(Vertex_RAM_Controller_Test, test_given_WrAddrValid0_then_WriteData) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        wr_addr_dst[i] = i;
        wr_v_value_dst[i] = 1.0;
        wr_valid_dst[i] = 1;
        front_iteration_id[i] = 0;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // run to output
    init_input_flag();
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], i / CORE_NUM);
        ASSERT_EQ(delta2_bram_rd_valid[i], 1);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], i / CORE_NUM);
        ASSERT_EQ(pr_uram_rd_valid[i], 1);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // set pr and delta2 data
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        delta2_bram_data[i] = 1.0;
        delta2_bram_data_valid[i] = 1;
        pr_uram_data[i] = 2.0;
        pr_uram_data_valid[i] = 1;
    }
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(1));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], i / CORE_NUM);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 2.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 1);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], i / CORE_NUM);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 3.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 1);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    init_input_flag();
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], i);
        ASSERT_EQ(backend_active_v_updated[i], 1);
        ASSERT_EQ(backend_active_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// test: normal mode
// given: rst=0, wr input valid
// while: no control singal set to high
// then: store and output
TEST_F(Vertex_RAM_Controller_Test, test_given_WrAddrValid1_then_WriteData) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        wr_addr_dst[i] = i;
        wr_v_value_dst[i] = 1.0;
        wr_valid_dst[i] = 1;
        front_iteration_id[i] = 1;
    }
    cout << "clk 1" << endl;
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // run to output
    init_input_flag();
    cout << "clk 2" << endl;
    for (int i = 0; i < CORE_NUM; i++) {
        front_iteration_id[i] = 1;
    }
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], i / CORE_NUM);
        ASSERT_EQ(delta1_bram_rd_valid[i], 1);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], i / CORE_NUM);
        ASSERT_EQ(pr_uram_rd_valid[i], 1);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // set pr and delta2 data
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        delta1_bram_data[i] = 1.0;
        delta1_bram_data_valid[i] = 1;
        pr_uram_data[i] = 2.0;
        pr_uram_data_valid[i] = 1;
        front_iteration_id[i] = 1;
    }
    cout << "clk 3" << endl;
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(1));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], i / CORE_NUM);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 2.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 1);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], i / CORE_NUM);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 3.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 1);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        front_iteration_id[i] = 1;
    }
    cout << "clk 4" << endl;
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], i);
        ASSERT_EQ(backend_active_v_updated[i], 1);
        ASSERT_EQ(backend_active_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// test: normal mode
// given: rst=0, wr input valid
// while: no control singal set to high
// then: store and output
TEST_F(Vertex_RAM_Controller_Test, test_given_WrAddrConflict_then_WriteDataBlock) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        wr_addr_dst[i] = i;
        wr_v_value_dst[i] = 1.0;
        wr_valid_dst[i] = 1;
        front_iteration_id[i] = 0;
    }
    run_module(2);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(2));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(2));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], i / CORE_NUM);
        ASSERT_EQ(delta2_bram_rd_valid[i], 1);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], i / CORE_NUM);
        ASSERT_EQ(pr_uram_rd_valid[i], 1);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // set pr and delta2 data
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        delta2_bram_data[i] = 1.0;
        delta2_bram_data_valid[i] = 1;
        pr_uram_data[i] = 2.0;
        pr_uram_data_valid[i] = 1;
    }
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(1));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], i / CORE_NUM);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 2.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 1);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], i / CORE_NUM);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 3.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 1);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    init_input_flag();
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], i);
        ASSERT_EQ(backend_active_v_updated[i], 1);
        ASSERT_EQ(backend_active_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    run_module(FLOAT_ADD_DELAY - 2);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], i / CORE_NUM);
        ASSERT_EQ(delta2_bram_rd_valid[i], 1);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], i / CORE_NUM);
        ASSERT_EQ(pr_uram_rd_valid[i], 1);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // set pr and delta2 data
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        delta2_bram_data[i] = 1.0;
        delta2_bram_data_valid[i] = 1;
        pr_uram_data[i] = 2.0;
        pr_uram_data_valid[i] = 1;
    }
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(1));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], i / CORE_NUM);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 2.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 1);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], i / CORE_NUM);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 3.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 1);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    init_input_flag();
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], i);
        ASSERT_EQ(backend_active_v_updated[i], 1);
        ASSERT_EQ(backend_active_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._wr_delta_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_delta_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._wr_pr_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_delta_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._active_pr_buffer[i].size(), size_t(0));

        ASSERT_EQ(delta1_bram_rd_addr[i], 0);
        ASSERT_EQ(delta1_bram_rd_valid[i], 0);
        ASSERT_EQ(delta2_bram_rd_addr[i], 0);
        ASSERT_EQ(delta2_bram_rd_valid[i], 0);
        ASSERT_EQ(delta1_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta1_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta1_bram_wr_valid[i], 0);
        ASSERT_EQ(delta2_bram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(delta2_bram_wr_value[i], 0.0);
        ASSERT_EQ(delta2_bram_wr_valid[i], 0);
        ASSERT_EQ(pr_uram_rd_addr[i], 0);
        ASSERT_EQ(pr_uram_rd_valid[i], 0);
        ASSERT_EQ(pr_uram_wr_addr[i], 0);
        ASSERT_FLOAT_EQ(pr_uram_wr_value[i], 0.0);
        ASSERT_EQ(pr_uram_wr_valid[i], 0);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_FLOAT_EQ(src_recv_update_v_value[i], 0.0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
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