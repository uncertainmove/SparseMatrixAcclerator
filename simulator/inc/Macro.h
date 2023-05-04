//interface macro sys
#define Core_Input_Interface int Front_Push_Flag[], int Front_Active_V_ID[], int Front_Active_V_Value[], int Front_Active_V_Pull_First_Flag[], int Front_Active_V_DValid[],\
                    int Front_Iteration_End[], int Front_Iteration_End_DValid[]


//buffer macro sys
#define basic_buffer static queue<int> push_flag_buffer[CORE_NUM], v_id_buffer[CORE_NUM], v_value_buffer[CORE_NUM], pull_first_flag_buffer[CORE_NUM]

#define basic_buffer_push(para_1,para_2,para_3,para_4) push_flag_buffer[Core_ID].push(para_1); \
                          v_id_buffer[Core_ID].push(para_2); \
                          v_value_buffer[Core_ID].push(para_3); \
                          pull_first_flag_buffer[Core_ID].push(para_4);


#define basic_buffer_pop push_flag_buffer[Core_ID].pop();\
                         v_id_buffer[Core_ID].pop();\
                         v_value_buffer[Core_ID].pop();\
                         pull_first_flag_buffer[Core_ID].pop();

