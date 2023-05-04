#include "gtest/gtest.h"
#include "Accelerator.h"
#include "debug.h"

#include <iostream>

using namespace std;

#if DEBUG

extern int rst_rd;
extern int rst_root;
extern int ROOT_ID;
extern debug_RD_ACTIVE_VERTEX_SINGLE debug_M1;
extern int DEBUG_PUSH_FLAG;
extern int DEBUG_NEXT_PUSH_FLAG;
extern int Csr_Offset[MAX_VERTEX_NUM + 1];

class RD_ACTIVE_VERTEX_TEST : public testing::Test {
public:
    // input signal
    int backend_active_v_id[CORE_NUM] = {0};
    int backend_active_v_updated[CORE_NUM] = {0};
    int backend_active_v_pull_first_flag[CORE_NUM] = {0};
    int backend_active_v_dvalid[CORE_NUM] = {0};
    int backend_iteration_end[CORE_NUM] = {0};
    int backend_iteration_end_dvalid[CORE_NUM] = {0};
    int nextstage_full[CORE_NUM] = {0};
    // output signal
    int push_flag[CORE_NUM];
    int active_v_id[CORE_NUM];
    int active_v_value[CORE_NUM];
    int active_v_pull_first_flag[CORE_NUM];
    int active_v_dvalid[CORE_NUM];
    int iteration_end[CORE_NUM];
    int iteration_end_dvalid[CORE_NUM];
    int global_iteration_id[CORE_NUM];

    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
        ASSERT_EQ(CONTROL_FLAG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            backend_active_v_id[i] = 0;
            backend_active_v_updated[i] = 0;
            backend_active_v_pull_first_flag[i] = 0;
            backend_active_v_dvalid[i] = 0;
            backend_iteration_end[i] = 0;
            backend_iteration_end_dvalid[i] = 0;
            nextstage_full[i] = 0;
        }
    }
    void init_local_reg() {
        for (int i = 0; i < CORE_NUM; i++) {
            queue<int> empty1, empty2;
            swap(debug_M1._active_vid_queue[0][i], empty1);
            swap(debug_M1._active_vid_queue[1][i], empty2);
            debug_M1._iteration_id[i] = 0;
            debug_M1._init_bitmap_id[i] = 0;
            debug_M1._iteration_end[i] = 0;
            for (int j = 0; j < BitMap_Compressed_NUM + 1; j++) {
                debug_M1._vis_bitmap[i][j].set(false);
            }
            debug_M1._vis_bitmap_index[i] = 0;
            debug_M1._vis_bitmap_now[i] = 0;
        }
    }
    void reset_module() {
        rst_rd = 1;
        rst_root = 1;
        init_input_flag();
        run_module(1); // run module once to bind debug port
        init_local_reg();
        run_module(9);
        rst_root = 0;
        run_module(1);
        rst_rd = 0;
    }
    void run_module(int cycle) {
        for (int i = 0; i < cycle; i++) {
            RD_ACTIVE_VERTEX(
                backend_active_v_id, backend_active_v_updated, backend_active_v_pull_first_flag, backend_active_v_dvalid,
                backend_iteration_end, backend_iteration_end_dvalid,
                nextstage_full,

                push_flag,
                active_v_id, active_v_value, active_v_pull_first_flag, active_v_dvalid,
                iteration_end, iteration_end_dvalid,
                global_iteration_id
            );
        }
    }
    virtual void TearDown() {
        reset_module();
    }
};

// given: rst=1, ROOT_ID=0
// while: active_vid_queue is empty
// then: active_vid_queue[ROOT_ID] is not empty
TEST_F(RD_ACTIVE_VERTEX_TEST, test_Push_given_TrueRst_then_ActiveVidQueueNotEmpty) {
    ROOT_ID = 0;
    DEBUG_PUSH_FLAG = 1;
    DEBUG_NEXT_PUSH_FLAG = 1;
    reset_module();
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal signal
        ASSERT_EQ(debug_M1._iteration_id[i], 0);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], 0);
        ASSERT_EQ(debug_M1._iteration_end[i], 0);
        if (i == ROOT_ID % CORE_NUM) {
            ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(1));
        } else {
            ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(0));
        }
        ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(0));
        // check output signal
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_value[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// given: rst=1, ROOT_ID=1
// while: bitmap is not initial
// then: bitmap[ROOT_ID]=1
TEST_F(RD_ACTIVE_VERTEX_TEST, test_Pull_given_TrueRst_then_BitmapInitial) {
    ROOT_ID = 0;
    DEBUG_PUSH_FLAG = 0;
    DEBUG_NEXT_PUSH_FLAG = 0;
    reset_module();
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal signal
        ASSERT_EQ(debug_M1._iteration_id[i], 0);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], 0);
        ASSERT_EQ(debug_M1._iteration_end[i], 0);
        ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(0));
        ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(0));
        ASSERT_EQ(debug_M1._vis_bitmap_index[i], 0);
        ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(bitmap(false)));
        for (int j = 0; j < BitMap_Compressed_NUM; j++) {
            ASSERT_TRUE(debug_M1._vis_bitmap[i][j].compare(bitmap(false)));
        }
        // check output signal
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_value[i], 0);
        ASSERT_EQ(active_v_pull_first_flag[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // bitmap will init in 64 cycles, we just check the first cycle
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal signal
        ASSERT_EQ(debug_M1._iteration_id[i], 0);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], 1);
        ASSERT_EQ(debug_M1._iteration_end[i], 0);
        ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(0));
        ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(0));
        ASSERT_EQ(debug_M1._vis_bitmap_index[i], 0);
        ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(bitmap(false)));
        for (int j = 0; j < BitMap_Compressed_NUM; j++) {
            if (i == ROOT_ID % CORE_NUM && j == ROOT_ID / CORE_NUM / BitMap_Compressed_Length) {
                bitmap temp(false);
                temp.v[(ROOT_ID / CORE_NUM) % BitMap_Compressed_Length] = true;
                // debug_M1._vis_bitmap[i][j].show();
                ASSERT_TRUE(debug_M1._vis_bitmap[i][j].compare(temp));
            } else {
                ASSERT_TRUE(debug_M1._vis_bitmap[i][j].compare(bitmap(false)));
            }
        }
        // check output signal
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_value[i], 0);
        ASSERT_EQ(active_v_pull_first_flag[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// given: rst=0, ROOT_ID=1, bitmap has init
// while: not control signal set to high
// then: output id
TEST_F(RD_ACTIVE_VERTEX_TEST, test_Pull_given_InitBitMap_then_OutputNormal) {
    ROOT_ID = 1;
    DEBUG_PUSH_FLAG = 0;
    DEBUG_NEXT_PUSH_FLAG = 0;
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
        ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(0));
        ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(0));
        ASSERT_EQ(debug_M1._vis_bitmap_index[i], 0);
        if (i == ROOT_ID % CORE_NUM) {
            bitmap temp(false);
            temp.v[ROOT_ID / CORE_NUM % BitMap_Compressed_Length] = true;
            // debug_M1._vis_bitmap_now[i].show();
            ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(temp));
        }
        for (int j = 0; j < BitMap_Compressed_NUM; j++) {
            if (i == ROOT_ID % CORE_NUM && j == ROOT_ID / CORE_NUM / BitMap_Compressed_Length) {
                bitmap temp(false);
                temp.v[(ROOT_ID / CORE_NUM) % BitMap_Compressed_Length] = true;
                // debug_M1._vis_bitmap[i][j].show();
                ASSERT_TRUE(debug_M1._vis_bitmap[i][j].compare(temp));
            } else {
                ASSERT_TRUE(debug_M1._vis_bitmap[i][j].compare(bitmap(false)));
            }
        }
        // check output signal
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_value[i], 0);
        ASSERT_EQ(active_v_pull_first_flag[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
    // run one cycle to output
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal signal
        ASSERT_EQ(debug_M1._iteration_id[i], 0);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
        ASSERT_EQ(debug_M1._iteration_end[i], 0);
        ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(0));
        ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(0));
        ASSERT_EQ(debug_M1._vis_bitmap_index[i], 0);
        if (i == ROOT_ID % CORE_NUM) {
            bitmap temp(false);
            temp.v[ROOT_ID / CORE_NUM % BitMap_Compressed_Length] = true;
            temp.v[1] = true;
            // debug_M1._vis_bitmap_now[i].show();
            ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(temp));
        } else {
            bitmap temp(false);
            temp.v[0] = true;
            ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(temp));
        }
        for (int j = 0; j < BitMap_Compressed_NUM; j++) {
            if (i == ROOT_ID % CORE_NUM && j == ROOT_ID / CORE_NUM / BitMap_Compressed_Length) {
                bitmap temp(false);
                temp.v[(ROOT_ID / CORE_NUM) % BitMap_Compressed_Length] = true;
                ASSERT_TRUE(debug_M1._vis_bitmap[i][j].compare(temp));
            } else {
                ASSERT_TRUE(debug_M1._vis_bitmap[i][j].compare(bitmap(false)));
            }
        }
        // check output signal
        ASSERT_EQ(push_flag[i], 0);
        if (i == ROOT_ID % CORE_NUM) {
            ASSERT_EQ(active_v_id[i], i + CORE_NUM);
        } else {
            ASSERT_EQ(active_v_id[i], i);
        }
        ASSERT_EQ(active_v_value[i], 1);
        ASSERT_EQ(active_v_pull_first_flag[i], 1);
        ASSERT_EQ(active_v_dvalid[i], 1);
        ASSERT_EQ(iteration_end[i], 0);
        ASSERT_EQ(iteration_end_dvalid[i], 0);
    }
}

// given: rst=0, ROOT_ID=0, bitmap has init
// while: not control signal set to high
// then: vis_bitmap_index changed
TEST_F(RD_ACTIVE_VERTEX_TEST, test_Pull_given_InitBitMap_then_VisIndexChanged) {
    ROOT_ID = 0;
    DEBUG_PUSH_FLAG = 0;
    DEBUG_NEXT_PUSH_FLAG = 0;
    reset_module();
    // bitmap will init in 64 cycles
    init_input_flag();
    run_module(BitMap_Compressed_NUM);
    // run to change vis_bitmap_index
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        backend_active_v_dvalid[i] = 1;
        backend_active_v_updated[i] = 1;
        backend_active_v_pull_first_flag[i] = 1;
    }
    for (int k = 0; k < BitMap_Compressed_Length - 1; k++) {
        // debug_M1._vis_bitmap_now[ROOT_ID % CORE_NUM].show();
        run_module(1);
        // check
        bitmap temp(false);
        for (int t = 0; t <= k; t++) temp.v[t] = true;
        for (int i = 0; i < CORE_NUM; i++) {
            // check internal signal
            ASSERT_EQ(debug_M1._iteration_id[i], 0);
            ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
            ASSERT_EQ(debug_M1._iteration_end[i], 0);
            ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(0));
            ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(0));
            ASSERT_EQ(debug_M1._vis_bitmap_index[i], 0);
            if (i == ROOT_ID % CORE_NUM) {
                bitmap temp2(temp);
                temp2.v[ROOT_ID / CORE_NUM % BitMap_Compressed_Length] = true;
                temp2.v[k + 1] = true;
                ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(temp2));
            } else {
                ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(temp));
            }
            for (int j = 0; j < BitMap_Compressed_NUM; j++) {
                // if (i == ROOT_ID % CORE_NUM && j == ROOT_ID / CORE_NUM / BitMap_Compressed_Length) {
                if (j == 0) {
                    bitmap temp2(false);
                    // temp2.v[(ROOT_ID / CORE_NUM) % BitMap_Compressed_Length] = true;
                    temp2.v[j] = true;
                    ASSERT_TRUE(debug_M1._vis_bitmap[i][j].compare(temp2));
                } else {
                    ASSERT_TRUE(debug_M1._vis_bitmap[i][j].compare(bitmap(false)));
                }
            }
            // check output signal
            ASSERT_EQ(push_flag[i], 0);
            if (ROOT_ID % CORE_NUM == i) {
                ASSERT_EQ(active_v_id[i], i + CORE_NUM * (k + 1));
            } else {
                ASSERT_EQ(active_v_id[i], i + CORE_NUM * k);
            }
            ASSERT_EQ(active_v_value[i], 1);
            ASSERT_EQ(active_v_pull_first_flag[i], 1);
            ASSERT_EQ(active_v_dvalid[i], 1);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
        }
    }
    // run one cycle and check index in root pipeline
    run_module(1);
    int i = ROOT_ID % CORE_NUM;
    ASSERT_EQ(debug_M1._iteration_id[i], 0);
    ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
    ASSERT_EQ(debug_M1._iteration_end[i], 0);
    ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(0));
    ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(0));
    ASSERT_EQ(debug_M1._vis_bitmap_index[i], 1);
    ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(bitmap(false)));
    bitmap temp(false);
    temp.v[ROOT_ID / CORE_NUM % BitMap_Compressed_Length] = true;
    ASSERT_TRUE(debug_M1._vis_bitmap[i][0].compare(temp));
    for (int j = 1; j < BitMap_Compressed_NUM; j++) {
        ASSERT_TRUE(debug_M1._vis_bitmap[i][j].compare(bitmap(false)));
    }
    // check output signal when index change
    ASSERT_EQ(push_flag[i], 0);
    ASSERT_EQ(active_v_id[i], 0);
    ASSERT_EQ(active_v_value[i], 0);
    ASSERT_EQ(active_v_pull_first_flag[i], 0);
    ASSERT_EQ(active_v_dvalid[i], 0);
    ASSERT_EQ(iteration_end[i], 0);
    ASSERT_EQ(iteration_end_dvalid[i], 0);
}

// given: rst=0, ROOT_ID=0, vis_index changed
// while: not control signal set to high
// then: normal output
TEST_F(RD_ACTIVE_VERTEX_TEST, test_Pull_given_VisIndexChanged_then_NormalOutput) {
    ROOT_ID = 0;
    DEBUG_PUSH_FLAG = 0;
    DEBUG_NEXT_PUSH_FLAG = 0;
    reset_module();
    // bitmap will init in 64 cycles
    init_input_flag();
    run_module(BitMap_Compressed_NUM);
    // run to change bitmap visit line
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        backend_active_v_dvalid[i] = 1;
        backend_active_v_updated[i] = 1;
        backend_active_v_pull_first_flag[i] = 1;
    }
    run_module(BitMap_Compressed_Length);
    // run one cycle and check index in root pipeline
    run_module(1);
    int i = ROOT_ID % CORE_NUM;
    ASSERT_EQ(debug_M1._iteration_id[i], 0);
    ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
    ASSERT_EQ(debug_M1._iteration_end[i], 0);
    ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(0));
    ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(0));
    ASSERT_EQ(debug_M1._vis_bitmap_index[i], 1);
    bitmap temp_now(false);
    temp_now.v[0] = true;
    ASSERT_TRUE(debug_M1._vis_bitmap_now[i].compare(temp_now));
    // bitmap temp(false);
    // temp.v[ROOT_ID / CORE_NUM % BitMap_Compressed_Length] = true;
    ASSERT_TRUE(debug_M1._vis_bitmap[i][0].compare(temp_now));
    for (int j = 1; j < BitMap_Compressed_NUM; j++) {
        ASSERT_TRUE(debug_M1._vis_bitmap[i][j].compare(bitmap(false)));
    }
    // check output signal when index change
    ASSERT_EQ(push_flag[i], 0);
    ASSERT_EQ(active_v_id[i], i + CORE_NUM * BitMap_Compressed_Length);
    ASSERT_EQ(active_v_value[i], 1);
    ASSERT_EQ(active_v_pull_first_flag[i], 1);
    ASSERT_EQ(active_v_dvalid[i], 1);
    ASSERT_EQ(iteration_end[i], 0);
    ASSERT_EQ(iteration_end_dvalid[i], 0);
}

// given: rst=0, ROOT_ID
// while: active_vid_queue is init, waiting to init bitmap
// then: init bitmap, no output
TEST_F(RD_ACTIVE_VERTEX_TEST, test_Push_given_FalseRst_then_InitBitmap) {
    // initialize
    ROOT_ID = 0;
    DEBUG_PUSH_FLAG = 1;
    DEBUG_NEXT_PUSH_FLAG = 1;
    reset_module();
    // init bitmap & check
    int cycle = BitMap_Compressed_NUM;
    for (int i = 0; i < cycle; i++) {
        run_module(1);
        for (int j = 0; j < CORE_NUM; j++) {
            ASSERT_EQ(debug_M1._iteration_id[i], 0);
            ASSERT_EQ(debug_M1._init_bitmap_id[j], i + 1);
            ASSERT_EQ(debug_M1._iteration_end[i], 0);
            if (ROOT_ID != i) {
                ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(0));
            } else {
                ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(1));
            }
            ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(0));
            ASSERT_EQ(push_flag[i], 0);
            ASSERT_EQ(active_v_id[i], 0);
            ASSERT_EQ(active_v_value[i], 0);
            ASSERT_EQ(active_v_dvalid[i], 0);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
        }
    }
}

// given: rst=0, ROOT_ID
// while: active_vid_queue is init
// then: active_vid_queue[ROOT_ID] is empty, output active id
TEST_F(RD_ACTIVE_VERTEX_TEST, test_Push_given_FalseRst_then_ActiveVidValid) {
    // initialize
    ROOT_ID = 0;
    DEBUG_PUSH_FLAG = 1;
    DEBUG_NEXT_PUSH_FLAG = 1;
    reset_module();
    // init bitmap
    run_module(BitMap_Compressed_NUM);
    // run once
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal signal
        ASSERT_EQ(debug_M1._iteration_id[i], 0);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
        if (ROOT_ID == i) {
            ASSERT_EQ(debug_M1._iteration_end[i], 0);
        } else {
            ASSERT_EQ(debug_M1._iteration_end[i], 1);
        }
        ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(0));
        ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(0));
        // check output signal
        if (ROOT_ID == i) {
            ASSERT_EQ(push_flag[i], 1);
            ASSERT_EQ(active_v_id[i], 0);
            ASSERT_EQ(active_v_value[i], 1);
            ASSERT_EQ(active_v_dvalid[i], 1);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
        } else {
            ASSERT_EQ(push_flag[i], 0);
            ASSERT_EQ(active_v_id[i], 0);
            ASSERT_EQ(active_v_value[i], 0);
            ASSERT_EQ(active_v_dvalid[i], 0);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
        }
   }
}

// given: rst=0, ROOT_ID, backend active vertex is valid
// while: active_vid_queue is init
// then: write active_vid_queue successfully
TEST_F(RD_ACTIVE_VERTEX_TEST, test_Push_given_BackendActiveId_then_WriteQueueSuccess) {
    // initialize
    ROOT_ID = 0;
    DEBUG_PUSH_FLAG = 1;
    DEBUG_NEXT_PUSH_FLAG = 1;
    reset_module();
    // init bitmap
    run_module(BitMap_Compressed_NUM);
    // write queue
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        if (i % 2 == 1) {
            backend_active_v_id[i] = i;
            backend_active_v_dvalid[i] = 1;
            backend_active_v_updated[i] = 1;
        }
    }
    run_module(2);
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        if (i % 2 == 0) {
            backend_active_v_id[i] = i;
            backend_active_v_dvalid[i] = 1;
            backend_active_v_updated[i] = 1;
        }
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal signal
        ASSERT_EQ(debug_M1._iteration_id[i], 0);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
        ASSERT_EQ(debug_M1._iteration_end[i], 1);
        ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(0));
        if (i == 0) {
            ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(1));
        } else {
            ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(2));
        }
        // check output signal
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_value[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 1);
        ASSERT_EQ(iteration_end_dvalid[i], 1);
   }
}

// given: rst=0, ROOT_ID, backend_iteraion_end=1
// while: active_vid_queue already init
// then: iteration id changed
TEST_F(RD_ACTIVE_VERTEX_TEST, test_Push_given_IterationEnd_then_SwitchIteration) {
    // initialize
    ROOT_ID = 0;
    DEBUG_PUSH_FLAG = 1;
    DEBUG_NEXT_PUSH_FLAG = 1;
    reset_module();
    // init bitmap
    run_module(BitMap_Compressed_NUM);
    // write queue twice
    // queue[1] will have two elements
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        backend_active_v_id[i] = i;
        backend_active_v_dvalid[i] = 1;
        backend_active_v_updated[i] = 1;
    }
    run_module(2);
    // set iteration end flag
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        backend_iteration_end[i] = 1;
        backend_iteration_end_dvalid[i] = 1;
    }
    // run once to change iteration
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        // check internal signal
        ASSERT_EQ(debug_M1._iteration_id[i], 1);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
        ASSERT_EQ(debug_M1._iteration_end[i], 0);
        ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(0));
        if (i == 0) {
            ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(0));
        } else {
            ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(1));
        }
        // check output signal
        ASSERT_EQ(push_flag[i], 0);
        ASSERT_EQ(active_v_id[i], 0);
        ASSERT_EQ(active_v_value[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
        ASSERT_EQ(iteration_end[i], 1);
        ASSERT_EQ(iteration_end_dvalid[i], 1);
    }
}

// given: rst=0, ROOT_ID, some back stage is full
// while: active_vid_queue is init
// then: some pipeline output active id, and others not
TEST_F(RD_ACTIVE_VERTEX_TEST, test_Push_given_BackStageFull_then_PartialOutput) {
    // initialize
    ROOT_ID = 0;
    DEBUG_PUSH_FLAG = 1;
    DEBUG_NEXT_PUSH_FLAG = 1;
    reset_module();
    // init bitmap
    run_module(BitMap_Compressed_NUM);
    // write queue twice, same element will be dropped
    // queue[1] will have one elements
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        backend_active_v_id[i] = i;
        backend_active_v_dvalid[i] = 1;
        backend_active_v_updated[i] = 1;
    }
    run_module(2);
    // set iteration end flag
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        backend_iteration_end[i] = 1;
        backend_iteration_end_dvalid[i] = 1;
    }
    // run once to change iteration
    run_module(1);
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(debug_M1._iteration_end[i], 0);
        ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), i == ROOT_ID ? size_t(0) : size_t(1));
    }
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
        ASSERT_EQ(debug_M1._iteration_id[i], 1);
        ASSERT_EQ(debug_M1._init_bitmap_id[i], BitMap_Compressed_NUM);
        ASSERT_EQ(debug_M1._active_vid_queue[0][i].size(), size_t(0));
        if (i % 2 == 0) {
            ASSERT_EQ(debug_M1._iteration_end[i], i == ROOT_ID ? 1 : 0);
            ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), i == ROOT_ID ? size_t(0) : size_t(1));
        } else {
            ASSERT_EQ(debug_M1._active_vid_queue[1][i].size(), size_t(0));
            ASSERT_EQ(debug_M1._iteration_end[i], 0);
        }
        if (i % 2 == 0) {
            // check output signal
            ASSERT_EQ(push_flag[i], 0);
            ASSERT_EQ(active_v_id[i], 0);
            ASSERT_EQ(active_v_value[i], 0);
            ASSERT_EQ(active_v_dvalid[i], 0);
            ASSERT_EQ(iteration_end[i], 0);
            ASSERT_EQ(iteration_end_dvalid[i], 0);
        } else {
            ASSERT_EQ(push_flag[i], 1);
            ASSERT_EQ(active_v_id[i], i);
            ASSERT_EQ(active_v_value[i], 2);
            ASSERT_EQ(active_v_dvalid[i], 1);
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