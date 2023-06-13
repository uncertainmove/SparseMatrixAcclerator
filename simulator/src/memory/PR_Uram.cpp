#include "memory.h"
#include "parameter.h"
#include "debug.h"
#include <bits/stdc++.h>

#if DEBUG
#endif

extern int clk;
extern int rst_rd;
V_VALUE_TP PR_URAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1][2];

int v_updated = 0;

using namespace std;

void PR_Uram_Single(
        int Core_ID,
        int PR_Uram_Rd_Addr, int PR_Uram_Rd_Valid,
        int PR_Uram_Wr_Addr, V_VALUE_TP PR_Uram_Wr_Value, int PR_Uram_Wr_Valid,

        V_VALUE_TP *PR_Uram_Data, int *PR_Uram_Data_Valid);

void PR_Uram(
        int PR_Uram_Rd_Addr[], int PR_Uram_Rd_Valid[],
        int PR_Uram_Wr_Addr[], V_VALUE_TP PR_Uram_Wr_Value[], int PR_Uram_Wr_Valid[],

        V_VALUE_TP *PR_Uram_Data, int *PR_Uram_Data_Valid) {

    for (int i = 0; i < CORE_NUM; ++ i) {
        PR_Uram_Single(
            i,
            PR_Uram_Rd_Addr[i], PR_Uram_Rd_Valid[i],
            PR_Uram_Wr_Addr[i], PR_Uram_Wr_Value[i], PR_Uram_Wr_Valid[i],

            &PR_Uram_Data[i], &PR_Uram_Data_Valid[i]);
    }
}

void PR_Uram_Single(
        int Core_ID,
        int PR_Uram_Rd_Addr, int PR_Uram_Rd_Valid,
        int PR_Uram_Wr_Addr, V_VALUE_TP PR_Uram_Wr_Value, int PR_Uram_Wr_Valid,

        V_VALUE_TP *PR_Uram_Data, int *PR_Uram_Data_Valid) {
    #if DEBUG
        // no debug signal
    #endif
    // read
    if (rst_rd) {
        *PR_Uram_Data = 0;
        *PR_Uram_Data_Valid = 0;
    }
    else {
        if (PR_Uram_Rd_Valid) {
            *PR_Uram_Data = PR_URAM[Core_ID][PR_Uram_Rd_Addr][0];
            *PR_Uram_Data_Valid = 1;

            v_updated += 1;
            if (Core_ID == 0) {
                // cout << "clk " << clk << " get pr rd, addr=" << PR_Uram_Rd_Addr << endl;
            }
        }
        else {
            *PR_Uram_Data = 0;
            *PR_Uram_Data_Valid = 0;
        }
    }

    // write
    if (rst_rd) {

    } else {
        if (PR_Uram_Wr_Valid) {
            PR_URAM[Core_ID][PR_Uram_Wr_Addr][0] = PR_Uram_Wr_Value;
        }
    }
}
