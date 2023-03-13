// #include "Network.h"
#include "Accelerator.h"

Network_reg Nreg_1;
Network_reg Nreg_2;

int P6_Source_Core_Full_rd[CORE_NUM];

class Info_P_Om1 info_p_om1_self_rd,info_p_om1_self_wr;

class Info_P_Om2 info_p_om2_self_rd,info_p_om2_self_wr;

void Noc(Pipe* pipes){
    static Info_P6 self_info_p6;
    static Info_P_Om1 self_info_om1;
    static Info_P_Om2 self_info_om2;
    static Info_P7 self_info_p7;
    
    pipes[0].write(&self_info_p6);
    pipes[2].write(&self_info_p7);

    Network(self_info_p6.P6_Push_Flag, self_info_p6.P6_Update_V_ID, self_info_p6.P6_Update_V_Value, self_info_p6.P6_Pull_First_Flag, self_info_p6.P6_Update_V_DValid, self_info_p6.P6_Iteration_End, self_info_p6.P6_Iteration_End_DValid,
            self_info_p7.P7_Source_Core_Full,

            self_info_om1.Om1_Stage_Full,
            self_info_om1.Om1_Push_Flag, self_info_om1.Om1_Update_V_ID, self_info_om1.Om1_Update_V_Value, self_info_om1.Om1_Update_V_Pull_First_Flag, self_info_om1.Om1_Update_V_DValid, self_info_om1.Om1_Iteration_End, self_info_om1.Om1_Iteration_End_DValid,
            Nreg_1,
            0,0);

    Network(self_info_p7.P7_Om2_Send_Push_Flag, self_info_p7.P7_Om2_Send_Update_V_ID, self_info_p7.P7_Om2_Send_Update_V_Value, self_info_p7.P7_Om2_Send_Update_V_Pull_First_Flag, self_info_p7.P7_Om2_Send_Update_V_DValid, self_info_p7.P7_Om2_Iteration_End, self_info_p7.P7_Om2_Iteration_End_DValid,
            P6_Source_Core_Full_rd,

            self_info_om2.Om2_Stage_Full,
            self_info_om2.Om2_Push_Flag, self_info_om2.Om2_Update_V_ID, self_info_om2.Om2_Update_V_Value, self_info_om2.Om2_Update_V_Pull_First_Flag, self_info_om2.Om2_Update_V_DValid, self_info_om2.Om2_Iteration_End, self_info_om2.Om2_Iteration_End_DValid,
            Nreg_2,
            1,0);
    for (int i = 0; i < CORE_NUM; ++ i) {
        // 9 - 1 - 1 - 3
        P6_Source_Core_Full_rd[i] = 0;
    }
    pipes[3].read(&self_info_om1);
    pipes[4].read(&self_info_om2);
}

void noc_connect(Pipe* pipes){
    pipes[3].bind(&info_p_om1_self_wr,&info_p_om1_self_rd);
    pipes[4].bind(&info_p_om2_self_wr,&info_p_om2_self_rd);
}