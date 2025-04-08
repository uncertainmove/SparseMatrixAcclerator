#ifndef MEMORY_H
#define MEMORY_H

#include "parameter.h"
#include "structure.h"

void Delta1_Bram(
        int Delta1_Bram_Rd_Addr[], int Delta1_Bram_Rd_Valid[],
        int Delta1_Bram_Wr_Addr[], V_VALUE_TP Delta1_Bram_Wr_Value[], int Delta1_Bram_Wr_Valid[],

        V_VALUE_TP *Delta1_Bram_Data, int *Delta1_Bram_Data_Valid);

void Delta2_Bram(
        int Delta2_Bram_Rd_Addr[], int Delta2_Bram_Rd_Valid[],
        int Delta2_Bram_Wr_Addr[], V_VALUE_TP Delta2_Bram_Wr_Value[], int Delta2_Bram_Wr_Valid[],

        V_VALUE_TP *Delta2_Bram_Data, int *Delta2_Bram_Data_Valid);

void Offset_Uram(
        int Front_RD_Active_V_Offset_Addr[], int Front_Active_V_DValid[],

        int *Active_V_LOffset, int *Active_V_ROffset,int *Active_V_DValid);

void PR_Uram(
        int PR_Uram_Rd_Addr[], int PR_Uram_Rd_Valid[],
        int PR_Uram_Wr_Addr[], V_VALUE_TP PR_Uram_Wr_Value[], int PR_Uram_Wr_Valid[],

        V_VALUE_TP *PR_Uram_Data, int *PR_Uram_Data_Valid);

void HBM_Interface(
        int Front_Rd_HBM_Edge_Addr[], int Front_Rd_HBM_Edge_Valid[],
        Cacheline_16 HBM_Controller_Edge[], int HBM_Controller_DValid[],
        int HBM_Controller_Full[],

        int *Stage_Full,
        int *Rd_HBM_Edge_Addr, int *Rd_HBM_Edge_Valid,
        int *Active_V_Edge, int *Active_V_Edge_Valid);

void HBM_Controller_IP(
        int Rd_HBM_Edge_Addr[], int Rd_HBM_Edge_Valid[],

        int *HBM_Controller_Full,
        Cacheline_16 *HBM_Controller_Edge, int *HBM_Controller_DValid);

void HBM_Controller_IP_Ideal(
        int Rd_HBM_Edge_Addr[], int Rd_HBM_Edge_Valid[],

        int *HBM_Controller_Full,
        Cacheline_16 *HBM_Controller_Edge, int *HBM_Controller_DValid);

#endif // MEMORY_H