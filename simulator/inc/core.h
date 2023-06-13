#ifndef CORE_H
#define CORE_H

#include "parameter.h"
#include "structure.h"

void RD_ACTIVE_VERTEX(
        int Backend_Active_V_ID[], int Backend_Active_V_Updated[], int Backend_Active_V_DValid[],
        int Backend_Iteration_End[], int Backend_Iteration_End_DValid[],
        int NextStage_Full[],

        int *Active_V_ID, int *Active_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid,
        int *Iteration_ID);

void Read_Active_Vertex_Offset(
        int Front_Active_V_ID[], int Front_Active_V_DValid[],
        int Front_Iteration_End[], int Front_Ieration_End_DValid[], int Front_Iteration_ID[],
        int NextStage_Full[],

        int *Stage_Full,
        int *Active_V_ID, int *RD_Active_V_Offset_Addr, int *RD_Active_V_Value_Addr, int *Active_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID);

void RD_Active_Vertex_Edge(
        int Front_Active_V_ID[], int Front_Active_V_DValid[], 
        int Front_Iteration_End[], int Front_Iteration_End_DValid[], int Front_Iteration_ID[],
        int Front_Active_V_LOffset[], int Front_Active_V_Roffset[], int Front_Offset_DValid[],
        V_VALUE_TP Front_Active_V_Value[], int Front_Active_V_Value_Valid[],
        int CombineStage_Full[],

        int *Stage_Full,
        int *Rd_HBM_Edge_Addr, BitVector_16 *Rd_HBM_Edge_Mask,
        int *HBM_Active_V_ID, V_VALUE_TP *HBM_Active_V_Value, int *Rd_HBM_Edge_Valid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID);

void Generate_HBM_Edge_Rqst(
        int Front_Rd_HBM_Edge_Addr[], BitVector_16 Front_Rd_HBM_Edge_Mask[], int Front_Rd_HBM_Edge_Valid[],
        int Front_Active_V_ID[], V_VALUE_TP Front_Active_V_Value[],
        int Front_Iteration_End[], int Front_Iteration_End_DValid[], int Front_Iteration_ID[],
        int HBM_Full[], int NextStage_Full[],

        int *Stage_Full,
        int *Rd_HBM_Edge_Addr, int *Rd_HBM_Edge_Valid,
        int *Active_V_ID, V_VALUE_TP *Active_V_Value, int *Active_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID);

void Schedule(
        int Front_Active_V_ID[], V_VALUE_TP Front_Active_V_Value[], int Front_Active_V_DValid[],
        int Front_Iteration_End[], int Front_Iteration_End_DValid[], int Front_Iteration_ID[],
        int HBM_Iterface_Active_V_Edge[], int HBM_Iterface_Active_V_Edge_Valid[],
        int NextStage_Full[],

        int *Stage_Full,
        int *Update_V_ID, V_VALUE_TP *Update_V_Value, int *Update_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID);

void Backend_Core(
        int HPX_Recv_Update_V_ID[], V_VALUE_TP HPX_Recv_Update_V_Value[], int HPX_Recv_Update_V_DValid[],
        int HPX_Recv_Iteration_End[], int HPX_Recv_Iteration_End_DValid[], int HPX_Recv_Iteration_ID[],
        int Next_StageFull[],

        int *Dest_Core_Full,
        int *Wr_Vertex_BRAM_Addr, V_VALUE_TP *Wr_Vertex_BRAM_Data, int *Wr_Vertex_BRAM_Valid,
        int *Wr_Vertex_BRAM_Iteration_End, int *Wr_Vertex_BRAM_Iteration_End_DValid, int *Wr_Vertex_BRAM_Iteration_ID);

void Vertex_RAM_Controller(
        int Rd_Addr_Src[], int Rd_Valid_Src[], // for delta 1
        int Wr_Addr_Dst[], V_VALUE_TP Wr_V_Value_Dst[], int Wr_Valid_Dst[], // for delta 2 & pr
        int Front_Iteration_End[], int Front_Iteration_End_DValid[],
        int Front_Iteration_ID[],

        V_VALUE_TP Delta1_Bram_Data[], int Delta1_Bram_Data_Valid[],
        V_VALUE_TP Delta2_Bram_Data[], int Delta2_Bram_Data_Valid[],
        V_VALUE_TP PR_Uram_Data[], int PR_Uram_Data_Valid[],
        int *Delta1_Bram_Rd_Addr, int *Delta1_Bram_Rd_Valid,
        int *Delta2_Bram_Rd_Addr, int *Delta2_Bram_Rd_Valid,
        int *Delta1_Bram_Wr_Addr, V_VALUE_TP *Delta1_Bram_Wr_Value, int *Delta1_Bram_Wr_Valid,
        int *Delta2_Bram_Wr_Addr, V_VALUE_TP *Delta2_Bram_Wr_Value, int *Delta2_Bram_Wr_Valid,
        int *PR_Uram_Rd_Addr, int *PR_Uram_Rd_Valid,
        int *PR_Uram_Wr_Addr, V_VALUE_TP *PR_Uram_Wr_Value, int *PR_Uram_Wr_Valid,

        int *Stage_Full,
        V_VALUE_TP *Src_Recv_Update_V_Value, int *Src_Recv_Update_V_DValid,
        int *Backend_Active_V_ID, int *Backend_Active_V_Updated, int *Backend_Active_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid);

void Apply_Iteration_End(
        int Front_Active_V_ID[], int Front_Active_V_Updated[], int Front_Active_V_DValid[],
        int Front_Iteration_End[], int Front_Iteration_End_DValid[],

        int *Active_V_ID, int *Active_V_Updated, int *Active_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid);

#endif // CORE_H