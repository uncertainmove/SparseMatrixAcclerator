#ifndef NETWORK_H
#define NETWORK_H

#include "parameter.h"

using namespace std;

void Hyperx_Network(
    int Hy1_Front_Update_V_ID[], V_VALUE_TP Hy1_Front_Update_V_Value[], int Hy1_Front_Update_V_DValid[],
    int Hy1_Front_Iteration_End[], int Hy1_Front_Iteration_End_DValid[], int Hy1_Front_Iteration_ID[],
    int Dest_Core_Full[],

    int *Hy1_Update_V_ID, V_VALUE_TP *Hy1_Update_V_Value, int *Hy1_Update_V_DValid,
    int *Hy1_Iteration_End, int *Hy1_Iteration_End_DValid, int *Hy1_Iteration_ID,
    int *Hy1_Stage_Full);

#endif // NETOWRK_H