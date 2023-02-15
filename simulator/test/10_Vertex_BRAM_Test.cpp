#include "gtest/gtest.h"
#include "Accelerator.h"
#include "debug.h"

#include <iostream>

using namespace std;

#if DEBUG

extern int rst_rd;
extern int ROOT_ID;
extern debug_Vertex_BRAM debug_M10;
extern int VERTEX_BRAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1];

class Vertex_BRAM_Test : public testing::Test {
public:
    // input signal
    int rd_addr_src[CORE_NUM] = {0};
    int rd_valid_src[CORE_NUM] = {0};
    int wr_push_flag_dst[CORE_NUM] = {0};
    int wr_addr_dst[CORE_NUM] = {0};
    int wr_v_value_dst[CORE_NUM] = {0};
    int wr_pull_first_flag_dst[CORE_NUM] = {0};
    int wr_valid_dst[CORE_NUM] = {0};
    int front_iteration_end[CORE_NUM] = {0};
    int front_iteration_end_dvalid[CORE_NUM] = {0};
    int front_global_iteration_id[CORE_NUM] = {0};
    // output signal
    int src_recv_update_v_value[CORE_NUM] = {0};
    int src_recv_update_v_dvalid[CORE_NUM] = {0};
    int backend_active_v_id[CORE_NUM] = {0};
    int backend_active_v_updated[CORE_NUM] = {0};
    int backend_active_v_pull_first_flag[CORE_NUM] = {0};
    int backend_active_v_dvalid[CORE_NUM] = {0};
    int iteration_end[CORE_NUM] = {0};
    int iteration_end_dvalid[CORE_NUM] = {0};

    static void SetUpTestCase() {
        for (int i = 0; i < CORE_NUM; i++) {
            for (int j = 0; j < MAX_VERTEX_NUM / CORE_NUM + 1; j++) {
                VERTEX_BRAM[i][j] = i + j;
            }
        }
    }
    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            rd_addr_src[i] = {0};
            rd_valid_src[i] = {0};
            wr_push_flag_dst[i] = {0};
            wr_addr_dst[i] = {0};
            wr_v_value_dst[i] = {0};
            wr_pull_first_flag_dst[i] = {0};
            wr_valid_dst[i] = {0};
            front_iteration_end[i] = {0};
            front_iteration_end_dvalid[i] = {0};
            front_global_iteration_id[i] = {0};
        }
    }
    void init_local_reg() {
        for (int i = 0; i < CORE_NUM; i++) {
            queue<int> empty1, empty2, empty3, empty4;
            swap(debug_M10._push_flag_buffer[i], empty1);
            swap(debug_M10._addr_buffer[i], empty2);
            swap(debug_M10._value_buffer[i], empty3);
            swap(debug_M10._pull_first_flag_buffer[i], empty4);
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
            Vertex_BRAM (
                rd_addr_src, rd_valid_src,
                wr_push_flag_dst, wr_addr_dst, wr_v_value_dst, wr_pull_first_flag_dst, wr_valid_dst,
                front_iteration_end, front_iteration_end_dvalid,
                front_global_iteration_id,

                src_recv_update_v_value, src_recv_update_v_dvalid,
                backend_active_v_id, backend_active_v_updated, backend_active_v_pull_first_flag, backend_active_v_dvalid,
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
TEST_F(Vertex_BRAM_Test, test_Pull_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._push_flag_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._pull_first_flag_buffer[i].size(), size_t(0));

        ASSERT_EQ(src_recv_update_v_value[i], 0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_pull_first_flag[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// test: normal mode
// given: rst=0, ROOT_ID, om1 input valid
// while: no control singal set to high
// then: store and output
TEST_F(Vertex_BRAM_Test, test_Pull_given_RdAddrValid_then_ReadData) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        rd_addr_src[i] = i;
        rd_valid_src[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._push_flag_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._addr_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._value_buffer[i].size(), size_t(1));
        ASSERT_EQ(debug_M10._pull_first_flag_buffer[i].size(), size_t(1));

        ASSERT_EQ(src_recv_update_v_value[i], 0);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 0);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_pull_first_flag[i], 0);
        ASSERT_EQ(backend_active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // run to output
    init_input_flag();
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M10._push_flag_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._addr_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._value_buffer[i].size(), size_t(0));
        ASSERT_EQ(debug_M10._pull_first_flag_buffer[i].size(), size_t(0));

        ASSERT_EQ(src_recv_update_v_value[i], VERTEX_BRAM[i][i / CORE_NUM]);
        ASSERT_EQ(src_recv_update_v_dvalid[i], 1);
        ASSERT_EQ(backend_active_v_id[i], 0);
        ASSERT_EQ(backend_active_v_updated[i], 0);
        ASSERT_EQ(backend_active_v_pull_first_flag[i], 0);
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