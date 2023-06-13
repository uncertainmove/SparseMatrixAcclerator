#include "memory.h"
#include "parameter.h"

extern int clk;
extern int rst_rd;

V_VALUE_TP DELTA1_BRAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1];

using namespace std;

void Delta1_Bram_Single(
        int Core_ID,
        int Delta1_Bram_Rd_Addr, int Delta1_Bram_Rd_Valid,
        int Delta1_Bram_Wr_Addr, V_VALUE_TP Delta1_Bram_Wr_Value, int Delta1_Bram_Wr_Valid,

        V_VALUE_TP *Delta1_Bram_Data, int *Delta1_Bram_Data_Valid);

void Delta1_Bram(
        int Delta1_Bram_Rd_Addr[], int Delta1_Bram_Rd_Valid[],
        int Delta1_Bram_Wr_Addr[], V_VALUE_TP Delta1_Bram_Wr_Value[], int Delta1_Bram_Wr_Valid[],

        V_VALUE_TP *Delta1_Bram_Data, int *Delta1_Bram_Data_Valid) {

    for (int i = 0; i < CORE_NUM; ++ i) {
        Delta1_Bram_Single(
            i,
            Delta1_Bram_Rd_Addr[i], Delta1_Bram_Rd_Valid[i],
            Delta1_Bram_Wr_Addr[i], Delta1_Bram_Wr_Value[i], Delta1_Bram_Wr_Valid[i],

            &Delta1_Bram_Data[i], &Delta1_Bram_Data_Valid[i]);
    }
}

void Delta1_Bram_Single(
        int Core_ID,
        int Delta1_Bram_Rd_Addr, int Delta1_Bram_Rd_Valid,
        int Delta1_Bram_Wr_Addr, V_VALUE_TP Delta1_Bram_Wr_Value, int Delta1_Bram_Wr_Valid,

        V_VALUE_TP *Delta1_Bram_Data, int *Delta1_Bram_Data_Valid) {

    // read
    if (rst_rd) {
        *Delta1_Bram_Data = 0;
        *Delta1_Bram_Data_Valid = 0;
    }
    else {
        if (Delta1_Bram_Rd_Valid) {
            *Delta1_Bram_Data = DELTA1_BRAM[Core_ID][Delta1_Bram_Rd_Addr];
            *Delta1_Bram_Data_Valid = 1;
            #if (DEBUG && PRINT_CONS)
            #endif
        }
        else {
            *Delta1_Bram_Data = 0;
            *Delta1_Bram_Data_Valid = 0;
        }
    }

    // write
    if (rst_rd) {

    } else {
        if (Delta1_Bram_Wr_Valid) {
            DELTA1_BRAM[Core_ID][Delta1_Bram_Wr_Addr] = Delta1_Bram_Wr_Value;
        }
    }
}