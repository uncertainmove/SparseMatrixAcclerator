#include "core.h"
#include "parameter.h"
#include "debug.h"

#if DEBUG
    debug_Backend_Core_Process debug_M9;
#endif

extern int clk;
extern int rst_rd;

using namespace std;

void Backend_Destination_Core_Single(
        int Core_ID,
        int HPX_Recv_Update_V_ID, V_VALUE_TP HPX_Recv_Update_V_Value, int HPX_Recv_Update_V_DValid,
        int HPX_Recv_Iteration_End, int HPX_Recv_Iteration_End_DValid, int HPX_Recv_Iteration_ID,
        int Next_StageFull,

        int *Dest_Core_Full,
        int *Wr_Vertex_BRAM_Addr, V_VALUE_TP *Wr_Vertex_BRAM_Data, int *Wr_Vertex_BRAM_Valid,
        int *Wr_Vertex_BRAM_Iteration_End, int *Wr_Vertex_BRAM_Iteration_End_DValid, int *Wr_Vertex_BRAM_Iteration_ID);

void Backend_Core(
        int HPX_Recv_Update_V_ID[], V_VALUE_TP HPX_Recv_Update_V_Value[], int HPX_Recv_Update_V_DValid[],
        int HPX_Recv_Iteration_End[], int HPX_Recv_Iteration_End_DValid[], int HPX_Recv_Iteration_ID[],
        int Next_StageFull[],

        int *Dest_Core_Full,
        int *Wr_Vertex_BRAM_Addr, V_VALUE_TP *Wr_Vertex_BRAM_Data, int *Wr_Vertex_BRAM_Valid,
        int *Wr_Vertex_BRAM_Iteration_End, int *Wr_Vertex_BRAM_Iteration_End_DValid, int *Wr_Vertex_BRAM_Iteration_ID) {

    for (int i = 0; i < CORE_NUM; ++ i) {
        Backend_Destination_Core_Single(
            i,
            HPX_Recv_Update_V_ID[i], HPX_Recv_Update_V_Value[i], HPX_Recv_Update_V_DValid[i],
            HPX_Recv_Iteration_End[i], HPX_Recv_Iteration_End_DValid[i], HPX_Recv_Iteration_ID[i],
            Next_StageFull[i],

            &Dest_Core_Full[i],
            &Wr_Vertex_BRAM_Addr[i], &Wr_Vertex_BRAM_Data[i], &Wr_Vertex_BRAM_Valid[i],
            &Wr_Vertex_BRAM_Iteration_End[i], &Wr_Vertex_BRAM_Iteration_End_DValid[i], &Wr_Vertex_BRAM_Iteration_ID[i]);
    }
}

void Backend_Destination_Core_Single(
        int Core_ID,
        int HPX_Recv_Update_V_ID, V_VALUE_TP HPX_Recv_Update_V_Value, int HPX_Recv_Update_V_DValid,
        int HPX_Recv_Iteration_End, int HPX_Recv_Iteration_End_DValid, int HPX_Recv_Iteration_ID,
        int Next_StageFull,

        int *Dest_Core_Full,
        int *Wr_Vertex_BRAM_Addr, V_VALUE_TP *Wr_Vertex_BRAM_Data, int *Wr_Vertex_BRAM_Valid,
        int *Wr_Vertex_BRAM_Iteration_End, int *Wr_Vertex_BRAM_Iteration_End_DValid, int *Wr_Vertex_BRAM_Iteration_ID) {
    if (rst_rd) {
        *Wr_Vertex_BRAM_Addr = 0;
        *Wr_Vertex_BRAM_Data = 0;
        *Wr_Vertex_BRAM_Valid = 0;

        *Wr_Vertex_BRAM_Iteration_ID = 0;
        *Dest_Core_Full = 0;
    }
    else {
        *Wr_Vertex_BRAM_Iteration_ID = HPX_Recv_Iteration_ID;
        *Dest_Core_Full = Next_StageFull;
        if (HPX_Recv_Update_V_DValid) {
            *Wr_Vertex_BRAM_Addr = HPX_Recv_Update_V_ID;
            *Wr_Vertex_BRAM_Data = HPX_Recv_Update_V_Value;
            *Wr_Vertex_BRAM_Valid = 1;

            if (*Wr_Vertex_BRAM_Addr == 0) {
                cout << "backend 0 value " << *Wr_Vertex_BRAM_Data << endl;
            }

            #if (DEBUG && PRINT_CONS)
            #endif
        }
        else{
            *Wr_Vertex_BRAM_Addr = 0;
            *Wr_Vertex_BRAM_Data = 0;
            *Wr_Vertex_BRAM_Valid = 0;
        }
    }

    if (!rst_rd && HPX_Recv_Iteration_End && HPX_Recv_Iteration_End_DValid && !HPX_Recv_Update_V_DValid) {
        *Wr_Vertex_BRAM_Iteration_End = 1;
        *Wr_Vertex_BRAM_Iteration_End_DValid = 1;
    }
    else {
        *Wr_Vertex_BRAM_Iteration_End = 0;
        *Wr_Vertex_BRAM_Iteration_End_DValid = 0;
    }
}