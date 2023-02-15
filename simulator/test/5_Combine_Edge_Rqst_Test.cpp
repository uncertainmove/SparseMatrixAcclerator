#include "gtest/gtest.h"
#include "Accelerator.h"
#include "debug.h"

#include <iostream>

using namespace std;

#if DEBUG

extern int rst_rd;
extern int ROOT_ID;
extern debug_Generate_HBM_Edge_Rqst debug_M5;

class Generate_HBM_Edge_Rqst_Test : public testing::Test {
public:
    // input signal
    int front_rd_hbm_edge_addr[CORE_NUM] = {0};
    BitVector_16 front_rd_hbm_edge_mask[CORE_NUM] = {false};
    int front_rd_hbm_edge_valid[CORE_NUM] = {0};
    int front_push_flag[CORE_NUM] = {0};
    int front_active_v_id[CORE_NUM] = {0};
    int front_active_v_value[CORE_NUM] = {0};
    int front_iteration_end[CORE_NUM] = {0};
    int front_iteration_end_dvalid[CORE_NUM] = {0};
    int hbm_full[CORE_NUM] = {0};
    int nextstage_full[CORE_NUM] = {0};
    // output signal
    int stage_full[CORE_NUM];
    int rd_hbm_edge_addr[CORE_NUM];
    int rd_hbm_edge_valid[CORE_NUM];
    int push_flag[CORE_NUM];
    int active_v_id[CORE_NUM];
    int active_v_value[CORE_NUM];
    int active_v_dvalid[CORE_NUM];
    int iteration_end[CORE_NUM];
    int iteration_end_dvalid[CORE_NUM];

    static void SetUpTestCase() {
    }
    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            front_rd_hbm_edge_addr[i] = {0};
            front_rd_hbm_edge_mask[i] = {false};
            front_rd_hbm_edge_valid[i] = {0};
            front_push_flag[i] = {0};
            front_active_v_id[i] = {0};
            front_active_v_value[i] = {0};
            front_iteration_end[i] = {0};
            front_iteration_end_dvalid[i] = {0};
            hbm_full[i] = {0};
            nextstage_full[i] = {0};
        }
    }
    void init_local_reg() {
        for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
            for (int j = 0; j < GROUP_CORE_NUM; j++) {
                queue<int> empty1, empty2, empty3, empty4;
                queue<BitVector_16> empty5;
                swap(debug_M5._push_flag_buffer[i][j], empty1);
                swap(debug_M5._v_id_buffer[i][j], empty2);
                swap(debug_M5._v_value_buffer[i][j], empty3);
                swap(debug_M5._edge_addr_buffer[i][j], empty4);
                swap(debug_M5._edge_mask_buffer[i][j], empty5);
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
            Generate_HBM_Edge_Rqst (
                front_rd_hbm_edge_addr, front_rd_hbm_edge_mask, front_rd_hbm_edge_valid,
                front_push_flag, front_active_v_id, front_active_v_value,
                front_iteration_end, front_iteration_end_dvalid,
                hbm_full, nextstage_full,

                stage_full,
                rd_hbm_edge_addr, rd_hbm_edge_valid,
                push_flag, active_v_id, active_v_value, active_v_dvalid,
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
TEST_F(Generate_HBM_Edge_Rqst_Test, test_Push_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        for (int j = 0; j < GROUP_CORE_NUM; j++) {
            ASSERT_EQ(debug_M5._push_flag_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M5._edge_addr_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M5._edge_mask_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M5._v_id_buffer[i][j].size(), size_t(0));
            ASSERT_EQ(debug_M5._v_value_buffer[i][j].size(), size_t(0));

            ASSERT_EQ(stage_full[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(push_flag[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(active_v_id[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(active_v_value[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(active_v_dvalid[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(iteration_end[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(iteration_end_dvalid[i * GROUP_CORE_NUM + j], 0);
        }
        ASSERT_EQ(rd_hbm_edge_addr[i], 0);
        ASSERT_EQ(rd_hbm_edge_valid[i], 0);
    }
}

// test: one hbm data for many vertex pipeline
// given: rst=1, ROOT_ID
// while: no control singal set to high
// then: local buffer empty
TEST_F(Generate_HBM_Edge_Rqst_Test, test_Push_given_SameAddr_then_UseSingleData) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        if (i % 2 == 1) {
            front_rd_hbm_edge_addr[i] = 1;
        }
        front_rd_hbm_edge_mask[i].flag[i % CACHELINE_LEN] = true;
        front_rd_hbm_edge_valid[i] = 1;
        front_push_flag[i] = 1;
        front_active_v_id[i] = i;
        front_active_v_value[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        for (int j = 0; j < GROUP_CORE_NUM; j++) {
            ASSERT_EQ(debug_M5._push_flag_buffer[i][j].size(), size_t(1));
            ASSERT_EQ(debug_M5._edge_addr_buffer[i][j].size(), size_t(1));
            ASSERT_EQ(debug_M5._edge_mask_buffer[i][j].size(), size_t(1));
            ASSERT_EQ(debug_M5._v_id_buffer[i][j].size(), size_t(1));
            ASSERT_EQ(debug_M5._v_value_buffer[i][j].size(), size_t(1));

            ASSERT_EQ(stage_full[i * GROUP_CORE_NUM + j], size_t(0));
            ASSERT_EQ(push_flag[i], size_t(0));
            ASSERT_EQ(active_v_id[i], size_t(0));
            ASSERT_EQ(active_v_value[i], size_t(0));
            ASSERT_EQ(active_v_dvalid[i], size_t(0));
            ASSERT_EQ(iteration_end[i], size_t(0));
            ASSERT_EQ(iteration_end_dvalid[i], size_t(0));
        }
        ASSERT_EQ(rd_hbm_edge_addr[i], size_t(0));
        ASSERT_EQ(rd_hbm_edge_valid[i], size_t(0));
    }
    // run twice
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        for (int j = 0; j < GROUP_CORE_NUM; j++) {
            if (j % 2 == 1) {
                ASSERT_EQ(debug_M5._push_flag_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._edge_addr_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._edge_mask_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._v_id_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._v_value_buffer[i][j].size(), size_t(1));

                ASSERT_EQ(stage_full[i * GROUP_CORE_NUM + j], 0);
                ASSERT_EQ(push_flag[i * GROUP_CORE_NUM + j], 1);
                ASSERT_EQ(active_v_id[i * GROUP_CORE_NUM + j], -1);
                ASSERT_EQ(active_v_value[i * GROUP_CORE_NUM + j], 1);
                ASSERT_EQ(active_v_dvalid[i * GROUP_CORE_NUM + j], 1);
                ASSERT_EQ(iteration_end[i * GROUP_CORE_NUM + j], 0);
                ASSERT_EQ(iteration_end_dvalid[i * GROUP_CORE_NUM + j], 0);
            } else {
                ASSERT_EQ(debug_M5._push_flag_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._edge_addr_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._edge_mask_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._v_id_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._v_value_buffer[i][j].size(), size_t(0));

                ASSERT_EQ(stage_full[i * GROUP_CORE_NUM + j], 0);
                ASSERT_EQ(push_flag[i * GROUP_CORE_NUM + j], 1);
                ASSERT_EQ(active_v_id[i * GROUP_CORE_NUM + j], i * GROUP_CORE_NUM + j);
                ASSERT_EQ(active_v_value[i * GROUP_CORE_NUM + j], 1);
                ASSERT_EQ(active_v_dvalid[i * GROUP_CORE_NUM + j], 1);
                ASSERT_EQ(iteration_end[i * GROUP_CORE_NUM + j], 0);
                ASSERT_EQ(iteration_end_dvalid[i * GROUP_CORE_NUM + j], 0);
            }
        }
        ASSERT_EQ(rd_hbm_edge_addr[i], 0);
        ASSERT_EQ(rd_hbm_edge_valid[i], 1);
    }
}

// test: normal mode
// given: rst=0, ROOT_ID, front addr valid
// while: no control singal set to high
// then: store and output
TEST_F(Generate_HBM_Edge_Rqst_Test, test_Push_given_AddrValid_then_OutputAddr) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        front_rd_hbm_edge_addr[i] = i;
        for (int j = 0; j <= i % CACHELINE_LEN; j++) {
            front_rd_hbm_edge_mask[i].flag[j] = true;
        }
        front_rd_hbm_edge_valid[i] = 1;
        front_push_flag[i] = 1;
        front_active_v_id[i] = i;
        front_active_v_value[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        for (int j = 0; j < GROUP_CORE_NUM; j++) {
            ASSERT_EQ(debug_M5._push_flag_buffer[i][j].size(), size_t(1));
            ASSERT_EQ(debug_M5._edge_addr_buffer[i][j].size(), size_t(1));
            ASSERT_EQ(debug_M5._edge_mask_buffer[i][j].size(), size_t(1));
            ASSERT_EQ(debug_M5._v_id_buffer[i][j].size(), size_t(1));
            ASSERT_EQ(debug_M5._v_value_buffer[i][j].size(), size_t(1));

            ASSERT_EQ(stage_full[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(push_flag[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(active_v_id[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(active_v_value[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(active_v_dvalid[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(iteration_end[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(iteration_end_dvalid[i * GROUP_CORE_NUM + j], 0);
        }
        ASSERT_EQ(rd_hbm_edge_addr[i], 0);
        ASSERT_EQ(rd_hbm_edge_valid[i], 0);
    }
    // run twice
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        for (int j = 0; j < GROUP_CORE_NUM; j++) {
            if (j == 0) {
                ASSERT_EQ(debug_M5._push_flag_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._edge_addr_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._edge_mask_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._v_id_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._v_value_buffer[i][j].size(), size_t(0));
            } else {
                ASSERT_EQ(debug_M5._push_flag_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._edge_addr_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._edge_mask_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._v_id_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._v_value_buffer[i][j].size(), size_t(1));
            }

            ASSERT_EQ(stage_full[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(push_flag[i], 1);
            if (j == 0) {
                ASSERT_EQ(active_v_id[i * GROUP_CORE_NUM + j], i * GROUP_CORE_NUM);
            } else {
                ASSERT_EQ(active_v_id[i * GROUP_CORE_NUM + j], -1);
            }
            ASSERT_EQ(active_v_value[i * GROUP_CORE_NUM + j], 1);
            ASSERT_EQ(active_v_dvalid[i * GROUP_CORE_NUM + j], 1);
            ASSERT_EQ(iteration_end[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(iteration_end_dvalid[i * GROUP_CORE_NUM + j], 0);
        }
        ASSERT_EQ(rd_hbm_edge_addr[i], i * GROUP_CORE_NUM);
        ASSERT_EQ(rd_hbm_edge_valid[i], 1);
    }
    // run three times
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        for (int j = 0; j < GROUP_CORE_NUM; j++) {
            if (j == 0 || j == 1) {
                ASSERT_EQ(debug_M5._push_flag_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._edge_addr_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._edge_mask_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._v_id_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._v_value_buffer[i][j].size(), size_t(0));
            } else {
                ASSERT_EQ(debug_M5._push_flag_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._edge_addr_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._edge_mask_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._v_id_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._v_value_buffer[i][j].size(), size_t(1));
            }

            ASSERT_EQ(stage_full[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(push_flag[i], 1);
            if (j == 0 || j == 1) {
                ASSERT_EQ(active_v_id[i * GROUP_CORE_NUM + j], i * GROUP_CORE_NUM + 1);
            } else {
                ASSERT_EQ(active_v_id[i * GROUP_CORE_NUM + j], -1);
            }
            ASSERT_EQ(active_v_value[i * GROUP_CORE_NUM + j], 1);
            ASSERT_EQ(active_v_dvalid[i * GROUP_CORE_NUM + j], 1);
            ASSERT_EQ(iteration_end[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(iteration_end_dvalid[i * GROUP_CORE_NUM + j], 0);
        }
        ASSERT_EQ(rd_hbm_edge_addr[i], i * GROUP_CORE_NUM + 1);
        ASSERT_EQ(rd_hbm_edge_valid[i], 1);
    }
    // run four times
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        for (int j = 0; j < GROUP_CORE_NUM; j++) {
            if (j == 0 || j == 1 || j == 2) {
                ASSERT_EQ(debug_M5._push_flag_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._edge_addr_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._edge_mask_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._v_id_buffer[i][j].size(), size_t(0));
                ASSERT_EQ(debug_M5._v_value_buffer[i][j].size(), size_t(0));
            } else {
                ASSERT_EQ(debug_M5._push_flag_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._edge_addr_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._edge_mask_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._v_id_buffer[i][j].size(), size_t(1));
                ASSERT_EQ(debug_M5._v_value_buffer[i][j].size(), size_t(1));
            }

            ASSERT_EQ(stage_full[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(push_flag[i], 1);
            if (j == 0 || j == 1 || j == 2) {
                ASSERT_EQ(active_v_id[i * GROUP_CORE_NUM + j], i * GROUP_CORE_NUM + 2);
            } else {
                ASSERT_EQ(active_v_id[i * GROUP_CORE_NUM + j], -1);
            }
            ASSERT_EQ(active_v_value[i * GROUP_CORE_NUM + j], 1);
            ASSERT_EQ(active_v_dvalid[i * GROUP_CORE_NUM + j], 1);
            ASSERT_EQ(iteration_end[i * GROUP_CORE_NUM + j], 0);
            ASSERT_EQ(iteration_end_dvalid[i * GROUP_CORE_NUM + j], 0);
        }
        ASSERT_EQ(rd_hbm_edge_addr[i], i * GROUP_CORE_NUM + 2);
        ASSERT_EQ(rd_hbm_edge_valid[i], 1);
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