#include "memory.h"
#include "parameter.h"
#include <bits/stdc++.h>

extern int clk;
extern int rst_rd;
V_VALUE_TP DELTA2_BRAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1];

using namespace std;

void Delta2_Bram_Single(
        int Core_ID,
        int Delta2_Bram_Rd_Addr, int Delta2_Bram_Rd_Valid,
        int Delta2_Bram_Wr_Addr, V_VALUE_TP Delta2_Bram_Wr_Value, int Delta2_Bram_Wr_Valid,

        V_VALUE_TP *Delta2_Bram_Data, int *Delta2_Bram_Data_Valid);

void Delta2_Bram(
        int Delta2_Bram_Rd_Addr[], int Delta2_Bram_Rd_Valid[],
        int Delta2_Bram_Wr_Addr[], V_VALUE_TP Delta2_Bram_Wr_Value[], int Delta2_Bram_Wr_Valid[],

        V_VALUE_TP *Delta2_Bram_Data, int *Delta2_Bram_Data_Valid) {

    for (int i = 0; i < CORE_NUM; ++ i) {
        Delta2_Bram_Single(
            i,
            Delta2_Bram_Rd_Addr[i], Delta2_Bram_Rd_Valid[i],
            Delta2_Bram_Wr_Addr[i], Delta2_Bram_Wr_Value[i], Delta2_Bram_Wr_Valid[i],

            &Delta2_Bram_Data[i], &Delta2_Bram_Data_Valid[i]);
    }
}

void Delta2_Bram_Single(
        int Core_ID,
        int Delta2_Bram_Rd_Addr, int Delta2_Bram_Rd_Valid,
        int Delta2_Bram_Wr_Addr, V_VALUE_TP Delta2_Bram_Wr_Value, int Delta2_Bram_Wr_Valid,

        V_VALUE_TP *Delta2_Bram_Data, int *Delta2_Bram_Data_Valid) {

    // read
    if (rst_rd) {
        *Delta2_Bram_Data = 0;
        *Delta2_Bram_Data_Valid = 0;
    }
    else {
        if (Delta2_Bram_Rd_Valid) {
            *Delta2_Bram_Data = DELTA2_BRAM[Core_ID][Delta2_Bram_Rd_Addr];
            *Delta2_Bram_Data_Valid = 1;
            if (Core_ID == 0) {
                // cout << "clk " << clk << " get delta2 rd, addr=" << Delta2_Bram_Rd_Addr << endl;
            }
        }
        else {
            *Delta2_Bram_Data = 0;
            *Delta2_Bram_Data_Valid = 0;
        }
    }

    // write
    if (rst_rd) {

    } else {
        if (Delta2_Bram_Wr_Valid) {
            DELTA2_BRAM[Core_ID][Delta2_Bram_Wr_Addr] = Delta2_Bram_Wr_Value;
        }
    }
}