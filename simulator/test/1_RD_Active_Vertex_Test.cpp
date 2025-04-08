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
extern int Csr_Offset[MAX_VERTEX_NUM + 1];
extern debug_RD_ACTIVE_VERTEX debug_M1;

class RD_ACTIVE_VERTEX_TEST : public testing::Test {
public:
    // input signal
    int backend_active_v_id[CORE_NUM] = {0};
    int backend_active_v_updated[CORE_NUM] = {0};
    int backend_active_v_dvalid[CORE_NUM] = {0};
    int backend_iteration_end[CORE_NUM] = {0};
    int backend_iteration_end_dvalid[CORE_NUM] = {0};
    int nextstage_full[CORE_NUM] = {0};
    // output signal
    int active_v_id[CORE_NUM];
    int active_v_dvalid[CORE_NUM];
    int iteration_end[CORE_NUM];
    int iteration_end_dvalid[CORE_NUM];
    int iteration_id[CORE_NUM];

    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            backend_active_v_id[i] = 0;
            backend_active_v_updated[i] = 0;
            backend_active_v_dvalid[i] = 0;
            backend_iteration_end[i] = 0;
            backend_iteration_end_dvalid[i] = 0;
            nextstage_full[i] = 0;
        }
    }
    void init_local_reg() {
        for (int i = 0; i < CORE_NUM; i++) {
            debug_M1._iteration_id[i] = 0;
            debug_M1._init_bitmap_id[i] = 0;
            debug_M1._iteration_end[i] = 0;
            for (int j = 0; j < BitMap_Compressed_NUM + 1; j++) {
                debug_M1._vis_bitmap[i][0][j].set(false);
                debug_M1._vis_bitmap[i][1][j].set(false);
            }
            debug_M1._vis_bitmap_index[i] = 0;
            debug_M1._vis_bitmap_now[i] = 0;
        }
    }
    void reset_module() {
        rst_rd = 1;
        init_input_flag();
        run_module(1); // run module once to bind debug port
        init_local_reg();
        run_module(10);
        rst_rd = 0;
    }
    void run_module(int cycle) {
        for (int i = 0; i < cycle; i++) {
            RD_ACTIVE_VERTEX(
                backend_active_v_id, backend_active_v_updated, backend_active_v_dvalid,
                backend_iteration_end, backend_iteration_end_dvalid,
                nextstage_full,

                active_v_id, active_v_dvalid,
                iteration_end, iteration_end_dvalid,
                iteration_id
            );
        }
    }
    virtual void TearDown() {
        reset_module();
    }
};

// given: rst=0
// while: bitmap is not initial
// then: bitmap[ALL_ID]=true means all vertex need traverse
TEST_F(RD_ACTIVE_VERTEX_TEST, test_given_FalseRst_then_BitmapInitial) {
    reset_module();
    // bitmap will init in 64 cycles
    init_input_flag();
    run_module(BitMap_Compressed_NUM);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal signal
        ASSERT_EQ(debug_M1._iteration_id[i], 0);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
        ASSERT_EQ(debug_M1._iteration_end[i], 0);
        ASSERT_EQ(debug_M1._vis_bitmap_index[i], 0);
        ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(bitmap(true)));
        for (int j = 0; j < BitMap_Compressed_NUM; j++) {
            ASSERT_TRUE(debug_M1._vis_bitmap[i][0][j].compare(bitmap(true)));
            ASSERT_TRUE(debug_M1._vis_bitmap[i][1][j].compare(bitmap(true)));
        }
        // check output signal
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
}

// given: rst=0, bitmap has init
// while: not control signal set to high
// then: output id
TEST_F(RD_ACTIVE_VERTEX_TEST, test_given_InitBitMap_then_OutputNormal) {
    reset_module();
    // bitmap will init in 64 cycles
    init_input_flag();
    run_module(BitMap_Compressed_NUM);
    // run one cycle to output
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal signal
        ASSERT_EQ(debug_M1._iteration_id[i], 0);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
        ASSERT_EQ(debug_M1._iteration_end[i], 0);
        ASSERT_EQ(debug_M1._vis_bitmap_index[i], 0);
        bitmap temp(true);
        temp.v[i / CORE_NUM % BitMap_Compressed_Length] = false;
        // debug_M1._vis_bitmap_now[i].show();
        ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(temp));
        // check output signal
        ASSERT_EQ(active_v_id[i], i);
        ASSERT_EQ(active_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
}

// given: rst=0, bitmap has init
// while: not control signal set to high
// then: vis_bitmap_index changed
TEST_F(RD_ACTIVE_VERTEX_TEST, test_given_InitBitMap_then_VisIndexChanged) {
    reset_module();
    // bitmap will init in 64 cycles
    init_input_flag();
    run_module(BitMap_Compressed_NUM);
    // run to change vis_bitmap_index
    init_input_flag();
    for (int k = 0; k < BitMap_Compressed_Length; k++) {
        // debug_M1._vis_bitmap_now[ROOT_ID % CORE_NUM].show();
        run_module(1);
        // check
        bitmap temp(true);
        for (int t = 0; t <= k; t++) temp.v[t] = false;
        for (int i = 0; i < CORE_NUM; i++) {
            // check internal signal
            ASSERT_EQ(debug_M1._iteration_id[i], 0);
            ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
            ASSERT_EQ(debug_M1._iteration_end[i], 0);
            ASSERT_EQ(debug_M1._vis_bitmap_index[i], 0);
            ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(temp));
            ASSERT_TRUE(debug_M1._vis_bitmap[i][0][0].compare(bitmap(true)));
            ASSERT_TRUE(debug_M1._vis_bitmap[i][1][0].compare(bitmap(true)));
            // check output signal
            ASSERT_EQ(active_v_id[i], i + CORE_NUM * k);
            ASSERT_EQ(active_v_dvalid[i], 1);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
            ASSERT_EQ(iteration_id[i], 0);
        }
    }
    // run one cycle and check index in pipeline
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M1._iteration_id[i], 0);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
        ASSERT_EQ(debug_M1._iteration_end[i], 0);
        ASSERT_EQ(debug_M1._vis_bitmap_index[i], 1);
        ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(bitmap(true)));
        ASSERT_TRUE(debug_M1._vis_bitmap[i][0][0].compare(bitmap(false)));
        ASSERT_TRUE(debug_M1._vis_bitmap[i][1][0].compare(bitmap(true)));
        // check output signal when index change
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
}

// given: rst=0, vis_index changed
// while: not control signal set to high
// then: normal output
TEST_F(RD_ACTIVE_VERTEX_TEST, test_given_VisIndexChanged_then_NormalOutput) {
    reset_module();
    // bitmap will init in 64 cycles
    init_input_flag();
    run_module(BitMap_Compressed_NUM);
    // run to change bitmap visit line
    init_input_flag();
    run_module(BitMap_Compressed_Length);
    // run one cycle and check index in root pipeline
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M1._iteration_id[i], 0);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
        ASSERT_EQ(debug_M1._iteration_end[i], 0);
        ASSERT_EQ(debug_M1._vis_bitmap_index[i], 1);
        ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(bitmap(true)));
        ASSERT_TRUE(debug_M1._vis_bitmap[i][0][0].compare(bitmap(false)));
        ASSERT_TRUE(debug_M1._vis_bitmap[i][1][0].compare(bitmap(true)));
        // check output signal when index change
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
    }
}

// given: rst=0, backend active vertex is valid
// while: visit_bitmap is init
// then: write bitmap successfully
TEST_F(RD_ACTIVE_VERTEX_TEST, test_given_BackendActiveId_then_WriteBitMapSuccess) {
    // initialize
    reset_module();
    // init bitmap
    run_module(BitMap_Compressed_NUM);
    // write
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        backend_active_v_id[i] = i;
        backend_active_v_dvalid[i] = 1;
        backend_active_v_updated[i] = 0;
    }
    run_module(1);
    init_input_flag();
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal signal
        ASSERT_EQ(debug_M1._iteration_id[i], 0);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
        ASSERT_EQ(debug_M1._iteration_end[i], 0);
        bitmap temp(true);
        temp.v[0] = false;
        ASSERT_TRUE(debug_M1._vis_bitmap[i][1][0].compare(temp));
        // check output signal
        ASSERT_EQ(active_v_id[i], i);
        ASSERT_EQ(active_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
        ASSERT_EQ(iteration_id[i], 0);
   }
}

// given: rst=0, backend_iteraion_end=1
// while: visit_bitmap already init
// then: iteration id changed
TEST_F(RD_ACTIVE_VERTEX_TEST, test_given_IterationEnd_then_SwitchIteration) {
    // initialize
    reset_module();
    // init bitmap
    run_module(BitMap_Compressed_NUM);
    // run to end
    run_module((BitMap_Compressed_Length + 1) * BitMap_Compressed_NUM + 1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M1._iteration_id[i], 0);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
        ASSERT_EQ(debug_M1._vis_bitmap_index[i], BitMap_Compressed_NUM);
        ASSERT_EQ(debug_M1._iteration_end[i], 1);
        ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(bitmap(false)));
        for (int j = 0; j < BitMap_Compressed_NUM; j++) {
            ASSERT_TRUE(debug_M1._vis_bitmap[i][0][j].compare(bitmap(false)));
            ASSERT_TRUE(debug_M1._vis_bitmap[i][1][j].compare(bitmap(true)));
        }
    }
    // run once to change iteration
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        backend_iteration_end[i] = 1;
        backend_iteration_end_dvalid[i] = 1;
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal signal
        ASSERT_EQ(debug_M1._iteration_id[i], 1);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
        ASSERT_EQ(debug_M1._vis_bitmap_index[i], 0);
        ASSERT_EQ(debug_M1._iteration_end[i], 0);
        // check output signal
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 1);
        ASSERT_EQ(iteration_end_dvalid[i], 1);
        ASSERT_EQ(iteration_id[i], 1);
    }
}

// given: rst=0, some back stage is full
// while: visit_biamp is init
// then: some pipeline output active id, and others not
TEST_F(RD_ACTIVE_VERTEX_TEST, test_given_BackStageFull_then_PartialOutput) {
    // initialize
    reset_module();
    // init bitmap
    run_module(BitMap_Compressed_NUM);
    // set full flag
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        if (i % 2 == 0) {
            nextstage_full[i] = 1;
        }
    }
    // run once
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal signal
        ASSERT_EQ(debug_M1._iteration_id[i], 0);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
        if (i % 2 == 0) {
            // check output signal
            ASSERT_EQ(active_v_id[i], 0);
            ASSERT_EQ(active_v_dvalid[i], 0);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
            ASSERT_EQ(iteration_id[i], 0);
        } else {
            ASSERT_EQ(active_v_id[i], i);
            ASSERT_EQ(active_v_dvalid[i], 1);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
            ASSERT_EQ(iteration_id[i], 0);
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