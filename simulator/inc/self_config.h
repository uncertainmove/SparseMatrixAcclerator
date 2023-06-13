#ifndef SELF_CONFIG_H
#define SELF_CONFIG_H

#include "parameter.h"
#include "structure.h"
#include "framework.h"

config (
    P1,
    int Active_V_ID[CORE_NUM];
    int Active_V_DValid[CORE_NUM];
    int Iteration_End[CORE_NUM];
    int Iteration_End_DValid[CORE_NUM];
    int Iteration_ID[CORE_NUM];
)

config (
    P2,
    int Stage_Full[CORE_NUM];
    int Active_V_ID[CORE_NUM];
    int RD_Active_V_Offset_Addr[CORE_NUM];
    int RD_Active_V_Value_Addr[CORE_NUM];
    int Active_V_DValid[CORE_NUM];
    int Iteration_End[CORE_NUM];
    int Iteration_End_DValid[CORE_NUM];
    int Iteration_ID[CORE_NUM];
)

config (
    MEM,
    V_VALUE_TP Delta1_Bram_Data[CORE_NUM];
    int Delta1_Bram_Data_Valid[CORE_NUM];
    V_VALUE_TP Delta2_Bram_Data[CORE_NUM];
    int Delta2_Bram_Data_Valid[CORE_NUM];
    int Active_V_LOffset[CORE_NUM];
    int Active_V_ROffset[CORE_NUM];
    int Active_V_DValid[CORE_NUM];
    V_VALUE_TP PR_Uram_Data[CORE_NUM];
    int PR_Uram_Data_Valid[CORE_NUM];
    // hbm
    int Stage_Full[PSEUDO_CHANNEL_NUM];
    int Rd_HBM_Edge_Addr[PSEUDO_CHANNEL_NUM];
    int Rd_HBM_Edge_Valid[PSEUDO_CHANNEL_NUM];
    int Active_V_Edge[CORE_NUM];
    int Active_V_Edge_Valid[CORE_NUM];
    int HBM_Controller_Full[PSEUDO_CHANNEL_NUM];
    Cacheline_16 HBM_Controller_Edge[PSEUDO_CHANNEL_NUM];
    int HBM_Controller_DValid[PSEUDO_CHANNEL_NUM];
)

config (
    P4,
    int Stage_Full[CORE_NUM];
    int Rd_HBM_Edge_Addr[CORE_NUM];
    BitVector_16 Rd_HBM_Edge_Mask[CORE_NUM];
    int HBM_Active_V_ID[CORE_NUM];
    V_VALUE_TP HBM_Active_V_Value[CORE_NUM];
    int Rd_HBM_Edge_Valid[CORE_NUM];
    int Iteration_End[CORE_NUM];
    int Iteration_End_DValid[CORE_NUM];
    int Iteration_ID[CORE_NUM];
)

config (
    P5,
    int Stage_Full[CORE_NUM];
    int Rd_HBM_Edge_Addr[PSEUDO_CHANNEL_NUM];
    int Rd_HBM_Edge_Valid[PSEUDO_CHANNEL_NUM];
    int Active_V_ID[CORE_NUM];
    V_VALUE_TP Active_V_Value[CORE_NUM];
    int Active_V_DValid[CORE_NUM];
    int Iteration_End[CORE_NUM];
    int Iteration_End_DValid[CORE_NUM];
    int Iteration_ID[CORE_NUM];
)

config (
    P7,
    int Stage_Full[CORE_NUM];
    int Update_V_ID[CORE_NUM];
    V_VALUE_TP Update_V_Value[CORE_NUM];
    int Update_V_DValid[CORE_NUM];
    int Iteration_End[CORE_NUM];
    int Iteration_End_DValid[CORE_NUM];
    int Iteration_ID[CORE_NUM];
)

config (
    P8,
    int Hy1_Update_V_ID[CORE_NUM];
    V_VALUE_TP Hy1_Update_V_Value[CORE_NUM];
    int Hy1_Update_V_DValid[CORE_NUM];
    int Hy1_Iteration_End[CORE_NUM];
    int Hy1_Iteration_End_DValid[CORE_NUM];
    int Hy1_Iteration_ID[CORE_NUM];
    int Stage_Full[CORE_NUM];
)

config (
    P9,
    int Dest_Core_Full[CORE_NUM];
    int Wr_Vertex_BRAM_Addr[CORE_NUM];
    V_VALUE_TP Wr_Vertex_BRAM_Data[CORE_NUM];
    int Wr_Vertex_BRAM_Valid[CORE_NUM];
    int Wr_Vertex_BRAM_Iteration_End[CORE_NUM];
    int Wr_Vertex_BRAM_Iteration_End_DValid[CORE_NUM];
    int Wr_Vertex_BRAM_Iteration_ID[CORE_NUM];
)

config (
    P10,
    int Delta1_Bram_Rd_Addr[CORE_NUM];
    int Delta1_Bram_Rd_Valid[CORE_NUM];
    int Delta2_Bram_Rd_Addr[CORE_NUM];
    int Delta2_Bram_Rd_Valid[CORE_NUM];
    int Delta1_Bram_Wr_Addr[CORE_NUM];
    V_VALUE_TP Delta1_Bram_Wr_Value[CORE_NUM];
    int Delta1_Bram_Wr_Valid[CORE_NUM];
    int Delta2_Bram_Wr_Addr[CORE_NUM];
    V_VALUE_TP Delta2_Bram_Wr_Value[CORE_NUM];
    int Delta2_Bram_Wr_Valid[CORE_NUM];
    int PR_Uram_Rd_Addr[CORE_NUM];
    int PR_Uram_Rd_Valid[CORE_NUM];
    int PR_Uram_Wr_Addr[CORE_NUM];
    V_VALUE_TP PR_Uram_Wr_Value[CORE_NUM];
    int PR_Uram_Wr_Valid[CORE_NUM];
    int Stage_Full[CORE_NUM];
    V_VALUE_TP Src_Recv_Update_V_Value[CORE_NUM];
    int Src_Recv_Update_V_DValid[CORE_NUM];
    int Backend_Active_V_ID[CORE_NUM];
    int Backend_Active_V_Updated[CORE_NUM];
    int Backend_Active_V_DValid[CORE_NUM];
    int Iteration_End[CORE_NUM];
    int Iteration_End_DValid[CORE_NUM];
)

config (
    P11,
    int Active_V_ID[CORE_NUM];
    int Active_V_Updated[CORE_NUM];
    int Active_V_DValid[CORE_NUM];
    int Iteration_End[CORE_NUM];
    int Iteration_End_DValid[CORE_NUM];
)

#endif // SELF_CONFIG_H