#include "memory.h"
#include "parameter.h"
#include "debug.h"

#if DEBUG
    debug_RD_Offset_Uram debug_M3;
#endif

extern int clk;
extern int rst_rd;

int Offset_URAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1][2];

using namespace std;

void Offset_Uram_Single(
        int Core_ID,
        int Front_RD_Active_V_Offset_Addr, int Front_Active_V_DValid,

        int *Uram_LOffset, int *Uram_ROffset, int *Uram_DValid);

void Offset_Uram(
        int Front_RD_Active_V_Offset_Addr[], int Front_Active_V_DValid[],

        int *Active_V_LOffset, int *Active_V_ROffset, int *Active_V_DValid) {

    for (int i = 0; i < CORE_NUM; ++ i) {
        Offset_Uram_Single(
            i,
            Front_RD_Active_V_Offset_Addr[i], Front_Active_V_DValid[i],

            &Active_V_LOffset[i], &Active_V_ROffset[i], &Active_V_DValid[i]);
    }
}

void Offset_Uram_Single(
        int Core_ID,
        int Front_RD_Active_V_Offset_Addr, int Front_Active_V_DValid,

        int *Uram_LOffset, int *Uram_ROffset, int *Uram_DValid) {
    if (rst_rd) {
        *Uram_LOffset = 0;
        *Uram_ROffset = 0;
        *Uram_DValid = 0;
    }
    else {
        if (Front_Active_V_DValid) {
            *Uram_LOffset = Offset_URAM[Core_ID][Front_RD_Active_V_Offset_Addr][0];
            *Uram_ROffset = Offset_URAM[Core_ID][Front_RD_Active_V_Offset_Addr][1];
            *Uram_DValid = 1;
        }
        else {
            *Uram_LOffset = 0;
            *Uram_ROffset = 0;
            *Uram_DValid = 0;
        }
    }
}
