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
extern int ROOT_ID;
extern debug_RD_Active_Vertex_Edge debug_M4;

class RD_Active_Vertex_Edge_Test : public testing::Test {
public:
    // input signal
    int front_active_v_id[CORE_NUM] = {0};
    int front_active_v_dvalid[CORE_NUM] = {0};
    int front_iteration_end[CORE_NUM] = {0};
    int front_iteration_end_dvalid[CORE_NUM] = {0};
    int front_iteration_id[CORE_NUM] = {0};
    int front_active_v_loffset[CORE_NUM] = {0};
    int front_active_v_roffset[CORE_NUM] = {0};
    int front_offset_dvalid[CORE_NUM] = {0};
    V_VALUE_TP front_active_v_value[CORE_NUM] = {0};
    int front_active_v_value_valid[CORE_NUM] = {0};
    int combinestage_full[CORE_NUM] = {0};
    // output signal
    int stage_full[CORE_NUM];
    int rd_hbm_edge_addr[CORE_NUM];
    BitVector_16 rd_hbm_edge_mask[CORE_NUM];
    int hbm_active_v_id[CORE_NUM];
    V_VALUE_TP hbm_active_v_value[CORE_NUM];
    int rd_hbm_edge_valid[CORE_NUM];
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
            front_active_v_dvalid[i] = {0};
            front_iteration_end[i] = {0};
            front_iteration_end_dvalid[i] = {0};
            front_iteration_id[i] = {0};
            front_active_v_loffset[i] = {0};
            front_active_v_roffset[i] = {0};
            front_offset_dvalid[i] = {0};
            front_active_v_value[i] = {0};
            front_active_v_value_valid[i] = {0};
            combinestage_full[i] = {0};
        }
    }
    void init_local_reg() {
        for (int i = 0; i < CORE_NUM; i++) {
            clear_buffer(debug_M4._v_id_buffer[i]);
            clear_buffer(debug_M4._v_value_buffer[i]);
            clear_buffer(debug_M4._v_loffset_buffer[i]);
            clear_buffer(debug_M4._v_roffset_buffer[i]);
            debug_M4._now_loffset[i] = 0;
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
            RD_Active_Vertex_Edge (
                front_active_v_id, front_active_v_dvalid,
                front_iteration_end, front_iteration_end_dvalid, front_iteration_id,
                front_active_v_loffset, front_active_v_roffset, front_offset_dvalid,
                front_active_v_value, front_active_v_value_valid,
                combinestage_full,

                stage_full,
                rd_hbm_edge_addr, rd_hbm_edge_mask, 
                hbm_active_v_id, hbm_active_v_value, rd_hbm_edge_valid,
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
TEST_F(RD_Active_Vertex_Edge_Test, test_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M4._now_loffset[i], 1000000000);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(rd_hbm_edge_addr[i], 0);
        ASSERT_TRUE(rd_hbm_edge_mask[i].compare(BitVector_16(false)));
        ASSERT_EQ(hbm_active_v_id[i], 0);
        ASSERT_FLOAT_EQ(hbm_active_v_value[i], 0.0);
        ASSERT_EQ(rd_hbm_edge_valid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
}

// test: write & read
// given: rst=0, front addr is valid
// while: no control singal set to high
// then: normal input and output
TEST_F(RD_Active_Vertex_Edge_Test, test_given_InputActiveId_then_OutputId) {
    // reset
    reset_module();
    // run once to write active id
    init_input_flag();
    int sum = 0;
    for (int i = 0; i < CORE_NUM; i++) {
        int front_sum = sum;
        sum += i;
        if (i % 2 == 0) {
            front_active_v_id[i] = i;
            front_active_v_dvalid[i] = 1;
            front_active_v_loffset[i] = front_sum;
            front_active_v_roffset[i] = sum;
            front_offset_dvalid[i] = 1;
            front_active_v_value[i] = 1.0;
            front_active_v_value_valid[i] = 1;
        }
    }
    run_module(1);
    // check
    int sum_check = 0;
    for (int i = 0; i < CORE_NUM; i++) {
        int value = (i + 1) % 2;
        ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(value));
        ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(value));
        ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(value));
        ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(value));
        ASSERT_EQ(debug_M4._now_loffset[i], 1000000000);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(rd_hbm_edge_addr[i], 0);
        ASSERT_TRUE(rd_hbm_edge_mask[i].compare(BitVector_16(false)));
        ASSERT_EQ(hbm_active_v_id[i], 0);
        ASSERT_FLOAT_EQ(hbm_active_v_value[i], 0.0);
        ASSERT_EQ(rd_hbm_edge_valid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
    // run once to output addr
    init_input_flag();
    run_module(1);
    // check
    sum_check = 0;
    for (int i = 0; i < CORE_NUM; i++) {
        int front_sum = sum_check;
        sum_check += i;
        if (i % 2 == 0) {
            if (front_sum == sum_check) {
                ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(0));
                ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(0));
                ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(0));
                ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(0));

                ASSERT_EQ(debug_M4._now_loffset[i], 1000000000);
                // ASSERT_EQ(debug_M4._init_flag[i], 1);
                ASSERT_EQ(stage_full[i], 0);
                ASSERT_EQ(rd_hbm_edge_addr[i], 0);
                BitVector_16 bv(false);
                // rd_hbm_edge_mask[i].show();
                // bv.show();
                ASSERT_TRUE(rd_hbm_edge_mask[i].compare(bv));
                ASSERT_EQ(hbm_active_v_id[i], 0);
                ASSERT_FLOAT_EQ(hbm_active_v_value[i], 0.0);
                ASSERT_EQ(rd_hbm_edge_valid[i], 0);
                ASSERT_EQ(iteration_end[i], 0);
                ASSERT_EQ(iteration_end_dvalid[i], 0);
                ASSERT_EQ(iteration_id[i], 0);
            } else if (front_sum / CACHELINE_LEN == (sum_check - 1) / CACHELINE_LEN) {
                ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(0));
                ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(0));
                ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(0));
                ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(0));

                ASSERT_EQ(debug_M4._now_loffset[i], 1000000000);
                // ASSERT_EQ(debug_M4._init_flag[i], 1);
                ASSERT_EQ(stage_full[i], 0);
                ASSERT_EQ(rd_hbm_edge_addr[i], front_sum / CACHELINE_LEN);
                BitVector_16 bv(false);
                for (int j = front_sum % CACHELINE_LEN; j <= (sum_check - 1) % CACHELINE_LEN; j++) {
                    bv.flag[j] = true;
                }
                // rd_hbm_edge_mask[i].show();
                // bv.show();
                ASSERT_TRUE(rd_hbm_edge_mask[i].compare(bv));
                ASSERT_EQ(hbm_active_v_id[i], i);
                ASSERT_FLOAT_EQ(hbm_active_v_value[i], 1.0 / (sum_check - front_sum) * DAMPING);
                ASSERT_EQ(rd_hbm_edge_valid[i], 1);
                ASSERT_EQ(iteration_end[i], 0);
                ASSERT_EQ(iteration_end_dvalid[i], 0);
                ASSERT_EQ(iteration_id[i], 0);
            } else {
                ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(1));
                ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(1));
                ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(1));
                ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(1));

                ASSERT_EQ(debug_M4._now_loffset[i], (front_sum / CACHELINE_LEN + 1) * CACHELINE_LEN);

                ASSERT_EQ(stage_full[i], 0);
                ASSERT_EQ(rd_hbm_edge_addr[i], front_sum / CACHELINE_LEN);
                BitVector_16 bv(false);
                for (int j = front_sum % CACHELINE_LEN; j < CACHELINE_LEN; j++) {
                    bv.flag[j] = true;
                }
                // rd_hbm_edge_mask[i].show();
                // bv.show();
                ASSERT_TRUE(rd_hbm_edge_mask[i].compare(bv));
                ASSERT_EQ(hbm_active_v_id[i], i);
                ASSERT_FLOAT_EQ(hbm_active_v_value[i], 1.0 / (sum_check - front_sum) * DAMPING);
                ASSERT_EQ(rd_hbm_edge_valid[i], !(front_sum == sum_check));
                ASSERT_EQ(iteration_end[i], 0);
                ASSERT_EQ(iteration_end_dvalid[i], 0);
                ASSERT_EQ(iteration_id[i], 0);
            }
        } else {
            ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(0));

            ASSERT_EQ(debug_M4._now_loffset[i], 1000000000);
            // ASSERT_EQ(debug_M4._init_flag[i], 1);
            ASSERT_EQ(stage_full[i], 0);
            ASSERT_EQ(rd_hbm_edge_addr[i], 0);
            ASSERT_TRUE(rd_hbm_edge_mask[i].compare(BitVector_16()));
            ASSERT_EQ(hbm_active_v_id[i], 0);
            ASSERT_FLOAT_EQ(hbm_active_v_value[i], 0.0);
            ASSERT_EQ(rd_hbm_edge_valid[i], 0);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
            ASSERT_EQ(iteration_id[i], 0);
        }
    }
}

// test: local offset logic
// given: rst=0, now_loffset+=16
// while: no control singal set to high
// then: output and now_loffset increase
TEST_F(RD_Active_Vertex_Edge_Test, test_given_OutputSignal_while_LocalOffsetValid_then_LocalOffsetIncrease) {
    // reset
    reset_module();
    // run once to write active id
    init_input_flag();
    front_active_v_id[0] = 0;
    front_active_v_dvalid[0] = 1;
    front_active_v_loffset[0] = 0;
    front_active_v_roffset[0] = 46;
    front_offset_dvalid[0] = 1;
    front_active_v_value[0] = 1.0;
    front_active_v_value_valid[0] = 1;
    run_module(1);
    // check
    ASSERT_EQ(debug_M4._v_id_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_value_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_loffset_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_roffset_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._now_loffset[0], 1000000000);

    ASSERT_EQ(stage_full[0], 0);
    ASSERT_EQ(rd_hbm_edge_addr[0], 0);
    ASSERT_TRUE(rd_hbm_edge_mask[0].compare(BitVector_16(false)));
    ASSERT_EQ(hbm_active_v_id[0], 0);
    ASSERT_FLOAT_EQ(hbm_active_v_value[0], 0.0);
    ASSERT_EQ(rd_hbm_edge_valid[0], 0);
    ASSERT_EQ(iteration_end[0], 0);
    ASSERT_EQ(iteration_end_dvalid[0], 0);
    ASSERT_EQ(iteration_id[0], 0);
    // output once
    init_input_flag();
    run_module(1);
    // check
    ASSERT_EQ(debug_M4._v_id_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_value_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_loffset_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_roffset_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._now_loffset[0], CACHELINE_LEN);

    ASSERT_EQ(stage_full[0], 0);
    ASSERT_EQ(rd_hbm_edge_addr[0], 0);
    ASSERT_TRUE(rd_hbm_edge_mask[0].compare(BitVector_16(true)));
    ASSERT_EQ(hbm_active_v_id[0], 0);
    ASSERT_FLOAT_EQ(hbm_active_v_value[0], 1.0 / 46 * DAMPING);
    ASSERT_EQ(rd_hbm_edge_valid[0], 1);
    ASSERT_EQ(iteration_end[0], 0);
    ASSERT_EQ(iteration_end_dvalid[0], 0);
    ASSERT_EQ(iteration_id[0], 0);
    // output twice
    run_module(1);
    // check
    ASSERT_EQ(debug_M4._v_id_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_value_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_loffset_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_roffset_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._now_loffset[0], CACHELINE_LEN * 2);

    ASSERT_EQ(stage_full[0], 0);
    ASSERT_EQ(rd_hbm_edge_addr[0], 1);
    ASSERT_TRUE(rd_hbm_edge_mask[0].compare(BitVector_16(true)));
    ASSERT_EQ(hbm_active_v_id[0], 0);
    ASSERT_FLOAT_EQ(hbm_active_v_value[0], 1.0 / 46 * DAMPING);
    ASSERT_EQ(rd_hbm_edge_valid[0], 1);
    ASSERT_EQ(iteration_end[0], 0);
    ASSERT_EQ(iteration_end_dvalid[0], 0);
    ASSERT_EQ(iteration_id[0], 0);
    // output three times
    run_module(1);
    // check
    ASSERT_EQ(debug_M4._v_id_buffer[0].size(), size_t(0));
    ASSERT_EQ(debug_M4._v_value_buffer[0].size(), size_t(0));
    ASSERT_EQ(debug_M4._v_loffset_buffer[0].size(), size_t(0));
    ASSERT_EQ(debug_M4._v_roffset_buffer[0].size(), size_t(0));
    ASSERT_EQ(debug_M4._now_loffset[0], 1000000000);

    ASSERT_EQ(stage_full[0], 0);
    ASSERT_EQ(rd_hbm_edge_addr[0], 2);
    BitVector_16 bv(false);
    for (int i = 0; i < 46 % CACHELINE_LEN; i++) {
        bv.flag[i] = true;
    }
    ASSERT_TRUE(rd_hbm_edge_mask[0].compare(bv));
    ASSERT_EQ(hbm_active_v_id[0], 0);
    ASSERT_FLOAT_EQ(hbm_active_v_value[0], 1.0 / 46 * DAMPING);
    ASSERT_EQ(rd_hbm_edge_valid[0], 1);
    ASSERT_EQ(iteration_end[0], 0);
    ASSERT_EQ(iteration_end_dvalid[0], 0);
    ASSERT_EQ(iteration_id[0], 0);
}

int main(int argc,char* argv[])
{ 
    //testing::GTEST_FLAG(output) = "xml:"; //若要生成xml结果文件
    testing::InitGoogleTest(&argc,argv);
    RUN_ALL_TESTS();
    return 0;
}

#endif