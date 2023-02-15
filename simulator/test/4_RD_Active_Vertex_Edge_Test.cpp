#include "gtest/gtest.h"
#include "Accelerator.h"
#include "debug.h"

#include <iostream>

using namespace std;

#if DEBUG

extern int rst_rd;
extern int ROOT_ID;
extern debug_RD_Active_Vertex_Edge debug_M4;

class RD_Active_Vertex_Edge_Test : public testing::Test {
public:
    // input signal
    int front_push_flag[CORE_NUM] = {0};
    int front_active_v_id[CORE_NUM] = {0};
    int front_active_v_value[CORE_NUM] = {0};
    int front_active_v_pull_first_flag[CORE_NUM] = {0};
    int front_active_v_dvalid[CORE_NUM] = {0};
    int front_iteration_end[CORE_NUM] = {0};
    int front_iteration_end_dvalid[CORE_NUM] = {0};
    int front_active_v_loffset[CORE_NUM] = {0};
    int front_active_v_roffset[CORE_NUM] = {0};
    int front_offset_dvalid[CORE_NUM] = {0};
    int combinestage_full[CORE_NUM] = {0};
    int bramstage_full[CORE_NUM] = {0};
    // output signal
    int stage_full[CORE_NUM];
    int rd_hbm_edge_addr[CORE_NUM];
    BitVector_16 rd_hbm_edge_mask[CORE_NUM];
    int hbm_push_flag[CORE_NUM];
    int hbm_active_v_id[CORE_NUM];
    int hbm_active_v_value[CORE_NUM];
    int rd_hbm_edge_valid[CORE_NUM];
    int rd_bram_edge_addr[CORE_NUM];
    int bram_push_flag[CORE_NUM];
    int bram_active_v_id[CORE_NUM];
    int bram_active_v_value[CORE_NUM];
    int rd_bram_edge_valid[CORE_NUM];
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
            front_active_v_pull_first_flag[i] = {0};
            front_active_v_dvalid[i] = {0};
            front_iteration_end[i] = {0};
            front_iteration_end_dvalid[i] = {0};
            front_active_v_loffset[i] = {0};
            front_active_v_roffset[i] = {0};
            front_offset_dvalid[i] = {0};
            combinestage_full[i] = {0};
            bramstage_full[i] = {0};
        }
    }
    void init_local_reg() {
        for (int i = 0; i < CORE_NUM; i++) {
            queue<int> empty1, empty2, empty3, empty4, empty5, empty6;
            swap(debug_M4._push_flag_buffer[i], empty1);
            swap(debug_M4._v_id_buffer[i], empty2);
            swap(debug_M4._v_value_buffer[i], empty3);
            swap(debug_M4._v_loffset_buffer[i], empty4);
            swap(debug_M4._v_roffset_buffer[i], empty5);
            swap(debug_M4._pull_first_flag_buffer[i], empty6);

            debug_M4._now_loffest = 0;
            // debug_M4._init_flag = 0;
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
            RD_Active_Vertex_Edge (
                front_push_flag, front_active_v_id, front_active_v_value, front_active_v_pull_first_flag, front_active_v_dvalid,
                front_iteration_end, front_iteration_end_dvalid,
                front_active_v_loffset, front_active_v_roffset, front_offset_dvalid,
                combinestage_full, bramstage_full,

                stage_full,
                rd_hbm_edge_addr, rd_hbm_edge_mask, hbm_push_flag, hbm_active_v_id, hbm_active_v_value, rd_hbm_edge_valid,
                rd_bram_edge_addr, bram_push_flag, bram_active_v_id, bram_active_v_value, rd_bram_edge_valid,
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
TEST_F(RD_Active_Vertex_Edge_Test, test_PushPull_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M4._push_flag_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M4._pull_first_flag_buffer[i].size(), size_t(0));

        ASSERT_EQ(debug_M4._now_loffest[i], 1000000000);
        // ASSERT_EQ(debug_M4._init_flag[i], 1);

        ASSERT_EQ(stage_full[i], 0);
        ASSERT_EQ(rd_hbm_edge_addr[i], 0);
        ASSERT_TRUE(rd_hbm_edge_mask[i].compare(BitVector_16(false)));
        ASSERT_EQ(hbm_push_flag[i], 0);
        ASSERT_EQ(hbm_active_v_id[i], 0);
        ASSERT_EQ(hbm_active_v_value[i], 0);
        ASSERT_EQ(rd_hbm_edge_valid[i], 0);
        ASSERT_EQ(rd_bram_edge_addr[i], 0);
        ASSERT_EQ(bram_push_flag[i], 0);
        ASSERT_EQ(bram_active_v_id[i], 0);
        ASSERT_EQ(bram_active_v_value[i], 0);
        ASSERT_EQ(rd_bram_edge_valid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// test: write & read
// given: rst=0, ROOT_ID, front addr is valid
// while: no control singal set to high
// then: normal input and output
TEST_F(RD_Active_Vertex_Edge_Test, test_Push_given_InputActiveId_then_OutputId) {
    // reset
    reset_module();
    // run once to write active id
    init_input_flag();
    int sum = 0;
    for (int i = 0; i < CORE_NUM; i++) {
        int front_sum = sum;
        sum += i;
        if (i % 2 == 0) {
            front_push_flag[i] = 1;
            front_active_v_id[i] = i;
            front_active_v_value[i] = 1;
            front_active_v_dvalid[i] = 1;
            front_active_v_loffset[i] = front_sum;
            front_active_v_roffset[i] = sum;
            front_offset_dvalid[i] = 1;
        }
    }
    run_module(1);
    // check
    int sum_check = 0;
    for (int i = 0; i < CORE_NUM; i++) {
        if (i % 2 == 0) {
            ASSERT_EQ(debug_M4._push_flag_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._pull_first_flag_buffer[i].size(), size_t(1));

            ASSERT_EQ(debug_M4._now_loffest[i], 1000000000);
            // ASSERT_EQ(debug_M4._init_flag[i], 1);

            ASSERT_EQ(stage_full[i], 0);
            ASSERT_EQ(rd_hbm_edge_addr[i], 0);
            ASSERT_TRUE(rd_hbm_edge_mask[i].compare(BitVector_16(false)));
            ASSERT_EQ(hbm_push_flag[i], 0);
            ASSERT_EQ(hbm_active_v_id[i], 0);
            ASSERT_EQ(hbm_active_v_value[i], 0);
            ASSERT_EQ(rd_hbm_edge_valid[i], 0);
            ASSERT_EQ(rd_bram_edge_addr[i], 0);
            ASSERT_EQ(bram_push_flag[i], 0);
            ASSERT_EQ(bram_active_v_id[i], 0);
            ASSERT_EQ(bram_active_v_value[i], 0);
            ASSERT_EQ(rd_bram_edge_valid[i], 0);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
        } else {
            ASSERT_EQ(debug_M4._push_flag_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._pull_first_flag_buffer[i].size(), size_t(0));

            ASSERT_EQ(debug_M4._now_loffest[i], 1000000000);
            // ASSERT_EQ(debug_M4._init_flag[i], 1);
            ASSERT_EQ(stage_full[i], 0);
            ASSERT_EQ(rd_hbm_edge_addr[i], 0);
            ASSERT_TRUE(rd_hbm_edge_mask[i].compare(BitVector_16()));
            ASSERT_EQ(hbm_push_flag[i], 0);
            ASSERT_EQ(hbm_active_v_id[i], 0);
            ASSERT_EQ(hbm_active_v_value[i], 0);
            ASSERT_EQ(rd_hbm_edge_valid[i], 0);
            ASSERT_EQ(rd_bram_edge_addr[i], 0);
            ASSERT_EQ(bram_push_flag[i], 0);
            ASSERT_EQ(bram_active_v_id[i], 0);
            ASSERT_EQ(bram_active_v_value[i], 0);
            ASSERT_EQ(rd_bram_edge_valid[i], 0);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
        }
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
            if (front_sum / CACHELINE_LEN == sum_check / CACHELINE_LEN) {
                ASSERT_EQ(debug_M4._push_flag_buffer[i].size(), size_t(0));
                ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(0));
                ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(0));
                ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(0));
                ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(0));
                ASSERT_EQ(debug_M4._pull_first_flag_buffer[i].size(), size_t(0));

                ASSERT_EQ(debug_M4._now_loffest[i], 1000000000);
                // ASSERT_EQ(debug_M4._init_flag[i], 1);
                ASSERT_EQ(stage_full[i], 0);
                ASSERT_EQ(rd_hbm_edge_addr[i], front_sum / CACHELINE_LEN);
                BitVector_16 bv(false);
                for (int j = front_sum % CACHELINE_LEN; j < sum_check % CACHELINE_LEN; j++) {
                    bv.flag[j] = true;
                }
                // rd_hbm_edge_mask[i].show();
                // bv.show();
                ASSERT_TRUE(rd_hbm_edge_mask[i].compare(bv));
                ASSERT_EQ(hbm_push_flag[i], 1);
                ASSERT_EQ(hbm_active_v_id[i], i);
                ASSERT_EQ(hbm_active_v_value[i], 1);
                ASSERT_EQ(rd_hbm_edge_valid[i], !(front_sum == sum_check));
                ASSERT_EQ(rd_bram_edge_addr[i], 0);
                ASSERT_EQ(bram_push_flag[i], 0);
                ASSERT_EQ(bram_active_v_id[i], 0);
                ASSERT_EQ(bram_active_v_value[i], 0);
                ASSERT_EQ(rd_bram_edge_valid[i], 0);
                ASSERT_EQ(iteration_end[i], 0);
                ASSERT_EQ(iteration_end_dvalid[i], 0);
            } else {
                // printf("check core_id=%d, la=%d, ra=%d\n", i, front_sum, sum_check);
                ASSERT_EQ(debug_M4._push_flag_buffer[i].size(), size_t(1));
                ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(1));
                ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(1));
                ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(1));
                ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(1));
                ASSERT_EQ(debug_M4._pull_first_flag_buffer[i].size(), size_t(1));

                ASSERT_EQ(debug_M4._now_loffest[i], (front_sum / CACHELINE_LEN + 1) * CACHELINE_LEN);
                // ASSERT_EQ(debug_M4._init_flag[i], 0);

                ASSERT_EQ(stage_full[i], 0);
                ASSERT_EQ(rd_hbm_edge_addr[i], front_sum / CACHELINE_LEN);
                BitVector_16 bv(false);
                for (int j = front_sum % CACHELINE_LEN; j < CACHELINE_LEN; j++) {
                    bv.flag[j] = true;
                }
                // rd_hbm_edge_mask[i].show();
                // bv.show();
                ASSERT_TRUE(rd_hbm_edge_mask[i].compare(bv));
                ASSERT_EQ(hbm_push_flag[i], 1);
                ASSERT_EQ(hbm_active_v_id[i], i);
                ASSERT_EQ(hbm_active_v_value[i], 1);
                ASSERT_EQ(rd_hbm_edge_valid[i], !(front_sum == sum_check));
                ASSERT_EQ(rd_bram_edge_addr[i], 0);
                ASSERT_EQ(bram_push_flag[i], 0);
                ASSERT_EQ(bram_active_v_id[i], 0);
                ASSERT_EQ(bram_active_v_value[i], 0);
                ASSERT_EQ(rd_bram_edge_valid[i], 0);
                ASSERT_EQ(iteration_end[i], 0);
                ASSERT_EQ(iteration_end_dvalid[i], 0);
            }
        } else {
            ASSERT_EQ(debug_M4._push_flag_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._pull_first_flag_buffer[i].size(), size_t(0));

            ASSERT_EQ(debug_M4._now_loffest[i], 1000000000);
            // ASSERT_EQ(debug_M4._init_flag[i], 1);
            ASSERT_EQ(stage_full[i], 0);
            ASSERT_EQ(rd_hbm_edge_addr[i], 0);
            ASSERT_TRUE(rd_hbm_edge_mask[i].compare(BitVector_16()));
            ASSERT_EQ(hbm_push_flag[i], 0);
            ASSERT_EQ(hbm_active_v_id[i], 0);
            ASSERT_EQ(hbm_active_v_value[i], 0);
            ASSERT_EQ(rd_hbm_edge_valid[i], 0);
            ASSERT_EQ(rd_bram_edge_addr[i], 0);
            ASSERT_EQ(bram_push_flag[i], 0);
            ASSERT_EQ(bram_active_v_id[i], 0);
            ASSERT_EQ(bram_active_v_value[i], 0);
            ASSERT_EQ(rd_bram_edge_valid[i], 0);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
        }
    }
}

// test: local offset logic
// given: rst=0, ROOT_ID, now_loffset=16
// while: no control singal set to high
// then: output and now_loffset increase
TEST_F(RD_Active_Vertex_Edge_Test, test_Push_given_OutputSignal_while_LocalOffsetValid_then_LocalOffsetIncrease) {
    // reset
    reset_module();
    // run once to write active id
    init_input_flag();
    front_push_flag[0] = 1;
    front_active_v_id[0] = 0;
    front_active_v_value[0] = 1;
    front_active_v_dvalid[0] = 1;
    front_active_v_loffset[0] = 0;
    front_active_v_roffset[0] = 46;
    front_offset_dvalid[0] = 1;
    run_module(1);
    // check
    ASSERT_EQ(debug_M4._push_flag_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_id_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_value_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_loffset_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_roffset_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._pull_first_flag_buffer[0].size(), size_t(1));

    ASSERT_EQ(debug_M4._now_loffest[0], 1000000000);
    // ASSERT_EQ(debug_M4._init_flag[i], 1);

    ASSERT_EQ(stage_full[0], 0);
    ASSERT_EQ(rd_hbm_edge_addr[0], 0);
    ASSERT_TRUE(rd_hbm_edge_mask[0].compare(BitVector_16(false)));
    ASSERT_EQ(hbm_push_flag[0], 0);
    ASSERT_EQ(hbm_active_v_id[0], 0);
    ASSERT_EQ(hbm_active_v_value[0], 0);
    ASSERT_EQ(rd_hbm_edge_valid[0], 0);
    ASSERT_EQ(rd_bram_edge_addr[0], 0);
    ASSERT_EQ(bram_push_flag[0], 0);
    ASSERT_EQ(bram_active_v_id[0], 0);
    ASSERT_EQ(bram_active_v_value[0], 0);
    ASSERT_EQ(rd_bram_edge_valid[0], 0);
    ASSERT_EQ(iteration_end[0], 0);
    ASSERT_EQ(iteration_end_dvalid[0], 0);
    // output once
    init_input_flag();
    run_module(1);
    // check
    ASSERT_EQ(debug_M4._push_flag_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_id_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_value_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_loffset_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_roffset_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._pull_first_flag_buffer[0].size(), size_t(1));

    ASSERT_EQ(debug_M4._now_loffest[0], CACHELINE_LEN);
    // ASSERT_EQ(debug_M4._init_flag[i], 1);

    ASSERT_EQ(stage_full[0], 0);
    ASSERT_EQ(rd_hbm_edge_addr[0], 0);
    ASSERT_TRUE(rd_hbm_edge_mask[0].compare(BitVector_16(true)));
    ASSERT_EQ(hbm_push_flag[0], 1);
    ASSERT_EQ(hbm_active_v_id[0], 0);
    ASSERT_EQ(hbm_active_v_value[0], 1);
    ASSERT_EQ(rd_hbm_edge_valid[0], 1);
    ASSERT_EQ(rd_bram_edge_addr[0], 0);
    ASSERT_EQ(bram_push_flag[0], 0);
    ASSERT_EQ(bram_active_v_id[0], 0);
    ASSERT_EQ(bram_active_v_value[0], 0);
    ASSERT_EQ(rd_bram_edge_valid[0], 0);
    ASSERT_EQ(iteration_end[0], 0);
    ASSERT_EQ(iteration_end_dvalid[0], 0);
    // output twice
    run_module(1);
    // check
    ASSERT_EQ(debug_M4._push_flag_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_id_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_value_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_loffset_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._v_roffset_buffer[0].size(), size_t(1));
    ASSERT_EQ(debug_M4._pull_first_flag_buffer[0].size(), size_t(1));

    ASSERT_EQ(debug_M4._now_loffest[0], CACHELINE_LEN * 2);
    // ASSERT_EQ(debug_M4._init_flag[i], 1);

    ASSERT_EQ(stage_full[0], 0);
    ASSERT_EQ(rd_hbm_edge_addr[0], 1);
    ASSERT_TRUE(rd_hbm_edge_mask[0].compare(BitVector_16(true)));
    ASSERT_EQ(hbm_push_flag[0], 1);
    ASSERT_EQ(hbm_active_v_id[0], 0);
    ASSERT_EQ(hbm_active_v_value[0], 1);
    ASSERT_EQ(rd_hbm_edge_valid[0], 1);
    ASSERT_EQ(rd_bram_edge_addr[0], 0);
    ASSERT_EQ(bram_push_flag[0], 0);
    ASSERT_EQ(bram_active_v_id[0], 0);
    ASSERT_EQ(bram_active_v_value[0], 0);
    ASSERT_EQ(rd_bram_edge_valid[0], 0);
    ASSERT_EQ(iteration_end[0], 0);
    ASSERT_EQ(iteration_end_dvalid[0], 0);
    // output three times
    run_module(1);
    // check
    ASSERT_EQ(debug_M4._push_flag_buffer[0].size(), size_t(0));
    ASSERT_EQ(debug_M4._v_id_buffer[0].size(), size_t(0));
    ASSERT_EQ(debug_M4._v_value_buffer[0].size(), size_t(0));
    ASSERT_EQ(debug_M4._v_loffset_buffer[0].size(), size_t(0));
    ASSERT_EQ(debug_M4._v_roffset_buffer[0].size(), size_t(0));
    ASSERT_EQ(debug_M4._pull_first_flag_buffer[0].size(), size_t(0));

    ASSERT_EQ(debug_M4._now_loffest[0], 1000000000);
    // ASSERT_EQ(debug_M4._init_flag[i], 1);

    ASSERT_EQ(stage_full[0], 0);
    ASSERT_EQ(rd_hbm_edge_addr[0], 2);
    BitVector_16 bv(false);
    for (int i = 0; i < 46 % CACHELINE_LEN; i++) {
        bv.flag[i] = true;
    }
    ASSERT_TRUE(rd_hbm_edge_mask[0].compare(bv));
    ASSERT_EQ(hbm_push_flag[0], 1);
    ASSERT_EQ(hbm_active_v_id[0], 0);
    ASSERT_EQ(hbm_active_v_value[0], 1);
    ASSERT_EQ(rd_hbm_edge_valid[0], 1);
    ASSERT_EQ(rd_bram_edge_addr[0], 0);
    ASSERT_EQ(bram_push_flag[0], 0);
    ASSERT_EQ(bram_active_v_id[0], 0);
    ASSERT_EQ(bram_active_v_value[0], 0);
    ASSERT_EQ(rd_bram_edge_valid[0], 0);
    ASSERT_EQ(iteration_end[0], 0);
    ASSERT_EQ(iteration_end_dvalid[0], 0);
}

// given: rst=0, ROOT_ID
// while: no control singal set to high
// then: valid input
TEST_F(RD_Active_Vertex_Edge_Test, test_PullFirstEdge_given_InputActiveId_then_OutputId) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        if (i % 2 == 0) {
            front_push_flag[i] = 0;
            front_active_v_id[i] = i;
            front_active_v_value[i] = 1;
            front_active_v_pull_first_flag[i] = 1;
            front_active_v_dvalid[i] = 1;
            front_active_v_loffset[i] = 0;
            front_active_v_roffset[i] = i;
            front_offset_dvalid[i] = 1;
        }
    }
    run_module(1);
    // check local register
    for (int i = 0; i < CORE_NUM; i++) {
        if (i % 2 == 0) {
            ASSERT_EQ(debug_M4._push_flag_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._pull_first_flag_buffer[i].size(), size_t(1));

            ASSERT_EQ(debug_M4._now_loffest[i], 1000000000);
            // ASSERT_EQ(debug_M4._init_flag[i], 1);

            ASSERT_EQ(stage_full[i], 0);
            ASSERT_EQ(rd_hbm_edge_addr[i], 0);
            ASSERT_TRUE(rd_hbm_edge_mask[i].compare(BitVector_16(false)));
            ASSERT_EQ(hbm_push_flag[i], 0);
            ASSERT_EQ(hbm_active_v_id[i], 0);
            ASSERT_EQ(hbm_active_v_value[i], 0);
            ASSERT_EQ(rd_hbm_edge_valid[i], 0);
            ASSERT_EQ(rd_bram_edge_addr[i], 0);
            ASSERT_EQ(bram_push_flag[i], 0);
            ASSERT_EQ(bram_active_v_id[i], 0);
            ASSERT_EQ(bram_active_v_value[i], 0);
            ASSERT_EQ(rd_bram_edge_valid[i], 0);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
        } else {
            ASSERT_EQ(debug_M4._push_flag_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._pull_first_flag_buffer[i].size(), size_t(0));

            ASSERT_EQ(debug_M4._now_loffest[i], 1000000000);
            // ASSERT_EQ(debug_M4._init_flag[i], 1);

            ASSERT_EQ(stage_full[i], 0);
            ASSERT_EQ(rd_hbm_edge_addr[i], 0);
            ASSERT_TRUE(rd_hbm_edge_mask[i].compare(BitVector_16(false)));
            ASSERT_EQ(hbm_push_flag[i], 0);
            ASSERT_EQ(hbm_active_v_id[i], 0);
            ASSERT_EQ(hbm_active_v_value[i], 0);
            ASSERT_EQ(rd_hbm_edge_valid[i], 0);
            ASSERT_EQ(rd_bram_edge_addr[i], 0);
            ASSERT_EQ(bram_push_flag[i], 0);
            ASSERT_EQ(bram_active_v_id[i], 0);
            ASSERT_EQ(bram_active_v_value[i], 0);
            ASSERT_EQ(rd_bram_edge_valid[i], 0);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
        }
    }
    run_module(1);
    // check output
    for (int i = 0; i < CORE_NUM; i++) {
        if (i % 2 == 0) {
            ASSERT_EQ(debug_M4._push_flag_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(1));
            ASSERT_EQ(debug_M4._pull_first_flag_buffer[i].size(), size_t(1));

            ASSERT_EQ(debug_M4._now_loffest[i], 1000000000);
            // ASSERT_EQ(debug_M4._init_flag[i], 1);

            ASSERT_EQ(stage_full[i], 0);
            ASSERT_EQ(rd_hbm_edge_addr[i], 0);
            ASSERT_TRUE(rd_hbm_edge_mask[i].compare(BitVector_16(false)));
            ASSERT_EQ(hbm_push_flag[i], 0);
            ASSERT_EQ(hbm_active_v_id[i], 0);
            ASSERT_EQ(hbm_active_v_value[i], 0);
            ASSERT_EQ(rd_hbm_edge_valid[i], 0);
            ASSERT_EQ(rd_bram_edge_addr[i], i / CORE_NUM);
            ASSERT_EQ(bram_push_flag[i], 0);
            ASSERT_EQ(bram_active_v_id[i], i);
            ASSERT_EQ(bram_active_v_value[i], 1);
            ASSERT_EQ(rd_bram_edge_valid[i], 1);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
        } else {
            ASSERT_EQ(debug_M4._push_flag_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_id_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_value_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_loffset_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._v_roffset_buffer[i].size(), size_t(0));
            ASSERT_EQ(debug_M4._pull_first_flag_buffer[i].size(), size_t(0));

            ASSERT_EQ(debug_M4._now_loffest[i], 1000000000);
            // ASSERT_EQ(debug_M4._init_flag[i], 1);

            ASSERT_EQ(stage_full[i], 0);
            ASSERT_EQ(rd_hbm_edge_addr[i], 0);
            ASSERT_TRUE(rd_hbm_edge_mask[i].compare(BitVector_16(false)));
            ASSERT_EQ(hbm_push_flag[i], 0);
            ASSERT_EQ(hbm_active_v_id[i], 0);
            ASSERT_EQ(hbm_active_v_value[i], 0);
            ASSERT_EQ(rd_hbm_edge_valid[i], 0);
            ASSERT_EQ(rd_bram_edge_addr[i], 0);
            ASSERT_EQ(bram_push_flag[i], 0);
            ASSERT_EQ(bram_active_v_id[i], 0);
            ASSERT_EQ(bram_active_v_value[i], 0);
            ASSERT_EQ(rd_bram_edge_valid[i], 0);
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