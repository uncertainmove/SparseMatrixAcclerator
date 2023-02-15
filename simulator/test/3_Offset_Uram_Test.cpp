#include "gtest/gtest.h"
#include "Accelerator.h"
#include "debug.h"

#include <iostream>

using namespace std;

#if DEBUG

extern int rst_rd;
extern int ROOT_ID;
extern int Offset_URAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1][2];
extern debug_RD_Offset_Uram debug_M3;

class RD_Offset_Uram_Test : public testing::Test {
public:
    // input signal
    int front_rd_active_v_offset_addr[CORE_NUM] = {0};
    int front_active_v_dvalid[CORE_NUM] = {0};
    // output signal
    int active_v_loffset[CORE_NUM];
    int active_v_roffset[CORE_NUM];
    int active_v_dvalid[CORE_NUM];

    static void SetUpTestCase() {
        for (int i = 0; i < CORE_NUM; i++) {
            for (int j = 0; j < MAX_VERTEX_NUM / CORE_NUM + 1; j++) {
                if (i == 0 && j == 0) {
                    Offset_URAM[i][j][0] = 0;
                    Offset_URAM[i][j][1] = Offset_URAM[i][j][0] + j * CORE_NUM + j + 1;
                    continue;
                }
                if (j == 0) {
                    Offset_URAM[i][j][0] = Offset_URAM[i - 1][MAX_VERTEX_NUM / CORE_NUM][1];
                } else {
                    Offset_URAM[i][j][0] = Offset_URAM[i][j - 1][1];
                }
                Offset_URAM[i][j][1] = Offset_URAM[i][j][0] + j * CORE_NUM + j + 1;
            }
        }
    }
    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    void init_input_flag() {
        for (int i = 0; i < CORE_NUM; i++) {
            front_rd_active_v_offset_addr[i] = 0;
            front_active_v_dvalid[i] = 0;
        }
    }
    void init_local_reg() {
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
            RD_Offset_Uram (
                front_rd_active_v_offset_addr, front_active_v_dvalid,

                active_v_loffset, active_v_roffset, active_v_dvalid
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
TEST_F(RD_Offset_Uram_Test, test_PushPull_given_Rst_then_LocalBufferInit) {
    // reset
    reset_module();
    // run once
    init_input_flag();
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        ASSERT_EQ(active_v_loffset[i], 0);
        ASSERT_EQ(active_v_roffset[i], 0);
        ASSERT_EQ(active_v_dvalid[i], 0);
    }
}

// given: rst=0, ROOT_ID, front addr is valid
// while: no control singal set to high
// then: normal input and output
TEST_F(RD_Offset_Uram_Test, test_PushPull_given_InputActiveAddr_then_OutputOffset) {
    // reset
    reset_module();
    // run once to access memory
    init_input_flag();
    for (int i = 0; i < CORE_NUM; i++) {
        if (i % 2 == 0) {
            front_rd_active_v_offset_addr[i] = 0;
            front_active_v_dvalid[i] = 1;
        }
    }
    run_module(1);
    // check
    for (int i = 0; i < CORE_NUM; i++) {
        if (i % 2 == 0) {
            ASSERT_EQ(active_v_dvalid[i], 1);
            ASSERT_EQ(active_v_loffset[i], Offset_URAM[i][front_rd_active_v_offset_addr[i]][0]);
            ASSERT_EQ(active_v_roffset[i], Offset_URAM[i][front_rd_active_v_offset_addr[i]][1]);
        } else {
            ASSERT_EQ(active_v_dvalid[i], 0);
            ASSERT_EQ(active_v_loffset[i], 0);
            ASSERT_EQ(active_v_roffset[i], 0);
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