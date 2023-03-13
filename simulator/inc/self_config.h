#include "Framesupport.h"
#include "Network.h"
#include "MEM.h"
#include "global.h"

class Info_P1 :public Info{
public:
    int P1_Global_Iteration_ID[CORE_NUM];
    virtual void copy(Info* info) {
        memcpy(this, info, sizeof(*this));
    }
 } ;

class Info_P6 : public Info {
public:
    int P6_Push_Flag[CORE_NUM];
    int P6_Update_V_ID[CORE_NUM];
    int P6_Update_V_Value[CORE_NUM] ;
    int P6_Pull_First_Flag[CORE_NUM];
    int P6_Update_V_DValid[CORE_NUM] ;
    int P6_Iteration_End[CORE_NUM];
    int P6_Iteration_End_DValid[CORE_NUM];
    virtual void copy(Info* info) {
        memcpy(this, info, sizeof(*this));
    }
 } ;

class Info_P5 : public Info {
public:
    int P5_Rd_HBM_Edge_Addr[CORE_NUM];
    int P5_Rd_HBM_Edge_Valid[CORE_NUM];
    virtual void copy(Info *info) {
        memcpy(this, info, sizeof(*this));
    }
 } ;

class Info_P7 : public Info{
public:
    int P7_Source_Core_Full[CORE_NUM]; 
    int P7_Om2_Send_Push_Flag[CORE_NUM]; 
    int P7_Om2_Send_Update_V_ID[CORE_NUM]; 
    int P7_Om2_Send_Update_V_Value[CORE_NUM]; 
    int P7_Om2_Send_Update_V_Pull_First_Flag[CORE_NUM]; 
    int P7_Om2_Send_Update_V_DValid[CORE_NUM]; 
    int P7_Om2_Iteration_End[CORE_NUM]; 
    int P7_Om2_Iteration_End_DValid[CORE_NUM]; 
    virtual void copy(Info *info) {
        memcpy(this, info, sizeof(*this));
    }
};

class Info_P_Om1 : public Info{
public:
    int Om1_Stage_Full[CORE_NUM];
    int Om1_Push_Flag[CORE_NUM];
    int Om1_Update_V_ID[CORE_NUM];
    int Om1_Update_V_Value[CORE_NUM];
    int Om1_Update_V_Pull_First_Flag[CORE_NUM];
    int Om1_Update_V_DValid[CORE_NUM];
    int Om1_Iteration_End[CORE_NUM];
    int Om1_Iteration_End_DValid[CORE_NUM];
    virtual void copy(Info *info) {
        memcpy(this, info, sizeof(*this));
    }
};

class Info_P_Om2 : public Info{
public:
    int Om2_Stage_Full[CORE_NUM];
    int Om2_Push_Flag[CORE_NUM];
    int Om2_Update_V_ID[CORE_NUM];
    int Om2_Update_V_Value[CORE_NUM];
    int Om2_Update_V_Pull_First_Flag[CORE_NUM];
    int Om2_Update_V_DValid[CORE_NUM];
    int Om2_Iteration_End[CORE_NUM];
    int Om2_Iteration_End_DValid[CORE_NUM];
    virtual void copy(Info *info) {
        memcpy(this, info, sizeof(*this));
    }
};

class Info_P_AIE_Global : public Info{
public:
    int AIE_Active_V_ID[CORE_NUM];
    int AIE_Active_V_Updated[CORE_NUM];
    int AIE_Active_V_DValid[CORE_NUM];
    virtual void copy(Info *info) {
        memcpy(this, info, sizeof(*this));
    }
};

class Info_P_HBM : public Info{
public:
    int HBM_Interface_Full[CORE_NUM];
    int HBM_Interface_Active_V_Edge[CORE_NUM];
    int HBM_Interface_Active_V_Edge_Valid[CORE_NUM];
    virtual void copy(Info *info) {
        memcpy(this, info, sizeof(*this));
    }
};

class Info_P_Global : public Info{
public:
    Powerlaw_Vid_Set P5_Global_Powerlaw_V_ID[CORE_NUM];
    Powerlaw_Vvisit_Set P5_Global_Powerlaw_V_Visit[CORE_NUM];
    int Om_Global_Iteration_ID[CORE_NUM];
    int P7_Global_Iteration_ID[CORE_NUM];
    virtual void copy(Info *info) {
        memcpy(this, info, sizeof(*this));
    }
};

extern Pipe pipe[K];


