#include "Accelerator.h"
#if DEBUG
    #include "debug.h"
    // debug_Hyperx_Network debug_M8;
#endif

#define debug_8 0
#define debug_8_0 1
#define debug_8_1 0

extern int clk;
extern int rst_rd;


using namespace std;

//map映射记录进入和写出的点 对应观察是否存在重复写出和丢失的点

static int mp_hy1[1048576]; 
static int mp_hy2[1048576]; 
static int rst_hy1 = 0;
static int rst_hy2 = 0;
/*write in pipe*/
static int tmp_hy1_vertical_push_flag_in[CORE_NUM][VERTICAL_LEN], tmp_hy1_vertical_v_id_in[CORE_NUM][VERTICAL_LEN],
         tmp_hy1_vertical_v_value_in[CORE_NUM][VERTICAL_LEN],tmp_hy1_vertical_pull_first_flag_in[CORE_NUM][VERTICAL_LEN],
          tmp_hy1_vertical_v_valid_in[CORE_NUM][VERTICAL_LEN], tmp_hy1_vertical_stage_full[CORE_NUM][VERTICAL_LEN], 
          tmp_hy1_vertical_block[CORE_NUM][VERTICAL_LEN];
static int tmp_hy1_horizontal_push_flag_in[CORE_NUM][HORIZONTAL_LEN], tmp_hy1_horizontal_v_id_in[CORE_NUM][HORIZONTAL_LEN], tmp_hy1_horizontal_v_value_in[CORE_NUM][HORIZONTAL_LEN],
    tmp_hy1_horizontal_pull_first_flag_in[CORE_NUM][HORIZONTAL_LEN], tmp_hy1_horizontal_v_valid_in[CORE_NUM][HORIZONTAL_LEN],  tmp_hy1_horizontal_stage_full[CORE_NUM][HORIZONTAL_LEN], tmp_hy1_horizontal_block[CORE_NUM][HORIZONTAL_LEN];


static int tmp_hy2_vertical_push_flag_in[CORE_NUM][VERTICAL_LEN], tmp_hy2_vertical_v_id_in[CORE_NUM][VERTICAL_LEN], tmp_hy2_vertical_v_value_in[CORE_NUM][VERTICAL_LEN], 
        tmp_hy2_vertical_pull_first_flag_in[CORE_NUM][VERTICAL_LEN], tmp_hy2_vertical_v_valid_in[CORE_NUM][VERTICAL_LEN],  tmp_hy2_vertical_stage_full[CORE_NUM][VERTICAL_LEN], tmp_hy2_vertical_block[CORE_NUM][VERTICAL_LEN];
static int tmp_hy2_horizontal_push_flag_in[CORE_NUM][HORIZONTAL_LEN], tmp_hy2_horizontal_v_id_in[CORE_NUM][HORIZONTAL_LEN], tmp_hy2_horizontal_v_value_in[CORE_NUM][HORIZONTAL_LEN],
    tmp_hy2_horizontal_pull_first_flag_in[CORE_NUM][HORIZONTAL_LEN], tmp_hy2_horizontal_v_valid_in[CORE_NUM][HORIZONTAL_LEN],  tmp_hy2_horizontal_stage_full[CORE_NUM][HORIZONTAL_LEN], tmp_hy2_horizontal_block[CORE_NUM][HORIZONTAL_LEN];
/*write in pipe*/

/*schedule*/
static int hy1_input_write_vertical_buffer[CORE_NUM],hy1_input_write_horizontal_buffer[CORE_NUM],hy1_input_write_output[CORE_NUM],
           hy1_horizontal_write_vertical_buffer[CORE_NUM],hy1_horizontal_write_output[CORE_NUM],
            hy1_vertical_write_output[CORE_NUM];

static int hy2_input_write_vertical_buffer[CORE_NUM],hy2_input_write_horizontal_buffer[CORE_NUM],hy2_input_write_output[CORE_NUM],
           hy2_horizontal_write_vertical_buffer[CORE_NUM],hy2_horizontal_write_output[CORE_NUM],
            hy2_vertical_write_output[CORE_NUM];
/*schedule*/
/*read pipe schedule pass to the hyperx*/
static int tmp_hy1_horizontal_v_valid_index_min[CORE_NUM] ,tmp_hy1_horizontal_v_valid_in_min[CORE_NUM],tmp_hy1_vertical_v_valid_index_min[CORE_NUM] ,tmp_hy1_vertical_v_valid_in_min[CORE_NUM],
           tmp_hy2_horizontal_v_valid_index_min[CORE_NUM] ,tmp_hy2_horizontal_v_valid_in_min[CORE_NUM],tmp_hy2_vertical_v_valid_index_min[CORE_NUM] ,tmp_hy2_vertical_v_valid_in_min[CORE_NUM];
/*read pipe schedule*/

/*read pipe */
static int hy1_vertical_block[CORE_NUM][VERTICAL_LEN], hy1_vertical_stage_full[CORE_NUM][VERTICAL_LEN], hy1_vertical_push_flag_in[CORE_NUM][VERTICAL_LEN], 
    hy1_vertical_v_id_in[CORE_NUM][VERTICAL_LEN], hy1_vertical_v_value_in[CORE_NUM][VERTICAL_LEN], hy1_vertical_pull_first_flag_in[CORE_NUM][VERTICAL_LEN],
    hy1_vertical_v_valid_in[CORE_NUM][VERTICAL_LEN];
static int hy1_horizontal_block[CORE_NUM][HORIZONTAL_LEN], hy1_horizontal_stage_full[CORE_NUM][HORIZONTAL_LEN], hy1_horizontal_push_flag_in[CORE_NUM][HORIZONTAL_LEN], 
hy1_horizontal_v_id_in[CORE_NUM][HORIZONTAL_LEN], hy1_horizontal_v_value_in[CORE_NUM][HORIZONTAL_LEN], hy1_horizontal_pull_first_flag_in[CORE_NUM][HORIZONTAL_LEN],
hy1_horizontal_v_valid_in[CORE_NUM][HORIZONTAL_LEN];

static int hy2_vertical_block[CORE_NUM][VERTICAL_LEN], hy2_vertical_stage_full[CORE_NUM][VERTICAL_LEN], hy2_vertical_push_flag_in[CORE_NUM][VERTICAL_LEN], 
    hy2_vertical_v_id_in[CORE_NUM][VERTICAL_LEN], hy2_vertical_v_value_in[CORE_NUM][VERTICAL_LEN], hy2_vertical_pull_first_flag_in[CORE_NUM][VERTICAL_LEN],
    hy2_vertical_v_valid_in[CORE_NUM][VERTICAL_LEN];
static int hy2_horizontal_block[CORE_NUM][HORIZONTAL_LEN], hy2_horizontal_stage_full[CORE_NUM][HORIZONTAL_LEN], hy2_horizontal_push_flag_in[CORE_NUM][HORIZONTAL_LEN], 
hy2_horizontal_v_id_in[CORE_NUM][HORIZONTAL_LEN], hy2_horizontal_v_value_in[CORE_NUM][HORIZONTAL_LEN], hy2_horizontal_pull_first_flag_in[CORE_NUM][HORIZONTAL_LEN],
hy2_horizontal_v_valid_in[CORE_NUM][HORIZONTAL_LEN];
/*read pipe*/

//input horizontal target col,row
static int tmp_hy1_input_col_id[CORE_NUM], tmp_hy1_input_row_id[CORE_NUM], tmp_hy1_horizontal_col_id[CORE_NUM], tmp_hy1_horizontal_row_id[CORE_NUM];
static int tmp_hy2_input_col_id[CORE_NUM], tmp_hy2_input_row_id[CORE_NUM], tmp_hy2_horizontal_col_id[CORE_NUM], tmp_hy2_horizontal_row_id[CORE_NUM];

/*write old schedule sign*/
static int hy1_write_old_vertical_network[CORE_NUM], hy2_write_old_vertical_network[CORE_NUM];
static int hy1_write_old_vertical_network_block[CORE_NUM], hy2_write_old_vertical_network_block[CORE_NUM];

void Hyperx_Network_1_Single(int Hyperx_Switch_ID,
                            int Push_Flag, int V_ID_In, int V_Value_In, int Pull_First_Flag, int V_Valid_In, 
                            int Iteration_End_In, int Iteration_End_DValid_In,int Next_Stage_Full, 

                        
                            int *Hy_Vertical_Push_Flag_Out, int *Hy_Vertical_V_ID_Out, int *Hy_Vertical_V_Value_Out, int *Hy_Vertical_Pull_First_Flag, 
                            int *Hy_Vertical_V_Valid_Out,
                            int *Vertical_Stage_Full,int* Vertical_Block,

                            int *Hy_Horizontal_Push_Flag_Out, int *Hy_Horizontal_V_ID_Out, int *Hy_Horizontal_V_Value_Out, int *Hy_Horizontal_Pull_First_Flag, 
                            int *Hy_Horizontal_V_Valid_Out,
                            int *Horizontal_Stage_Full,int *Horizontal_Block,

                            int *Hy_Push_Flag, int *Hy_Update_V_ID, int *Hy_Update_V_Value, int *Hy_Pull_First_Flag, int *Hy_Update_V_DValid, 
                            int *Hy_Iteration_End, int *Hy_Iteration_End_DValid, int *Stage_Full_Hy,
                            
                            int* combine_input_buffer_size,int *combine_push_flag_buffer_input_front,int *combine_id_buffer_input_front,int *combine_value_buffer_input_front,int * combine_pull_first_flag_buffer_input_front,
                            int* combine_horizontal_buffer_size,int *combine_push_flag_buffer_horizontal_front,int *combine_id_buffer_horizontal_front,int *combine_value_buffer_horizontal_front,int * combine_pull_first_flag_buffer_horizontal_front,     
                            int* combine_vertical_buffer_size,int *combine_push_flag_buffer_vertical_front,int *combine_id_buffer_vertical_front,int *combine_value_buffer_vertical_front,int * combine_pull_first_flag_buffer_vertical_front);

void Hyperx_Network_2_Single(int Hyperx_Switch_ID,
                            int Push_Flag, int V_ID_In, int V_Value_In, int Pull_First_Flag, int V_Valid_In, 
                            int Iteration_End_In, int Iteration_End_DValid_In,int Next_Stage_Full, 

                         
                            int *Hy_Vertical_Push_Flag_Out, int *Hy_Vertical_V_ID_Out, int *Hy_Vertical_V_Value_Out, int *Hy_Vertical_Pull_First_Flag, 
                            int *Hy_Vertical_V_Valid_Out,
                            int *Vertical_Stage_Full,int* Vertical_Block,

                            int *Hy_Horizontal_Push_Flag_Out, int *Hy_Horizontal_V_ID_Out, int *Hy_Horizontal_V_Value_Out, int *Hy_Horizontal_Pull_First_Flag, 
                            int *Hy_Horizontal_V_Valid_Out,
                            int *Horizontal_Stage_Full,int *Horizontal_Block,

                            int *Hy_Push_Flag, int *Hy_Update_V_ID, int *Hy_Update_V_Value, int *Hy_Pull_First_Flag, int *Hy_Update_V_DValid, 
                            int *Hy_Iteration_End, int *Hy_Iteration_End_DValid, int *Stage_Full_Hy,
                            
                            int* combine_input_buffer_size,int *combine_push_flag_buffer_input_front,int *combine_id_buffer_input_front,int *combine_value_buffer_input_front,int * combine_pull_first_flag_buffer_input_front,
                            int* combine_horizontal_buffer_size,int *combine_push_flag_buffer_hmp_hy1orizontal_front,int *combine_id_buffer_horizontal_front,int *combine_value_buffer_horizontal_front,int * combine_pull_first_flag_buffer_horizontal_front,
                            int* combine_vertical_buffer_size,int *combine_push_flag_buffer_vertical_front,int *combine_id_buffer_vertical_front,int *combine_value_buffer_vertical_front,int * combine_pull_first_flag_buffer_vertical_front);

void Hyperx_Network(int Hy1_Front_Push_Flag[], int Hy1_Front_Update_V_ID[], int Hy1_Front_Update_V_Value[], int Hy1_Front_Pull_First_Flag[], int Hy1_Front_Update_V_DValid[], int Hy1_Front_Iteration_End[], int Hy1_Front_Iteration_End_DValid[],
                   int Hy2_Front_Push_Flag[], int Hy2_Front_Update_V_ID[], int Hy2_Front_Update_V_Value[], int Hy2_Front_Pull_First_Flag[], int Hy2_Front_Update_V_DValid[], int Hy2_Front_Iteration_End[], int Hy2_Front_Iteration_End_DValid[],
                   int Source_Core_Full[],

                   int *Hy1_Push_Flag, int *Hy1_Update_V_ID, int *Hy1_Update_V_Value, int *Hy1_Pull_First_Flag, int *Hy1_Update_V_DValid, int *Hy1_Iteration_End, int *Hy1_Iteration_End_DValid, int *Stage_Full_Hy1,
                   int *Hy2_Push_Flag, int *Hy2_Update_V_ID, int *Hy2_Update_V_Value, int *Hy2_Pull_First_Flag, int *Hy2_Update_V_DValid, int *Hy2_Iteration_End, int *Hy2_Iteration_End_DValid, int *Stage_Full_Hy2) {
//硬件排序上彼此间连线更简单的一种实现方法
    static int tmp_hy1_push_flag_in[CORE_NUM], tmp_hy1_v_id_in[CORE_NUM], tmp_hy1_v_value_in[CORE_NUM], tmp_hy1_pull_first_flag_in[CORE_NUM], tmp_hy1_v_valid_in[CORE_NUM],
        tmp_hy1_iteration_end[CORE_NUM], tmp_hy1_iteration_end_dvalid[CORE_NUM], tmp_hy1_stage_full[CORE_NUM];

    static int tmp_hy2_push_flag_in[CORE_NUM], tmp_hy2_v_id_in[CORE_NUM], tmp_hy2_v_value_in[CORE_NUM], tmp_hy2_pull_first_flag_in[CORE_NUM], tmp_hy2_v_valid_in[CORE_NUM],
        tmp_hy2_iteration_end[CORE_NUM], tmp_hy2_iteration_end_dvalid[CORE_NUM], tmp_hy2_stage_full[CORE_NUM];
    
    /*read pipe schedule*/
    int tmp_hy1_horizontal_v_valid_in_arr[CORE_NUM][HORIZONTAL_LEN], tmp_hy1_horizontal_v_valid_index_arr[CORE_NUM][HORIZONTAL_LEN];
    int tmp_hy1_vertical_v_valid_in_arr[CORE_NUM][VERTICAL_LEN], tmp_hy1_vertical_v_valid_index_arr[CORE_NUM][VERTICAL_LEN];
    int tmp_hy2_horizontal_v_valid_in_arr[CORE_NUM][HORIZONTAL_LEN], tmp_hy2_horizontal_v_valid_index_arr[CORE_NUM][HORIZONTAL_LEN];
    int tmp_hy2_vertical_v_valid_in_arr[CORE_NUM][VERTICAL_LEN], tmp_hy2_vertical_v_valid_index_arr[CORE_NUM][VERTICAL_LEN];
    /*read pipe schedule*/

    /*combine logic */
    static int hy1_combine_horizontal_buffer_size[CORE_NUM],hy1_combine_push_flag_buffer_horizontal_front[CORE_NUM],hy1_combine_id_buffer_horizontal_front[CORE_NUM],hy1_combine_value_buffer_horizontal_front[CORE_NUM],hy1_combine_pull_first_flag_buffer_horizontal_front[CORE_NUM],
    hy1_combine_vertical_buffer_size[CORE_NUM],hy1_combine_push_flag_buffer_vertical_front[CORE_NUM],hy1_combine_id_buffer_vertical_front[CORE_NUM],hy1_combine_value_buffer_vertical_front[CORE_NUM],hy1_combine_pull_first_flag_buffer_vertical_front[CORE_NUM],
    hy1_combine_input_buffer_size[CORE_NUM],hy1_combine_push_flag_buffer_input_front[CORE_NUM],hy1_combine_id_buffer_input_front[CORE_NUM],hy1_combine_value_buffer_input_front[CORE_NUM],hy1_combine_pull_first_flag_buffer_input_front[CORE_NUM];
    static int hy2_combine_horizontal_buffer_size[CORE_NUM],hy2_combine_push_flag_buffer_horizontal_front[CORE_NUM],hy2_combine_id_buffer_horizontal_front[CORE_NUM],hy2_combine_value_buffer_horizontal_front[CORE_NUM],hy2_combine_pull_first_flag_buffer_horizontal_front[CORE_NUM],
    hy2_combine_vertical_buffer_size[CORE_NUM],hy2_combine_push_flag_buffer_vertical_front[CORE_NUM],hy2_combine_id_buffer_vertical_front[CORE_NUM],hy2_combine_value_buffer_vertical_front[CORE_NUM],hy2_combine_pull_first_flag_buffer_vertical_front[CORE_NUM],
    hy2_combine_input_buffer_size[CORE_NUM],hy2_combine_push_flag_buffer_input_front[CORE_NUM],hy2_combine_id_buffer_input_front[CORE_NUM],hy2_combine_value_buffer_input_front[CORE_NUM],hy2_combine_pull_first_flag_buffer_input_front[CORE_NUM];

    /*combine logic */

    /*check*/
    #if(debug_8_0)
    if(clk % 500 == 0){
        int tmp_sum1 = 0, tmp_sum2 = 0;
        for(int i = 0 ; i<1048576 ; i++){
            if(mp_hy1[i]){
                // printf("point %d still has %d in net hy1\n",i,mp_hy1[i]);
                tmp_sum1 += mp_hy1[i];
            }
            if(mp_hy2[i]){
                // printf("point %d still has %d in net hy2\n",i,mp_hy2[i]);
                tmp_sum2 += mp_hy2[i];
            }
        }
        printf("hy1 has %d points,hy2 has %d points\n",tmp_sum1,tmp_sum2);
    }
    #endif
    /*check*/


    for (int i = 0; i < CORE_NUM; ++ i) {
        tmp_hy1_push_flag_in[i] = Hy1_Front_Push_Flag[i];
        tmp_hy1_v_id_in[i] = Hy1_Front_Update_V_ID[i];
        tmp_hy1_v_value_in[i] = Hy1_Front_Update_V_Value[i];
        tmp_hy1_pull_first_flag_in[i] = Hy1_Front_Pull_First_Flag[i];
        tmp_hy1_v_valid_in[i] = Hy1_Front_Update_V_DValid[i];
        tmp_hy1_iteration_end[i] = Hy1_Front_Iteration_End[i];
        tmp_hy1_iteration_end_dvalid[i] = Hy1_Front_Iteration_End_DValid[i];
        // tmp_hy1_stage_full[HYPERX_DEPTH][i] = Source_Core_Full[i];
        tmp_hy1_stage_full[i] = Source_Core_Full[i];
        // check power-law vertex
        
        for (int j = 0; j < POWER_LAW_V_NUM; j++) {
            if (Hy1_Front_Update_V_Value[i] == VERTEX_NOT_READ[j]) {
                printf("catch power-law vertex in %d, v_id=%d, v_value=%d\n", i, Hy1_Front_Update_V_ID[i], Hy1_Front_Update_V_Value[i]);
                exit(-1);
            }
        }
        
        tmp_hy2_push_flag_in[i] = Hy2_Front_Push_Flag[i];
        tmp_hy2_v_id_in[i] = Hy2_Front_Update_V_ID[i];
        tmp_hy2_v_value_in[i] = Hy2_Front_Update_V_Value[i];
        tmp_hy2_pull_first_flag_in[i] = Hy2_Front_Pull_First_Flag[i];
        tmp_hy2_v_valid_in[i] = Hy2_Front_Update_V_DValid[i];
        tmp_hy2_iteration_end[i] = Hy2_Front_Iteration_End[i];
        tmp_hy2_iteration_end_dvalid[i] = Hy2_Front_Iteration_End_DValid[i];
        tmp_hy2_stage_full[i] = 0; //destination core never full
    }
    //send info
    for (int i = 0; i < CORE_NUM; ++ i) {
        for(int j = 0 ; j<HORIZONTAL_LEN ; j++){
            hy1_horizontal_block[i][j] = tmp_hy1_horizontal_block[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
        }

        for(int j = 0 ; j<VERTICAL_LEN; j++){
            hy1_vertical_block[i][j] = tmp_hy1_vertical_block[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
        }

        for(int j = 0 ; j<HORIZONTAL_LEN ; j++){
            hy2_horizontal_block[i][j] = tmp_hy2_horizontal_block[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
        }

        for(int j = 0 ; j<VERTICAL_LEN; j++){
            hy2_vertical_block[i][j] = tmp_hy2_vertical_block[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
        }

    }

    for (int i = 1; i <= CORE_NUM; i++) {
        Hyperx_Network_1_Single(i-1 ,
                                tmp_hy1_push_flag_in[i - 1], tmp_hy1_v_id_in[i - 1], tmp_hy1_v_value_in[i - 1], tmp_hy1_pull_first_flag_in[i - 1], 
                                tmp_hy1_v_valid_in[i - 1], tmp_hy1_iteration_end[i - 1], tmp_hy1_iteration_end_dvalid[i - 1],tmp_hy1_stage_full[i-1],
                                /*read*/
                               
                                /*write*/
                                tmp_hy1_vertical_push_flag_in[i-1], tmp_hy1_vertical_v_id_in[i-1], tmp_hy1_vertical_v_value_in[i-1], 
                                tmp_hy1_vertical_pull_first_flag_in[i-1], tmp_hy1_vertical_v_valid_in[i-1], tmp_hy1_vertical_stage_full[i-1],tmp_hy1_vertical_block[i-1],

                                tmp_hy1_horizontal_push_flag_in[i-1], tmp_hy1_horizontal_v_id_in[i-1], tmp_hy1_horizontal_v_value_in[i-1],
                                tmp_hy1_horizontal_pull_first_flag_in[i-1], tmp_hy1_horizontal_v_valid_in[i-1],  tmp_hy1_horizontal_stage_full[i-1],tmp_hy1_horizontal_block[i-1],

                                Hy1_Push_Flag, Hy1_Update_V_ID, Hy1_Update_V_Value, Hy1_Pull_First_Flag, Hy1_Update_V_DValid, 
                                Hy1_Iteration_End, Hy1_Iteration_End_DValid, Stage_Full_Hy1, 
                            
                                &hy1_combine_input_buffer_size[i-1],&hy1_combine_push_flag_buffer_input_front[i-1],&hy1_combine_id_buffer_input_front[i-1],&hy1_combine_value_buffer_input_front[i-1], &hy1_combine_pull_first_flag_buffer_input_front[i-1],
                                &hy1_combine_horizontal_buffer_size[i-1],&hy1_combine_push_flag_buffer_horizontal_front[i-1],&hy1_combine_id_buffer_horizontal_front[i-1],&hy1_combine_value_buffer_horizontal_front[i-1], &hy1_combine_pull_first_flag_buffer_horizontal_front[i-1],
                                &hy1_combine_vertical_buffer_size[i-1],&hy1_combine_push_flag_buffer_vertical_front[i-1],&hy1_combine_id_buffer_vertical_front[i-1],&hy1_combine_value_buffer_vertical_front[i-1], &hy1_combine_pull_first_flag_buffer_vertical_front[i-1]
                                );

        Hyperx_Network_2_Single(i-1 ,
                                tmp_hy2_push_flag_in[i - 1], tmp_hy2_v_id_in[i - 1], tmp_hy2_v_value_in[i - 1], tmp_hy2_pull_first_flag_in[i - 1], 
                                tmp_hy2_v_valid_in[i - 1], tmp_hy2_iteration_end[i - 1], tmp_hy2_iteration_end_dvalid[i - 1],tmp_hy2_stage_full[i-1],
                                /*read*/
                              
                                /*write*/
                                tmp_hy2_vertical_push_flag_in[i-1], tmp_hy2_vertical_v_id_in[i-1], tmp_hy2_vertical_v_value_in[i-1], 
                                tmp_hy2_vertical_pull_first_flag_in[i-1], tmp_hy2_vertical_v_valid_in[i-1], tmp_hy2_vertical_stage_full[i-1],tmp_hy2_vertical_block[i-1],

                                tmp_hy2_horizontal_push_flag_in[i-1], tmp_hy2_horizontal_v_id_in[i-1], tmp_hy2_horizontal_v_value_in[i-1],
                                tmp_hy2_horizontal_pull_first_flag_in[i-1], tmp_hy2_horizontal_v_valid_in[i-1], tmp_hy2_horizontal_stage_full[i-1],tmp_hy2_horizontal_block[i-1],

                                Hy2_Push_Flag, Hy2_Update_V_ID, Hy2_Update_V_Value, Hy2_Pull_First_Flag, Hy2_Update_V_DValid, 
                                Hy2_Iteration_End, Hy2_Iteration_End_DValid, Stage_Full_Hy2, 

                                &hy2_combine_input_buffer_size[i-1],&hy2_combine_push_flag_buffer_input_front[i-1],&hy2_combine_id_buffer_input_front[i-1],&hy2_combine_value_buffer_input_front[i-1],&hy2_combine_pull_first_flag_buffer_input_front[i-1] ,
                                &hy2_combine_horizontal_buffer_size[i-1],&hy2_combine_push_flag_buffer_horizontal_front[i-1],&hy2_combine_id_buffer_horizontal_front[i-1],&hy2_combine_value_buffer_horizontal_front[i-1], &hy2_combine_pull_first_flag_buffer_horizontal_front[i-1] ,
                                &hy2_combine_vertical_buffer_size[i-1],&hy2_combine_push_flag_buffer_vertical_front[i-1],&hy2_combine_id_buffer_vertical_front[i-1],&hy2_combine_value_buffer_vertical_front[i-1], &hy2_combine_pull_first_flag_buffer_vertical_front[i-1]
                                );
    }


    for(int i = 0 ; i<CORE_NUM ; i++){
        int tmp_hy1_horizontal_v_id_front = hy1_combine_push_flag_buffer_horizontal_front[i]?hy1_combine_id_buffer_horizontal_front[i]:hy1_combine_value_buffer_horizontal_front[i];
        tmp_hy1_horizontal_row_id[i] = (tmp_hy1_horizontal_v_id_front%CORE_NUM)/HORIZONTAL_LEN;
        tmp_hy1_horizontal_col_id[i] = (tmp_hy1_horizontal_v_id_front%CORE_NUM)%HORIZONTAL_LEN;
        int tmp_hy1_input_v_id_front = hy1_combine_push_flag_buffer_input_front[i]?hy1_combine_id_buffer_input_front[i]:hy1_combine_value_buffer_input_front[i];
        tmp_hy1_input_row_id[i] = (tmp_hy1_input_v_id_front%CORE_NUM)/HORIZONTAL_LEN;
        tmp_hy1_input_col_id[i] = (tmp_hy1_input_v_id_front%CORE_NUM)%HORIZONTAL_LEN;

        int tmp_hy2_horizontal_v_id_front = hy2_combine_id_buffer_horizontal_front[i];
        tmp_hy2_horizontal_row_id[i] = (tmp_hy2_horizontal_v_id_front%CORE_NUM)/HORIZONTAL_LEN;
        tmp_hy2_horizontal_col_id[i] = (tmp_hy2_horizontal_v_id_front%CORE_NUM)%HORIZONTAL_LEN;
        int tmp_hy2_input_v_id_front = hy2_combine_id_buffer_input_front[i];
        tmp_hy2_input_row_id[i] = (tmp_hy2_input_v_id_front%CORE_NUM)/HORIZONTAL_LEN;
        tmp_hy2_input_col_id[i] = (tmp_hy2_input_v_id_front%CORE_NUM)%HORIZONTAL_LEN;
    }

    //send dtage full info
    for (int i = 0; i < CORE_NUM; ++ i) {
        for(int j = 0 ; j<HORIZONTAL_LEN ; j++){
            hy1_horizontal_stage_full[i][j] = tmp_hy1_horizontal_stage_full[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
        }

        for(int j = 0 ; j<VERTICAL_LEN; j++){
            hy1_vertical_stage_full[i][j] = tmp_hy1_vertical_stage_full[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
        }

        for(int j = 0 ; j<HORIZONTAL_LEN ; j++){
            hy2_horizontal_stage_full[i][j] = tmp_hy2_horizontal_stage_full[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
        }

        for(int j = 0 ; j<VERTICAL_LEN; j++){
            hy2_vertical_stage_full[i][j] = tmp_hy2_vertical_stage_full[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
        }

    }

    /*write output combine logic*/

    for(int i = 0 ; i<CORE_NUM ; i++){
        if(hy1_combine_vertical_buffer_size[i]){
            hy1_vertical_write_output[i] = 1;
            hy1_horizontal_write_output[i] = 0;
            hy1_input_write_output[i] = 0;
        }else if(hy1_combine_horizontal_buffer_size[i] && tmp_hy1_horizontal_col_id[i] == i%HORIZONTAL_LEN && tmp_hy1_horizontal_row_id[i] == i/HORIZONTAL_LEN){
            hy1_vertical_write_output[i] = 0;
            hy1_horizontal_write_output[i] = 1;
            hy1_input_write_output[i] = 0;
        }else if(hy1_combine_input_buffer_size[i] &&  tmp_hy1_input_col_id[i] == i%HORIZONTAL_LEN && tmp_hy1_input_row_id[i] == i/HORIZONTAL_LEN){
            hy1_vertical_write_output[i] = 0;
            hy1_horizontal_write_output[i] = 0;
            hy1_input_write_output[i] = 1;
        }else{
            hy1_vertical_write_output[i] = 0;
            hy1_horizontal_write_output[i] = 0;
            hy1_input_write_output[i] = 0;
        }
        #if(debug_8)
        // if( hy1_combine_id_buffer_input_front[i] == 15074 && hy1_combine_input_buffer_size[i] &&  tmp_hy1_input_col_id[i] == i%HORIZONTAL_LEN && tmp_hy1_input_row_id[i] == i/HORIZONTAL_LEN){
        //     printf("15074 wait for pop\n");
        // }else if( hy1_combine_id_buffer_input_front[i] == 15074 ){
        //     printf("15074 input buffer size:%d input col id:%d input row id:%d\n",hy1_combine_input_buffer_size[i],tmp_hy1_input_col_id[i],tmp_hy1_input_row_id[i]);
        // }
        #endif

        if(hy2_combine_vertical_buffer_size[i]){
            hy2_vertical_write_output[i] = 1;
            hy2_horizontal_write_output[i] = 0;
            hy2_input_write_output[i] = 0;
        }else if(hy2_combine_horizontal_buffer_size[i] && tmp_hy2_horizontal_col_id[i] == i%HORIZONTAL_LEN && tmp_hy2_horizontal_row_id[i] == i/HORIZONTAL_LEN){
            hy2_vertical_write_output[i] = 0;
            hy2_horizontal_write_output[i] = 1;
            hy2_input_write_output[i] = 0;
        }else if(hy2_combine_input_buffer_size[i] &&  tmp_hy2_input_col_id[i] == i%HORIZONTAL_LEN && tmp_hy2_input_row_id[i] == i/HORIZONTAL_LEN){
            hy2_vertical_write_output[i] = 0;
            hy2_horizontal_write_output[i] = 0;
            hy2_input_write_output[i] = 1;
        }else{
            hy2_vertical_write_output[i] = 0;
            hy2_horizontal_write_output[i] = 0;
            hy2_input_write_output[i] = 0;
        }
    }


    /* write vertical buffer combine logic */
    for(int i = 0 ; i<CORE_NUM ; i++){
        if(hy1_write_old_vertical_network[i] && hy1_write_old_vertical_network_block[i]){
            if(hy1_write_old_vertical_network[i] == 1){
                hy1_horizontal_write_vertical_buffer[i] = 1;
                hy1_input_write_vertical_buffer[i] = 0;
            }else{
                hy1_horizontal_write_vertical_buffer[i] = 0;
                hy1_input_write_vertical_buffer[i] = 1;
            }
        }else if(hy1_combine_horizontal_buffer_size[i] && !hy1_horizontal_write_output[i] && tmp_hy1_horizontal_row_id[i] != i/HORIZONTAL_LEN){
            hy1_horizontal_write_vertical_buffer[i] = 1;
            hy1_input_write_vertical_buffer[i] = 0;
        }else if(hy1_combine_input_buffer_size[i] && !hy1_input_write_output[i] && tmp_hy1_input_row_id[i] != i/HORIZONTAL_LEN && tmp_hy1_input_col_id[i] == i%HORIZONTAL_LEN){
            hy1_horizontal_write_vertical_buffer[i] = 0;
            hy1_input_write_vertical_buffer[i] = 1;
            //printf("id:%d input write vertical buffer\n",tmp_hy1_input_v_id_front);
        }else{
            hy1_horizontal_write_vertical_buffer[i] = 0;
            hy1_input_write_vertical_buffer[i] = 0;
        }

        if(hy2_write_old_vertical_network[i] && hy2_write_old_vertical_network_block[i]){
            if(hy2_write_old_vertical_network[i] == 1){
                hy2_horizontal_write_vertical_buffer[i] = 1;
                hy2_input_write_vertical_buffer[i] = 0;
            }else{
                hy2_horizontal_write_vertical_buffer[i] = 0;
                hy2_input_write_vertical_buffer[i] = 1;
            }
        }else if(hy2_combine_horizontal_buffer_size[i] && !hy2_horizontal_write_output[i] && tmp_hy2_horizontal_row_id[i] != i/HORIZONTAL_LEN){
            hy2_horizontal_write_vertical_buffer[i] = 1;
            hy2_input_write_vertical_buffer[i] = 0;
        }else if(hy2_combine_input_buffer_size[i] && !hy2_input_write_output[i] && tmp_hy2_input_row_id[i] != i/HORIZONTAL_LEN && tmp_hy2_input_col_id[i] == i%HORIZONTAL_LEN){
            hy2_horizontal_write_vertical_buffer[i] = 0;
            hy2_input_write_vertical_buffer[i] = 1;
        }else{
            hy2_horizontal_write_vertical_buffer[i] = 0;
            hy2_input_write_vertical_buffer[i] = 0;
        }
    }

    /* write horizontal buffer combine logic */
    for(int i = 0 ; i<CORE_NUM ; i++){
        if(hy1_combine_input_buffer_size[i] && !hy1_input_write_vertical_buffer[i] && !hy1_input_write_output[i] && tmp_hy1_input_col_id[i] != i%HORIZONTAL_LEN){
            hy1_input_write_horizontal_buffer[i] = 1;
        }else{
            hy1_input_write_horizontal_buffer[i] = 0;
        }

        if(hy2_combine_input_buffer_size[i] && !hy2_input_write_vertical_buffer[i] && !hy2_input_write_output[i] && tmp_hy2_input_col_id[i] != i%HORIZONTAL_LEN){
            hy2_input_write_horizontal_buffer[i] = 1;
        }else{
            hy2_input_write_horizontal_buffer[i] = 0;
        }
    }
    //input write horizontal
    for(int i = 0 ; i<CORE_NUM ; ++ i){
        for(int j = 0 ; j<HORIZONTAL_LEN ; j++){
            if(j == tmp_hy1_input_col_id[i] && hy1_input_write_horizontal_buffer[i]){
                tmp_hy1_horizontal_push_flag_in[i][j] = hy1_combine_push_flag_buffer_input_front[i];
                tmp_hy1_horizontal_v_id_in[i][j] = hy1_combine_id_buffer_input_front[i];
                tmp_hy1_horizontal_pull_first_flag_in[i][j] = hy1_combine_pull_first_flag_buffer_input_front[i];
                tmp_hy1_horizontal_v_value_in[i][j] = hy1_combine_value_buffer_input_front[i];
                tmp_hy1_horizontal_v_valid_in[i][j] = (!hy1_horizontal_stage_full[i][tmp_hy1_input_col_id[i]])? 1 : 0;
                #if(debug_8)
                if(tmp_hy1_horizontal_v_valid_in[i][j]){
                    //printf("module_8 Horizontal input write clk: %d v_id:%d v_value:%d push_flag:%d col:%d horizontal_mov -> col:%d\n",clk,tmp_hy1_horizontal_v_id_in[i][j],tmp_hy1_horizontal_v_value_in[i][j],tmp_hy1_horizontal_push_flag_in[i][j],Hyperx_Switch_ID%HORIZONTAL_LEN,tmp_hy1_input_col_id[Hyperx_Switch_ID]);
                }
                #endif
            }else{
                tmp_hy1_horizontal_push_flag_in[i][j] = 0;
                tmp_hy1_horizontal_v_id_in[i][j] = 0;
                tmp_hy1_horizontal_pull_first_flag_in[i][j] = 0;
                tmp_hy1_horizontal_v_value_in[i][j] = 0;
                tmp_hy1_horizontal_v_valid_in[i][j] = 0;
            }
            if(j == tmp_hy2_input_col_id[i] && hy2_input_write_horizontal_buffer[i]){
                tmp_hy2_horizontal_push_flag_in[i][j] = hy2_combine_push_flag_buffer_input_front[i];
                tmp_hy2_horizontal_v_id_in[i][j] = hy2_combine_id_buffer_input_front[i];
                tmp_hy2_horizontal_pull_first_flag_in[i][j] = hy2_combine_pull_first_flag_buffer_input_front[i];
                tmp_hy2_horizontal_v_value_in[i][j] = hy2_combine_value_buffer_input_front[i];
                tmp_hy2_horizontal_v_valid_in[i][j] = (!hy2_horizontal_stage_full[i][tmp_hy2_input_col_id[i]])? 1 : 0;
            }else{
                tmp_hy2_horizontal_push_flag_in[i][j] = 0;
                tmp_hy2_horizontal_v_id_in[i][j] = 0;
                tmp_hy2_horizontal_pull_first_flag_in[i][j] = 0;
                tmp_hy2_horizontal_v_value_in[i][j] = 0;
                tmp_hy2_horizontal_v_valid_in[i][j] = 0;
            }
        }
    }
    //input horizontal write vertical
    for(int i = 0 ; i<CORE_NUM ; ++ i){
        int hy1_write_vertical_id = hy1_horizontal_write_vertical_buffer[i]? tmp_hy1_horizontal_row_id[i] : tmp_hy1_input_row_id[i];
        int hy1_write_vertical_valid = !hy1_vertical_stage_full[i][hy1_write_vertical_id] && ( hy1_horizontal_write_vertical_buffer[i] || hy1_input_write_vertical_buffer[i] );
        int hy1_write_vertical_push_flag = hy1_horizontal_write_vertical_buffer[i]? hy1_combine_push_flag_buffer_horizontal_front[i] : hy1_combine_push_flag_buffer_input_front[i];
        int hy1_write_vertical_v_id = hy1_horizontal_write_vertical_buffer[i]? hy1_combine_id_buffer_horizontal_front[i] : hy1_combine_id_buffer_input_front[i];
        int hy1_write_vertical_v_value = hy1_horizontal_write_vertical_buffer[i]? hy1_combine_value_buffer_horizontal_front[i] : hy1_combine_value_buffer_input_front[i];
        int hy1_write_vertical_pull_first_flag = hy1_horizontal_write_vertical_buffer[i]? hy1_combine_pull_first_flag_buffer_horizontal_front[i] : hy1_combine_pull_first_flag_buffer_input_front[i];

        int hy2_write_vertical_id = hy2_horizontal_write_vertical_buffer[i]? tmp_hy2_horizontal_row_id[i] : tmp_hy2_input_row_id[i];
        int hy2_write_vertical_valid = !hy2_vertical_stage_full[i][hy2_write_vertical_id] && ( hy2_horizontal_write_vertical_buffer[i] || hy2_input_write_vertical_buffer[i] );
        int hy2_write_vertical_push_flag = hy2_horizontal_write_vertical_buffer[i]? hy2_combine_push_flag_buffer_horizontal_front[i] : hy2_combine_push_flag_buffer_input_front[i];
        int hy2_write_vertical_v_id = hy2_horizontal_write_vertical_buffer[i]? hy2_combine_id_buffer_horizontal_front[i] : hy2_combine_id_buffer_input_front[i];
        int hy2_write_vertical_v_value = hy2_horizontal_write_vertical_buffer[i]? hy2_combine_value_buffer_horizontal_front[i] : hy2_combine_value_buffer_input_front[i];
        int hy2_write_vertical_pull_first_flag = hy2_horizontal_write_vertical_buffer[i]? hy2_combine_pull_first_flag_buffer_horizontal_front[i] : hy2_combine_pull_first_flag_buffer_input_front[i];

        for(int j = 0 ; j<VERTICAL_LEN ; j++){
            if(j == hy1_write_vertical_id && (hy1_input_write_vertical_buffer[i] || hy1_horizontal_write_vertical_buffer[i])){
                tmp_hy1_vertical_push_flag_in[i][j] = hy1_write_vertical_push_flag;
                tmp_hy1_vertical_v_id_in[i][j] = hy1_write_vertical_v_id;
                tmp_hy1_vertical_pull_first_flag_in[i][j] = hy1_write_vertical_pull_first_flag;
                tmp_hy1_vertical_v_value_in[i][j] = hy1_write_vertical_v_value;
                tmp_hy1_vertical_v_valid_in[i][j] = hy1_write_vertical_valid;
            }else{
                tmp_hy1_vertical_push_flag_in[i][j] = 0;
                tmp_hy1_vertical_v_id_in[i][j] = 0;
                tmp_hy1_vertical_pull_first_flag_in[i][j] = 0;
                tmp_hy1_vertical_v_value_in[i][j] = 0;
                tmp_hy1_vertical_v_valid_in[i][j] = 0;
            }

            if(j == hy2_write_vertical_id && (hy2_input_write_vertical_buffer[i] ||  hy2_horizontal_write_vertical_buffer[i])){
                tmp_hy2_vertical_push_flag_in[i][j] = hy2_write_vertical_push_flag;
                tmp_hy2_vertical_v_id_in[i][j] = hy2_write_vertical_v_id;
                tmp_hy2_vertical_pull_first_flag_in[i][j] = hy2_write_vertical_pull_first_flag;
                tmp_hy2_vertical_v_value_in[i][j] = hy2_write_vertical_v_value;
                tmp_hy2_vertical_v_valid_in[i][j] = hy2_write_vertical_valid;
            }else{
                tmp_hy2_vertical_push_flag_in[i][j] = 0;
                tmp_hy2_vertical_v_id_in[i][j] = 0;
                tmp_hy2_vertical_pull_first_flag_in[i][j] = 0;
                tmp_hy2_vertical_v_value_in[i][j] = 0;
                tmp_hy2_vertical_v_valid_in[i][j] = 0;
            }
        }
    }

    //send info
    for (int i = 0; i < CORE_NUM; ++ i) {
        for(int j = 0 ; j<HORIZONTAL_LEN ; j++){
            hy1_horizontal_push_flag_in[i][j] = tmp_hy1_horizontal_push_flag_in[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
            hy1_horizontal_v_id_in[i][j] = tmp_hy1_horizontal_v_id_in[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
            hy1_horizontal_v_value_in[i][j] = tmp_hy1_horizontal_v_value_in[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
            hy1_horizontal_pull_first_flag_in[i][j] = tmp_hy1_horizontal_pull_first_flag_in[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
            hy1_horizontal_v_valid_in[i][j] = tmp_hy1_horizontal_v_valid_in[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
        }

        for(int j = 0 ; j<VERTICAL_LEN; j++){
            hy1_vertical_push_flag_in[i][j] = tmp_hy1_vertical_push_flag_in[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
            hy1_vertical_v_id_in[i][j] = tmp_hy1_vertical_v_id_in[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
            hy1_vertical_v_value_in[i][j] = tmp_hy1_vertical_v_value_in[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
            hy1_vertical_pull_first_flag_in[i][j] = tmp_hy1_vertical_pull_first_flag_in[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
            hy1_vertical_v_valid_in[i][j] = tmp_hy1_vertical_v_valid_in[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
        }

        for(int j = 0 ; j<HORIZONTAL_LEN ; j++){
            hy2_horizontal_push_flag_in[i][j] = tmp_hy2_horizontal_push_flag_in[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
            hy2_horizontal_v_id_in[i][j] = tmp_hy2_horizontal_v_id_in[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
            hy2_horizontal_v_value_in[i][j] = tmp_hy2_horizontal_v_value_in[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
            hy2_horizontal_pull_first_flag_in[i][j] = tmp_hy2_horizontal_pull_first_flag_in[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
            hy2_horizontal_v_valid_in[i][j] = tmp_hy2_horizontal_v_valid_in[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
        }

        for(int j = 0 ; j<VERTICAL_LEN; j++){
            hy2_vertical_push_flag_in[i][j] = tmp_hy2_vertical_push_flag_in[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
            hy2_vertical_v_id_in[i][j] = tmp_hy2_vertical_v_id_in[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
            hy2_vertical_v_value_in[i][j] = tmp_hy2_vertical_v_value_in[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
            hy2_vertical_pull_first_flag_in[i][j] = tmp_hy2_vertical_pull_first_flag_in[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
            hy2_vertical_v_valid_in[i][j] = tmp_hy2_vertical_v_valid_in[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
        }

    }

    /*vertical buffer receive*/

    /* horizontal buffer receive */
    for(int i = 0 ; i<CORE_NUM ; i++){
        for(int j = 0 ; j<HORIZONTAL_LEN ; j++){
            tmp_hy1_horizontal_v_valid_in_arr[i][j] = tmp_hy1_horizontal_v_valid_in[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
            tmp_hy1_horizontal_v_valid_index_arr[i][j] = j;
        }
        int t = HORIZONTAL_LEN;
        while(t>1){
            t = t/2;
            for(int k = 0 ; k<t ; k++){
                tmp_hy1_horizontal_v_valid_index_arr[i][k] = (tmp_hy1_horizontal_v_valid_in_arr[i][2 * k]) ? tmp_hy1_horizontal_v_valid_index_arr[i][2 * k] : tmp_hy1_horizontal_v_valid_index_arr[i][2 * k + 1];//未被访问则往前传，优先传index小的
                tmp_hy1_horizontal_v_valid_in_arr[i][k] = tmp_hy1_horizontal_v_valid_in_arr[i][2 * k] | tmp_hy1_horizontal_v_valid_in_arr[i][2 * k + 1];
            }
        }
        tmp_hy1_horizontal_v_valid_index_min[i] = tmp_hy1_horizontal_v_valid_index_arr[i][0];
        tmp_hy1_horizontal_v_valid_in_min[i] = tmp_hy1_horizontal_v_valid_in_arr[i][0];
        if (tmp_hy1_horizontal_v_valid_in_min[i]) {
            for(int j = 0 ; j<HORIZONTAL_LEN ; j++){
                if(j == tmp_hy1_horizontal_v_valid_index_min[i] || !tmp_hy1_horizontal_v_valid_in[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN]){
                    tmp_hy1_horizontal_block[i][j] = 0;
                }else{
                    tmp_hy1_horizontal_block[i][j] = 1;
                }
            }
        }else{
            for(int j = 0 ; j<HORIZONTAL_LEN ; j++){
                tmp_hy1_horizontal_block[i][j] = 0;
            }
        } 


        for(int j = 0 ; j<VERTICAL_LEN ; j++){
            tmp_hy1_vertical_v_valid_in_arr[i][j] = tmp_hy1_vertical_v_valid_in[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
            tmp_hy1_vertical_v_valid_index_arr[i][j] = j;
        }
        // t = VERTICAL_LEN;
        // while(t>1){
        //     t = t/2;
        //     for(int k = 0 ; k<t ; k++){
        //         tmp_hy1_vertical_v_valid_index_arr[i][k] = (tmp_hy1_vertical_v_valid_in_arr[i][2 * k]) ? tmp_hy1_vertical_v_valid_index_arr[i][2 * k] : tmp_hy1_vertical_v_valid_index_arr[i][2 * k + 1];//未被访问则往前传，优先传index小的
        //         tmp_hy1_vertical_v_valid_in_arr[i][k] = tmp_hy1_vertical_v_valid_in_arr[i][2 * k] | tmp_hy1_vertical_v_valid_in_arr[i][2 * k + 1];
        //     }
        // }
        // tmp_hy1_vertical_v_valid_index_min[i] = tmp_hy1_vertical_v_valid_index_arr[i][0];
        // tmp_hy1_vertical_v_valid_in_min[i] = tmp_hy1_vertical_v_valid_in_arr[i][0];
        int min_index = VERTICAL_LEN ;
        for(int j = 0 ; j<VERTICAL_LEN ; j++){
            if(tmp_hy1_vertical_v_valid_in_arr[i][j]){
                min_index = j;
            }
        }
        tmp_hy1_vertical_v_valid_index_min[i] = min_index;
        tmp_hy1_vertical_v_valid_in_min[i] = ( min_index != VERTICAL_LEN );
        if (tmp_hy1_vertical_v_valid_in_min[i]) {
            for(int j = 0 ; j<VERTICAL_LEN ; j++){
                if(j == tmp_hy1_vertical_v_valid_index_min[i] || !tmp_hy1_vertical_v_valid_in[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN]){
                    tmp_hy1_vertical_block[i][j] = 0;
                }else{
                    tmp_hy1_vertical_block[i][j] = 1;
                }
            }
        }else{
            for(int j = 0 ; j<VERTICAL_LEN ; j++){
                tmp_hy1_vertical_block[i][j] = 0;
            }
        } 
        
    }

    for(int i = 0 ; i<CORE_NUM ; i++){
        for(int j = 0 ; j<HORIZONTAL_LEN ; j++){
            tmp_hy2_horizontal_v_valid_in_arr[i][j] = tmp_hy2_horizontal_v_valid_in[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN];
            tmp_hy2_horizontal_v_valid_index_arr[i][j] = j;
        }
        int t = HORIZONTAL_LEN;
        while(t>1){
            t = t/2;
            for(int k = 0 ; k<t ; k++){
                tmp_hy2_horizontal_v_valid_index_arr[i][k] = (tmp_hy2_horizontal_v_valid_in_arr[i][2 * k]) ? tmp_hy2_horizontal_v_valid_index_arr[i][2 * k] : tmp_hy2_horizontal_v_valid_index_arr[i][2 * k + 1];//未被访问则往前传，优先传index小的
                tmp_hy2_horizontal_v_valid_in_arr[i][k] = tmp_hy2_horizontal_v_valid_in_arr[i][2 * k] | tmp_hy2_horizontal_v_valid_in_arr[i][2 * k + 1];
            }
        }
        tmp_hy2_horizontal_v_valid_index_min[i] = tmp_hy2_horizontal_v_valid_index_arr[i][0];
        tmp_hy2_horizontal_v_valid_in_min[i] = tmp_hy2_horizontal_v_valid_in_arr[i][0];
        if (tmp_hy2_horizontal_v_valid_in_min[i]) {
            for(int j = 0 ; j<HORIZONTAL_LEN ; j++){
                if(j == tmp_hy2_horizontal_v_valid_index_min[i] || !tmp_hy2_horizontal_v_valid_in[j+i/HORIZONTAL_LEN*HORIZONTAL_LEN][i%HORIZONTAL_LEN]){
                    tmp_hy2_horizontal_block[i][j] = 0;
                }else{
                    tmp_hy2_horizontal_block[i][j] = 1;
                }
            }
        }else{
            for(int j = 0 ; j<HORIZONTAL_LEN ; j++){
                tmp_hy2_horizontal_block[i][j] = 0;
            }
        } 

        for(int j = 0 ; j<VERTICAL_LEN ; j++){
            tmp_hy2_vertical_v_valid_in_arr[i][j] = tmp_hy2_vertical_v_valid_in[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN];
            tmp_hy2_vertical_v_valid_index_arr[i][j] = j;
        }

        t = VERTICAL_LEN;
        // while(t>1){
        //     t = t/2;
        //     for(int k = 0 ; k<t ; k++){
        //         tmp_hy2_vertical_v_valid_index_arr[i][k] = (tmp_hy2_vertical_v_valid_in_arr[i][2 * k]) ? tmp_hy2_vertical_v_valid_index_arr[i][2 * k] : tmp_hy2_vertical_v_valid_index_arr[i][2 * k + 1];//未被访问则往前传，优先传index小的
        //         tmp_hy2_vertical_v_valid_in_arr[i][k] = tmp_hy2_vertical_v_valid_in_arr[i][2 * k] | tmp_hy2_vertical_v_valid_in_arr[i][2 * k + 1];
        //     }
        // }
        // 对于不是2的次方的数字 二分电路比较过程中会忽略一些结果
        int min_index = VERTICAL_LEN ;
        for(int j = 0 ; j<VERTICAL_LEN ; j++){
            if(tmp_hy2_vertical_v_valid_in_arr[i][j]){
                min_index = j;
            }
        }
        tmp_hy2_vertical_v_valid_index_min[i] = min_index;
        tmp_hy2_vertical_v_valid_in_min[i] = ( min_index != VERTICAL_LEN );
        // tmp_hy2_vertical_v_valid_index_min[i] = tmp_hy2_vertical_v_valid_index_arr[i][0];
        // tmp_hy2_vertical_v_valid_in_min[i] = tmp_hy2_vertical_v_valid_in_arr[i][0];
        if (tmp_hy2_vertical_v_valid_in_min[i]) {
            for(int j = 0 ; j<VERTICAL_LEN ; j++){
                if(j == tmp_hy2_vertical_v_valid_index_min[i] || !tmp_hy2_vertical_v_valid_in[j*HORIZONTAL_LEN+i%HORIZONTAL_LEN][i/HORIZONTAL_LEN]){
                    tmp_hy2_vertical_block[i][j] = 0;
                }else{
                    tmp_hy2_vertical_block[i][j] = 1;
                }
            }
        }else{
            for(int j = 0 ; j<VERTICAL_LEN ; j++){
                tmp_hy2_vertical_block[i][j] = 0;
            }
        } 
        
    }

}

void Omega_Network_Global_Signal(int Front_Global_Iteration_ID[], Powerlaw_Vid_Set Front_Global_Powerlaw_V_ID[], Powerlaw_Vvisit_Set Front_Global_Powerlaw_V_Visit[],

                                int *Global_Iteration_ID, Powerlaw_Vid_Set *Global_Powerlaw_V_ID, Powerlaw_Vvisit_Set *Global_Powerlaw_V_Visit) {
    static int local_Global_Iteration_ID[CORE_NUM];
    static Powerlaw_Vid_Set local_Global_Powerlaw_V_ID[CORE_NUM];
    static Powerlaw_Vvisit_Set local_Global_Powerlaw_V_Visit[CORE_NUM];

    for (int i = 0; i < CORE_NUM; i++) {
        if (rst_rd) {
            Global_Iteration_ID[i] = 0;
            for (int j = 0; j < POWER_LAW_V_NUM; j++) {
                Global_Powerlaw_V_ID[i].v[j] = VERTEX_NOT_READ[j];
                Global_Powerlaw_V_Visit[i].visit[j] = false;
            }
        } else {
            Global_Iteration_ID[i] = local_Global_Iteration_ID[i];
            Global_Powerlaw_V_ID[i].set(local_Global_Powerlaw_V_ID[i]);
            Global_Powerlaw_V_Visit[i].set(local_Global_Powerlaw_V_Visit[i]);
        }

        if (rst_rd) {
            local_Global_Iteration_ID[i] = 0;
            for (int j = 0; j < POWER_LAW_V_NUM; j++) {
                local_Global_Powerlaw_V_ID[i].v[j] = VERTEX_NOT_READ[j];
                local_Global_Powerlaw_V_Visit[i].visit[j] = false;
            }
        } else {
            local_Global_Iteration_ID[i] = Front_Global_Iteration_ID[i];
            for (int j = 0; j < POWER_LAW_V_NUM; j++) {
                local_Global_Powerlaw_V_ID[i].set(Front_Global_Powerlaw_V_ID[i]);
                local_Global_Powerlaw_V_Visit[i].set(Front_Global_Powerlaw_V_Visit[i]);
            }
        }
    }
}

void Hyperx_Network_1_Single(int Hyperx_Switch_ID,
                            int Push_Flag, int V_ID_In, int V_Value_In, int Pull_First_Flag, int V_Valid_In, 
                            int Iteration_End_In, int Iteration_End_DValid_In,int Next_Stage_Full, 

                            int *Hy_Vertical_Push_Flag_Out, int *Hy_Vertical_V_ID_Out, int *Hy_Vertical_V_Value_Out, int *Hy_Vertical_Pull_First_Flag, 
                            int *Hy_Vertical_V_Valid_Out,
                            int *Vertical_Stage_Full,int* Vertical_Block,

                            int *Hy_Horizontal_Push_Flag_Out, int *Hy_Horizontal_V_ID_Out, int *Hy_Horizontal_V_Value_Out, int *Hy_Horizontal_Pull_First_Flag, 
                            int *Hy_Horizontal_V_Valid_Out, 
                            int *Horizontal_Stage_Full,int *Horizontal_Block,

                            int *Hy_Push_Flag, int *Hy_Update_V_ID, int *Hy_Update_V_Value, int *Hy_Pull_First_Flag, int *Hy_Update_V_DValid, 
                            int *Hy_Iteration_End, int *Hy_Iteration_End_DValid, int *Stage_Full_Hy,

                            int* combine_input_buffer_size,int *combine_push_flag_buffer_input_front,int *combine_id_buffer_input_front,int *combine_value_buffer_input_front,int * combine_pull_first_flag_buffer_input_front,            
                            int* combine_horizontal_buffer_size,int *combine_push_flag_buffer_horizontal_front,int *combine_id_buffer_horizontal_front,int *combine_value_buffer_horizontal_front,int * combine_pull_first_flag_buffer_horizontal_front,
                            int* combine_vertical_buffer_size,int *combine_push_flag_buffer_vertical_front,int *combine_id_buffer_vertical_front,int *combine_value_buffer_vertical_front,int * combine_pull_first_flag_buffer_vertical_front){

    static queue<int> push_flag_buffer_vertical[CORE_NUM], id_buffer_vertical[CORE_NUM], value_buffer_vertical[CORE_NUM], pull_first_flag_buffer_vertical[CORE_NUM];
    static queue<int> push_flag_buffer_horizontal[CORE_NUM], id_buffer_horizontal[CORE_NUM], value_buffer_horizontal[CORE_NUM], pull_first_flag_buffer_horizontal[CORE_NUM];
    static queue<int> push_flag_buffer_input[CORE_NUM], id_buffer_input[CORE_NUM], value_buffer_input[CORE_NUM], pull_first_flag_buffer_input[CORE_NUM];
    static int iteration_end_clk1[CORE_NUM], iteration_end_dvalid_clk1[CORE_NUM];
    static int vertical_buffer_empty[CORE_NUM], horizontal_buffer_empty[CORE_NUM], input_buffer_empty[CORE_NUM];
    static int vertical_buffer_full[CORE_NUM], horizontal_buffer_full[CORE_NUM], input_buffer_full[CORE_NUM];
   

    vertical_buffer_empty[Hyperx_Switch_ID] = (id_buffer_vertical[Hyperx_Switch_ID].size() == 0);
    horizontal_buffer_empty[Hyperx_Switch_ID] = (id_buffer_horizontal[Hyperx_Switch_ID].size() == 0);
    input_buffer_empty[Hyperx_Switch_ID] = (id_buffer_input[Hyperx_Switch_ID].size() == 0);

    #if(debug_8_0)
    /*
        if(clk == 832 && Hyperx_Switch_ID == 0){
            for(int i = 0 ; i<16384 ; i++){
                if(mp_hy1[i]){
                    printf("clk:%d hy1 %d still has %d in the network\n",clk,i,mp_hy1[i]);
                }
            }
            for(int i = 0 ; i<CORE_NUM ; i++){
                if(id_buffer_horizontal[i].size()){
                    printf("clk:%d hy1 buffer horizontal %d still has points %d\n",clk,i,id_buffer_horizontal[i].size());
                }
            }
            for(int i = 0 ; i<CORE_NUM ; i++){
                if(id_buffer_vertical[i].size()){
                    printf("clk:%d hy1 buffer vertical %d still has points %d\n",clk,i,id_buffer_vertical[i].size());
                }
            }
        }
        */
    #endif


    if(!rst_rd && iteration_end_clk1[Hyperx_Switch_ID] && iteration_end_dvalid_clk1[Hyperx_Switch_ID] && input_buffer_empty[Hyperx_Switch_ID] && horizontal_buffer_empty[Hyperx_Switch_ID] && vertical_buffer_empty[Hyperx_Switch_ID]){
        Hy_Iteration_End[Hyperx_Switch_ID] = 1;
        Hy_Iteration_End_DValid[Hyperx_Switch_ID] = 1;
    }else{
        Hy_Iteration_End[Hyperx_Switch_ID] = 0;
        Hy_Iteration_End_DValid[Hyperx_Switch_ID] = 0;
    }
    
    if(!rst_rd && Iteration_End_In && Iteration_End_DValid_In  && input_buffer_empty[Hyperx_Switch_ID]  && vertical_buffer_empty[Hyperx_Switch_ID] && horizontal_buffer_empty[Hyperx_Switch_ID]){
        iteration_end_clk1[Hyperx_Switch_ID] = 1;
        iteration_end_dvalid_clk1[Hyperx_Switch_ID] = 1;
    }else{
        iteration_end_clk1[Hyperx_Switch_ID] = 0;
        iteration_end_dvalid_clk1[Hyperx_Switch_ID] = 0;
    }

    /*input horizontal_mov*/
    if (rst_rd) {
    }else {
        if(!hy1_horizontal_stage_full[Hyperx_Switch_ID][tmp_hy1_input_col_id[Hyperx_Switch_ID]] && hy1_input_write_horizontal_buffer[Hyperx_Switch_ID] && !hy1_horizontal_block[Hyperx_Switch_ID][tmp_hy1_input_col_id[Hyperx_Switch_ID]]){
            #if(debug_8)
            if(id_buffer_input[Hyperx_Switch_ID].front() == 1431)
                printf("module_8 Horizontal input Send clk: %d v_id:%d v_value:%d push_flag:%d col:%d horizontal_mov -> col:%d\n",clk,id_buffer_input[Hyperx_Switch_ID].front(),value_buffer_input[Hyperx_Switch_ID].front(),push_flag_buffer_input[Hyperx_Switch_ID].front(),Hyperx_Switch_ID%HORIZONTAL_LEN,tmp_hy1_input_col_id[Hyperx_Switch_ID]);
            #endif
            push_flag_buffer_input[Hyperx_Switch_ID].pop();
            id_buffer_input[Hyperx_Switch_ID].pop();
            value_buffer_input[Hyperx_Switch_ID].pop();
            pull_first_flag_buffer_input[Hyperx_Switch_ID].pop();
        }
           
    }



    /*input/horizontal vertical_mov*/
     if (rst_rd) {
    }else {
        if(!hy1_vertical_stage_full[Hyperx_Switch_ID][tmp_hy1_horizontal_row_id[Hyperx_Switch_ID]] && hy1_horizontal_write_vertical_buffer[Hyperx_Switch_ID] && !hy1_vertical_block[Hyperx_Switch_ID][tmp_hy1_horizontal_row_id[Hyperx_Switch_ID]]){
            #if(debug_8)
            if(id_buffer_horizontal[Hyperx_Switch_ID].front() == 1431)
                printf("module_8 Vertical horizontal Send clk: %d v_id:%d v_value:%d push_flag:%d row:%d vertical_mov -> row:%d\n",clk,id_buffer_horizontal[Hyperx_Switch_ID].front(),value_buffer_horizontal[Hyperx_Switch_ID].front(),push_flag_buffer_horizontal[Hyperx_Switch_ID].front(),Hyperx_Switch_ID/HORIZONTAL_LEN,tmp_hy1_horizontal_row_id[Hyperx_Switch_ID]);
            #endif
            push_flag_buffer_horizontal[Hyperx_Switch_ID].pop();
            id_buffer_horizontal[Hyperx_Switch_ID].pop();
            value_buffer_horizontal[Hyperx_Switch_ID].pop();
            pull_first_flag_buffer_horizontal[Hyperx_Switch_ID].pop();
        }

        if(!hy1_vertical_stage_full[Hyperx_Switch_ID][tmp_hy1_input_row_id[Hyperx_Switch_ID]] && hy1_input_write_vertical_buffer[Hyperx_Switch_ID] && !hy1_vertical_block[Hyperx_Switch_ID][tmp_hy1_input_row_id[Hyperx_Switch_ID]]){
            #if(debug_8)
            if(id_buffer_input[Hyperx_Switch_ID].front() == 1431)
                printf("module_8 Vertical input Send clk: %d v_id:%d v_value:%d push_flag:%d row:%d vertical_mov -> row:%d\n",clk,id_buffer_input[Hyperx_Switch_ID].front(),value_buffer_input[Hyperx_Switch_ID].front(),push_flag_buffer_input[Hyperx_Switch_ID].front(),Hyperx_Switch_ID/HORIZONTAL_LEN,tmp_hy1_input_row_id[Hyperx_Switch_ID]);
            #endif
            push_flag_buffer_input[Hyperx_Switch_ID].pop();
            id_buffer_input[Hyperx_Switch_ID].pop();
            value_buffer_input[Hyperx_Switch_ID].pop();
            pull_first_flag_buffer_input[Hyperx_Switch_ID].pop();
        }
    }

    if(rst_rd){
        hy1_write_old_vertical_network[Hyperx_Switch_ID] = 0;
        hy1_write_old_vertical_network_block[Hyperx_Switch_ID] = 0;
    }else{
        hy1_write_old_vertical_network[Hyperx_Switch_ID] = hy1_horizontal_write_vertical_buffer[Hyperx_Switch_ID] ? 1 : (hy1_input_write_vertical_buffer[Hyperx_Switch_ID] ? 2 : 0) ;
        hy1_write_old_vertical_network_block[Hyperx_Switch_ID] = hy1_vertical_block[Hyperx_Switch_ID][hy1_horizontal_write_vertical_buffer[Hyperx_Switch_ID] ? tmp_hy1_horizontal_row_id[Hyperx_Switch_ID] : (hy1_input_write_vertical_buffer[Hyperx_Switch_ID] ? tmp_hy1_input_row_id[Hyperx_Switch_ID] : 0)];
    }

    /*input/horizontal/vertical output_mov*/
    if(Next_Stage_Full){
        Hy_Push_Flag[Hyperx_Switch_ID] = 0;
        Hy_Update_V_ID[Hyperx_Switch_ID] = 0;
        Hy_Update_V_Value[Hyperx_Switch_ID] = 0;
        Hy_Pull_First_Flag[Hyperx_Switch_ID] = 0;
        Hy_Update_V_DValid[Hyperx_Switch_ID] = 0;
    }else{
        if(hy1_vertical_write_output[Hyperx_Switch_ID]){
            Hy_Push_Flag[Hyperx_Switch_ID] = push_flag_buffer_vertical[Hyperx_Switch_ID].front();
            Hy_Update_V_ID[Hyperx_Switch_ID] = id_buffer_vertical[Hyperx_Switch_ID].front();
            Hy_Update_V_Value[Hyperx_Switch_ID] = value_buffer_vertical[Hyperx_Switch_ID].front();
            Hy_Pull_First_Flag[Hyperx_Switch_ID] = pull_first_flag_buffer_vertical[Hyperx_Switch_ID].front();
            Hy_Update_V_DValid[Hyperx_Switch_ID] = 1;

            push_flag_buffer_vertical[Hyperx_Switch_ID].pop();
            id_buffer_vertical[Hyperx_Switch_ID].pop();
            value_buffer_vertical[Hyperx_Switch_ID].pop();
            pull_first_flag_buffer_vertical[Hyperx_Switch_ID].pop();
        }else if(hy1_horizontal_write_output[Hyperx_Switch_ID]){
            Hy_Push_Flag[Hyperx_Switch_ID] = push_flag_buffer_horizontal[Hyperx_Switch_ID].front();
            Hy_Update_V_ID[Hyperx_Switch_ID] = id_buffer_horizontal[Hyperx_Switch_ID].front();
            Hy_Update_V_Value[Hyperx_Switch_ID] = value_buffer_horizontal[Hyperx_Switch_ID].front();
            Hy_Pull_First_Flag[Hyperx_Switch_ID] = pull_first_flag_buffer_horizontal[Hyperx_Switch_ID].front();
            Hy_Update_V_DValid[Hyperx_Switch_ID] = 1;

            push_flag_buffer_horizontal[Hyperx_Switch_ID].pop();
            id_buffer_horizontal[Hyperx_Switch_ID].pop();
            value_buffer_horizontal[Hyperx_Switch_ID].pop();
            pull_first_flag_buffer_horizontal[Hyperx_Switch_ID].pop();
        }else if(hy1_input_write_output[Hyperx_Switch_ID]){
            Hy_Push_Flag[Hyperx_Switch_ID] = push_flag_buffer_input[Hyperx_Switch_ID].front();
            Hy_Update_V_ID[Hyperx_Switch_ID] = id_buffer_input[Hyperx_Switch_ID].front();
            Hy_Update_V_Value[Hyperx_Switch_ID] = value_buffer_input[Hyperx_Switch_ID].front();
            Hy_Pull_First_Flag[Hyperx_Switch_ID] = pull_first_flag_buffer_input[Hyperx_Switch_ID].front();
            Hy_Update_V_DValid[Hyperx_Switch_ID] = 1;

            push_flag_buffer_input[Hyperx_Switch_ID].pop();
            id_buffer_input[Hyperx_Switch_ID].pop();
            value_buffer_input[Hyperx_Switch_ID].pop();
            pull_first_flag_buffer_input[Hyperx_Switch_ID].pop();
        }else{
            Hy_Push_Flag[Hyperx_Switch_ID] = 0;
            Hy_Update_V_ID[Hyperx_Switch_ID] = 0;
            Hy_Update_V_Value[Hyperx_Switch_ID] = 0;
            Hy_Pull_First_Flag[Hyperx_Switch_ID] = 0;
            Hy_Update_V_DValid[Hyperx_Switch_ID] = 0;
        }
    }
    #if(debug_8)
    if(Hy_Update_V_DValid[Hyperx_Switch_ID] && Hy_Update_V_ID[Hyperx_Switch_ID] == 1431){
        printf("output clk:%d channel:%d id:%d push_flag:%d\n",clk,Hyperx_Switch_ID,Hy_Update_V_ID[Hyperx_Switch_ID],Hy_Push_Flag[Hyperx_Switch_ID]);
    }
    #endif


    /*vertical_buffer_Receive*/
    if (rst_rd) {
    }else if(tmp_hy1_vertical_v_valid_in_min[Hyperx_Switch_ID]){//这个标记是否会增加依赖链路
        push_flag_buffer_vertical[Hyperx_Switch_ID].push(hy1_vertical_push_flag_in[Hyperx_Switch_ID][tmp_hy1_vertical_v_valid_index_min[Hyperx_Switch_ID]]);
        id_buffer_vertical[Hyperx_Switch_ID].push(hy1_vertical_v_id_in[Hyperx_Switch_ID][tmp_hy1_vertical_v_valid_index_min[Hyperx_Switch_ID]]);
        value_buffer_vertical[Hyperx_Switch_ID].push(hy1_vertical_v_value_in[Hyperx_Switch_ID][tmp_hy1_vertical_v_valid_index_min[Hyperx_Switch_ID]]);
        pull_first_flag_buffer_vertical[Hyperx_Switch_ID].push(hy1_vertical_pull_first_flag_in[Hyperx_Switch_ID][tmp_hy1_vertical_v_valid_index_min[Hyperx_Switch_ID]]);
        #if(debug_8)
        if(hy1_vertical_v_id_in[Hyperx_Switch_ID][tmp_hy1_vertical_v_valid_index_min[Hyperx_Switch_ID]] == 1431)
            printf("module_8 Vertical Receive clk: %d v_id:%d v_value:%d push_flag:%d channel:%d row:%d vertical_mov -> row:%d\n",clk,hy1_vertical_v_id_in[Hyperx_Switch_ID][tmp_hy1_vertical_v_valid_index_min[Hyperx_Switch_ID]],hy1_vertical_v_value_in[Hyperx_Switch_ID][tmp_hy1_vertical_v_valid_index_min[Hyperx_Switch_ID]],hy1_vertical_push_flag_in[Hyperx_Switch_ID][tmp_hy1_vertical_v_valid_index_min[Hyperx_Switch_ID]],Hyperx_Switch_ID,tmp_hy1_vertical_v_valid_index_min[Hyperx_Switch_ID],Hyperx_Switch_ID%CORE_NUM/HORIZONTAL_LEN);
        #endif
    }

    /*horizontal_buffer_receive*/
    if (rst_rd) {
    }else if(tmp_hy1_horizontal_v_valid_in_min[Hyperx_Switch_ID]){//这个标记是否会增加依赖链路
        push_flag_buffer_horizontal[Hyperx_Switch_ID].push(hy1_horizontal_push_flag_in[Hyperx_Switch_ID][tmp_hy1_horizontal_v_valid_index_min[Hyperx_Switch_ID]]);
        id_buffer_horizontal[Hyperx_Switch_ID].push(hy1_horizontal_v_id_in[Hyperx_Switch_ID][tmp_hy1_horizontal_v_valid_index_min[Hyperx_Switch_ID]]);
        value_buffer_horizontal[Hyperx_Switch_ID].push(hy1_horizontal_v_value_in[Hyperx_Switch_ID][tmp_hy1_horizontal_v_valid_index_min[Hyperx_Switch_ID]]);
        pull_first_flag_buffer_horizontal[Hyperx_Switch_ID].push(hy1_horizontal_pull_first_flag_in[Hyperx_Switch_ID][tmp_hy1_horizontal_v_valid_index_min[Hyperx_Switch_ID]]);
        #if(debug_8)
        if(hy1_horizontal_v_id_in[Hyperx_Switch_ID][tmp_hy1_horizontal_v_valid_index_min[Hyperx_Switch_ID]] == 1431)
            printf("module_8 Horizontal Receive clk: %d v_id:%d v_value:%d push_flag:%d channel:%d col:%d horizontal_mov -> col:%d\n",clk,hy1_horizontal_v_id_in[Hyperx_Switch_ID][tmp_hy1_horizontal_v_valid_index_min[Hyperx_Switch_ID]],hy1_horizontal_v_value_in[Hyperx_Switch_ID][tmp_hy1_horizontal_v_valid_index_min[Hyperx_Switch_ID]],hy1_horizontal_push_flag_in[Hyperx_Switch_ID][tmp_hy1_horizontal_v_valid_index_min[Hyperx_Switch_ID]],Hyperx_Switch_ID,tmp_hy1_horizontal_v_valid_index_min[Hyperx_Switch_ID],Hyperx_Switch_ID%CORE_NUM%HORIZONTAL_LEN);
        #endif
    }

    /*in->vertical/horizontal_buffer*/
    if (rst_rd) {
        /*buffer initilization*/
    }
    else {
        if(V_Valid_In){
            push_flag_buffer_input[Hyperx_Switch_ID].push(Push_Flag);
            id_buffer_input[Hyperx_Switch_ID].push(V_ID_In);
            value_buffer_input[Hyperx_Switch_ID].push(V_Value_In);
            pull_first_flag_buffer_input[Hyperx_Switch_ID].push(Pull_First_Flag);
            #if(debug_8)
            if(V_ID_In == 1431)
            printf("clk:%d v_id:%d push_flag:%d channel:%d in\n",clk,V_ID_In,Push_Flag,Hyperx_Switch_ID);
            #endif
        }
    }

    #if(debug_8_0)
    if(rst_rd){
        if(!rst_hy1){
            for(int i = 0 ; i< 1048576 ; i++){
                mp_hy1[i] = 0;
            }
            rst_hy1 = 1;
        }
    }
    if(V_Valid_In){
        mp_hy1[V_ID_In] ++;
    }
    #endif

    #if(debug_8_0)
    if(Hy_Update_V_DValid[Hyperx_Switch_ID]){
        mp_hy1[Hy_Update_V_ID[Hyperx_Switch_ID]] --;
        if(mp_hy1[Hy_Update_V_ID[Hyperx_Switch_ID]] < 0){
            printf("clk:%d,hy1 produce unknown point %d\n",clk,Hy_Update_V_ID[Hyperx_Switch_ID]);
        }
        if(Hy_Push_Flag[Hyperx_Switch_ID]){
            if(Hy_Update_V_ID[Hyperx_Switch_ID] % CORE_NUM != Hyperx_Switch_ID){
                printf("clk:%d,hy1 error channel for id:%d in channel:%d\n",clk,Hy_Update_V_ID[Hyperx_Switch_ID],Hyperx_Switch_ID);
            }
        }else{
            if(Hy_Update_V_Value[Hyperx_Switch_ID] % CORE_NUM != Hyperx_Switch_ID){
                printf("clk:%d,hy1 error channel for id:%d in channel:%d\n",clk,Hy_Update_V_Value[Hyperx_Switch_ID],Hyperx_Switch_ID);
            }
        }
    }
    #endif

    vertical_buffer_full[Hyperx_Switch_ID] = (id_buffer_vertical[Hyperx_Switch_ID].size() >= FIFO_SIZE);
    horizontal_buffer_full[Hyperx_Switch_ID] = (id_buffer_horizontal[Hyperx_Switch_ID].size() >= FIFO_SIZE);
    input_buffer_full[Hyperx_Switch_ID] = (id_buffer_input[Hyperx_Switch_ID].size() >= FIFO_SIZE);
    for(int i = 0 ; i< VERTICAL_LEN ; i++){
        Vertical_Stage_Full[i] = vertical_buffer_full[Hyperx_Switch_ID];
    }
    for(int i = 0 ; i< HORIZONTAL_LEN ; i++){
        Horizontal_Stage_Full[i] = horizontal_buffer_full[Hyperx_Switch_ID];
    }

    Stage_Full_Hy[Hyperx_Switch_ID] = input_buffer_full[Hyperx_Switch_ID] ;

    *combine_vertical_buffer_size = id_buffer_vertical[Hyperx_Switch_ID].size();
    *combine_id_buffer_vertical_front = id_buffer_vertical[Hyperx_Switch_ID].front();
    *combine_push_flag_buffer_vertical_front = push_flag_buffer_vertical[Hyperx_Switch_ID].front();
    *combine_value_buffer_vertical_front = value_buffer_vertical[Hyperx_Switch_ID].front();
    *combine_pull_first_flag_buffer_vertical_front = pull_first_flag_buffer_vertical[Hyperx_Switch_ID].front();
    
    *combine_horizontal_buffer_size = id_buffer_horizontal[Hyperx_Switch_ID].size();
    *combine_id_buffer_horizontal_front = id_buffer_horizontal[Hyperx_Switch_ID].front();
    *combine_push_flag_buffer_horizontal_front = push_flag_buffer_horizontal[Hyperx_Switch_ID].front();
    *combine_value_buffer_horizontal_front = value_buffer_horizontal[Hyperx_Switch_ID].front();
    *combine_pull_first_flag_buffer_horizontal_front = pull_first_flag_buffer_horizontal[Hyperx_Switch_ID].front();
    
    *combine_input_buffer_size = id_buffer_input[Hyperx_Switch_ID].size();
    *combine_id_buffer_input_front = id_buffer_input[Hyperx_Switch_ID].front();
    *combine_push_flag_buffer_input_front = push_flag_buffer_input[Hyperx_Switch_ID].front();
    *combine_value_buffer_input_front = value_buffer_input[Hyperx_Switch_ID].front();
    *combine_pull_first_flag_buffer_input_front = pull_first_flag_buffer_input[Hyperx_Switch_ID].front();

}


void Hyperx_Network_2_Single(int Hyperx_Switch_ID,
                            int Push_Flag, int V_ID_In, int V_Value_In, int Pull_First_Flag, int V_Valid_In, 
                            int Iteration_End_In, int Iteration_End_DValid_In,int Next_Stage_Full, 

                            int *Hy_Vertical_Push_Flag_Out, int *Hy_Vertical_V_ID_Out, int *Hy_Vertical_V_Value_Out, int *Hy_Vertical_Pull_First_Flag, 
                            int *Hy_Vertical_V_Valid_Out,
                            int *Vertical_Stage_Full,int* Vertical_Block,

                            int *Hy_Horizontal_Push_Flag_Out, int *Hy_Horizontal_V_ID_Out, int *Hy_Horizontal_V_Value_Out, int *Hy_Horizontal_Pull_First_Flag, 
                            int *Hy_Horizontal_V_Valid_Out, 
                            int *Horizontal_Stage_Full,int *Horizontal_Block,

                            int *Hy_Push_Flag, int *Hy_Update_V_ID, int *Hy_Update_V_Value, int *Hy_Pull_First_Flag, int *Hy_Update_V_DValid, 
                            int *Hy_Iteration_End, int *Hy_Iteration_End_DValid, int *Stage_Full_Hy,

                            int* combine_input_buffer_size,int *combine_push_flag_buffer_input_front,int *combine_id_buffer_input_front,int *combine_value_buffer_input_front, int * combine_pull_first_flag_buffer_input_front,           
                            int* combine_horizontal_buffer_size,int *combine_push_flag_buffer_horizontal_front,int *combine_id_buffer_horizontal_front,int *combine_value_buffer_horizontal_front,int * combine_pull_first_flag_buffer_horizontal_front,
                            int* combine_vertical_buffer_size,int *combine_push_flag_buffer_vertical_front,int *combine_id_buffer_vertical_front,int *combine_value_buffer_vertical_front,int * combine_pull_first_flag_buffer_vertical_front){


    static queue<int> push_flag_buffer_vertical[CORE_NUM], id_buffer_vertical[CORE_NUM], value_buffer_vertical[CORE_NUM], pull_first_flag_buffer_vertical[CORE_NUM];
    static queue<int> push_flag_buffer_horizontal[CORE_NUM], id_buffer_horizontal[CORE_NUM], value_buffer_horizontal[CORE_NUM], pull_first_flag_buffer_horizontal[CORE_NUM];
    static queue<int> push_flag_buffer_input[CORE_NUM], id_buffer_input[CORE_NUM], value_buffer_input[CORE_NUM], pull_first_flag_buffer_input[CORE_NUM];
    static int iteration_end_clk1[CORE_NUM], iteration_end_dvalid_clk1[CORE_NUM];
    static int vertical_buffer_empty[CORE_NUM], horizontal_buffer_empty[CORE_NUM], input_buffer_empty[CORE_NUM];
    static int vertical_buffer_full[CORE_NUM], horizontal_buffer_full[CORE_NUM], input_buffer_full[CORE_NUM];
   

    vertical_buffer_empty[Hyperx_Switch_ID] = (id_buffer_vertical[Hyperx_Switch_ID].size() == 0);
    horizontal_buffer_empty[Hyperx_Switch_ID] = (id_buffer_horizontal[Hyperx_Switch_ID].size() == 0);
    input_buffer_empty[Hyperx_Switch_ID] = (id_buffer_input[Hyperx_Switch_ID].size() == 0);

    #if(debug_8_0)
    /*
        if(clk == 832 && Hyperx_Switch_ID == 0){
            for(int i = 0 ; i<16384 ; i++){
                if(mp_hy1[i]){
                    printf("clk:%d hy1 %d still has %d in the network\n",clk,i,mp_hy1[i]);
                }
            }
            for(int i = 0 ; i<CORE_NUM ; i++){
                if(id_buffer_horizontal[i].size()){
                    printf("clk:%d hy1 buffer horizontal %d still has points %d\n",clk,i,id_buffer_horizontal[i].size());
                }
            }
            for(int i = 0 ; i<CORE_NUM ; i++){
                if(id_buffer_vertical[i].size()){
                    printf("clk:%d hy1 buffer vertical %d still has points %d\n",clk,i,id_buffer_vertical[i].size());
                }
            }
        }
        */
    #endif


    if(!rst_rd && iteration_end_clk1[Hyperx_Switch_ID] && iteration_end_dvalid_clk1[Hyperx_Switch_ID] && input_buffer_empty[Hyperx_Switch_ID] && horizontal_buffer_empty[Hyperx_Switch_ID] && vertical_buffer_empty[Hyperx_Switch_ID]){
        Hy_Iteration_End[Hyperx_Switch_ID] = 1;
        Hy_Iteration_End_DValid[Hyperx_Switch_ID] = 1;
    }else{
        Hy_Iteration_End[Hyperx_Switch_ID] = 0;
        Hy_Iteration_End_DValid[Hyperx_Switch_ID] = 0;
    }
    
    if(!rst_rd && Iteration_End_In && Iteration_End_DValid_In  && input_buffer_empty[Hyperx_Switch_ID]  && vertical_buffer_empty[Hyperx_Switch_ID] && horizontal_buffer_empty[Hyperx_Switch_ID]){
        iteration_end_clk1[Hyperx_Switch_ID] = 1;
        iteration_end_dvalid_clk1[Hyperx_Switch_ID] = 1;
    }else{
        iteration_end_clk1[Hyperx_Switch_ID] = 0;
        iteration_end_dvalid_clk1[Hyperx_Switch_ID] = 0;
    }

    /*input horizontal_mov*/
    if (rst_rd) {
    }else {
        if(!hy2_horizontal_stage_full[Hyperx_Switch_ID][tmp_hy2_input_col_id[Hyperx_Switch_ID]] && hy2_input_write_horizontal_buffer[Hyperx_Switch_ID] && !hy2_horizontal_block[Hyperx_Switch_ID][tmp_hy2_input_col_id[Hyperx_Switch_ID]]){
            push_flag_buffer_input[Hyperx_Switch_ID].pop();
            // if( clk > 73000 && id_buffer_input[Hyperx_Switch_ID].front() == 1048614 ){
            //     printf("line %d find 1048614 in clk:%d in channel:%d\n",__LINE__,clk,Hyperx_Switch_ID);
            // }
            id_buffer_input[Hyperx_Switch_ID].pop();
            value_buffer_input[Hyperx_Switch_ID].pop();
            pull_first_flag_buffer_input[Hyperx_Switch_ID].pop();
        }
           
    }



    /*input/horizontal vertical_mov*/
     if (rst_rd) {
    }else {
        if(!hy2_vertical_stage_full[Hyperx_Switch_ID][tmp_hy2_horizontal_row_id[Hyperx_Switch_ID]] && hy2_horizontal_write_vertical_buffer[Hyperx_Switch_ID] && !hy2_vertical_block[Hyperx_Switch_ID][tmp_hy2_horizontal_row_id[Hyperx_Switch_ID]]){
            push_flag_buffer_horizontal[Hyperx_Switch_ID].pop();
            // if( clk > 73000 && id_buffer_horizontal[Hyperx_Switch_ID].front() == 1048614 ){
            //     printf("line %d find 1048614 in clk:%d in channel:%d\n",__LINE__,clk,Hyperx_Switch_ID);
            // }
            id_buffer_horizontal[Hyperx_Switch_ID].pop();
            value_buffer_horizontal[Hyperx_Switch_ID].pop();
            pull_first_flag_buffer_horizontal[Hyperx_Switch_ID].pop();
        }

        if(!hy2_vertical_stage_full[Hyperx_Switch_ID][tmp_hy2_input_row_id[Hyperx_Switch_ID]] && hy2_input_write_vertical_buffer[Hyperx_Switch_ID] && !hy2_vertical_block[Hyperx_Switch_ID][tmp_hy2_input_row_id[Hyperx_Switch_ID]]){
            push_flag_buffer_input[Hyperx_Switch_ID].pop();
            // if( clk > 73000 && id_buffer_input[Hyperx_Switch_ID].front() == 1048614 ){
            //     printf("line %d find 1048614 in clk:%d in channel:%d\n",__LINE__,clk,Hyperx_Switch_ID);
            // }
            id_buffer_input[Hyperx_Switch_ID].pop();
            value_buffer_input[Hyperx_Switch_ID].pop();
            pull_first_flag_buffer_input[Hyperx_Switch_ID].pop();
        }
    }

    if(rst_rd){

    }else{
        hy2_write_old_vertical_network[Hyperx_Switch_ID] = hy2_horizontal_write_vertical_buffer[Hyperx_Switch_ID] ? 1 : (hy2_input_write_vertical_buffer[Hyperx_Switch_ID] ? 2 : 0) ;
        hy2_write_old_vertical_network_block[Hyperx_Switch_ID] = hy2_vertical_block[Hyperx_Switch_ID][hy2_horizontal_write_vertical_buffer[Hyperx_Switch_ID] ? tmp_hy2_horizontal_row_id[Hyperx_Switch_ID] : (hy2_input_write_vertical_buffer[Hyperx_Switch_ID] ? tmp_hy2_input_row_id[Hyperx_Switch_ID] : 0)];
    }

    /*input/horizontal/vertical output_mov*/
    if(Next_Stage_Full){
        Hy_Push_Flag[Hyperx_Switch_ID] = 0;
        Hy_Update_V_ID[Hyperx_Switch_ID] = 0;
        Hy_Update_V_Value[Hyperx_Switch_ID] = 0;
        Hy_Pull_First_Flag[Hyperx_Switch_ID] = 0;
        Hy_Update_V_DValid[Hyperx_Switch_ID] = 0;
    }else{
        if(hy2_vertical_write_output[Hyperx_Switch_ID]){
            Hy_Push_Flag[Hyperx_Switch_ID] = push_flag_buffer_vertical[Hyperx_Switch_ID].front();
            Hy_Update_V_ID[Hyperx_Switch_ID] = id_buffer_vertical[Hyperx_Switch_ID].front();
            // if( clk > 73000 && Hy_Update_V_ID[Hyperx_Switch_ID] == 1048614 ){
            //     printf("line %d find 1048614 in clk:%d in channel:%d\n",__LINE__,clk,Hyperx_Switch_ID);
            // }
            Hy_Update_V_Value[Hyperx_Switch_ID] = value_buffer_vertical[Hyperx_Switch_ID].front();
            Hy_Pull_First_Flag[Hyperx_Switch_ID] = pull_first_flag_buffer_vertical[Hyperx_Switch_ID].front();
            Hy_Update_V_DValid[Hyperx_Switch_ID] = 1;

            push_flag_buffer_vertical[Hyperx_Switch_ID].pop();
            id_buffer_vertical[Hyperx_Switch_ID].pop();
            value_buffer_vertical[Hyperx_Switch_ID].pop();
            pull_first_flag_buffer_vertical[Hyperx_Switch_ID].pop();
        }else if(hy2_horizontal_write_output[Hyperx_Switch_ID]){
            Hy_Push_Flag[Hyperx_Switch_ID] = push_flag_buffer_horizontal[Hyperx_Switch_ID].front();
            Hy_Update_V_ID[Hyperx_Switch_ID] = id_buffer_horizontal[Hyperx_Switch_ID].front();
            // if( clk > 73000 && Hy_Update_V_ID[Hyperx_Switch_ID] == 1048614 ){
            //     printf("line %d find 1048614 in clk:%d in channel:%d\n",__LINE__,clk,Hyperx_Switch_ID);
            // }
            Hy_Update_V_Value[Hyperx_Switch_ID] = value_buffer_horizontal[Hyperx_Switch_ID].front();
            Hy_Pull_First_Flag[Hyperx_Switch_ID] = pull_first_flag_buffer_horizontal[Hyperx_Switch_ID].front();
            Hy_Update_V_DValid[Hyperx_Switch_ID] = 1;

            push_flag_buffer_horizontal[Hyperx_Switch_ID].pop();
            id_buffer_horizontal[Hyperx_Switch_ID].pop();
            value_buffer_horizontal[Hyperx_Switch_ID].pop();
            pull_first_flag_buffer_horizontal[Hyperx_Switch_ID].pop();
        }else if(hy2_input_write_output[Hyperx_Switch_ID]){
            Hy_Push_Flag[Hyperx_Switch_ID] = push_flag_buffer_input[Hyperx_Switch_ID].front();
            Hy_Update_V_ID[Hyperx_Switch_ID] = id_buffer_input[Hyperx_Switch_ID].front();
            // if( clk > 73000 && Hy_Update_V_ID[Hyperx_Switch_ID] == 1048614 ){
            //     printf("line %d find 1048614 in clk:%d in channel:%d\n",__LINE__,clk,Hyperx_Switch_ID);
            // }
            Hy_Update_V_Value[Hyperx_Switch_ID] = value_buffer_input[Hyperx_Switch_ID].front();
            Hy_Pull_First_Flag[Hyperx_Switch_ID] = pull_first_flag_buffer_input[Hyperx_Switch_ID].front();
            Hy_Update_V_DValid[Hyperx_Switch_ID] = 1;

            push_flag_buffer_input[Hyperx_Switch_ID].pop();
            id_buffer_input[Hyperx_Switch_ID].pop();
            value_buffer_input[Hyperx_Switch_ID].pop();
            pull_first_flag_buffer_input[Hyperx_Switch_ID].pop();
        }else{
            Hy_Push_Flag[Hyperx_Switch_ID] = 0;
            Hy_Update_V_ID[Hyperx_Switch_ID] = 0;
            Hy_Update_V_Value[Hyperx_Switch_ID] = 0;
            Hy_Pull_First_Flag[Hyperx_Switch_ID] = 0;
            Hy_Update_V_DValid[Hyperx_Switch_ID] = 0;
        }
    }


    /*vertical_buffer_Receive*/
    if (rst_rd) {
    }else if(tmp_hy2_vertical_v_valid_in_min[Hyperx_Switch_ID]){//这个标记是否会增加依赖链路
        push_flag_buffer_vertical[Hyperx_Switch_ID].push(hy2_vertical_push_flag_in[Hyperx_Switch_ID][tmp_hy2_vertical_v_valid_index_min[Hyperx_Switch_ID]]);
        id_buffer_vertical[Hyperx_Switch_ID].push(hy2_vertical_v_id_in[Hyperx_Switch_ID][tmp_hy2_vertical_v_valid_index_min[Hyperx_Switch_ID]]);
        value_buffer_vertical[Hyperx_Switch_ID].push(hy2_vertical_v_value_in[Hyperx_Switch_ID][tmp_hy2_vertical_v_valid_index_min[Hyperx_Switch_ID]]);
        pull_first_flag_buffer_vertical[Hyperx_Switch_ID].push(hy2_vertical_pull_first_flag_in[Hyperx_Switch_ID][tmp_hy2_vertical_v_valid_index_min[Hyperx_Switch_ID]]);
        #if(debug_8)
        if(hy2_vertical_v_id_in[Hyperx_Switch_ID][tmp_hy2_vertical_v_valid_index_min[Hyperx_Switch_ID]] == 1047122)
            printf("module_8 Vertical Receive clk: %d v_id:%d v_value:%d push_flag:%d row:%d vertical_mov -> row:%d\n",clk,hy2_vertical_v_id_in[Hyperx_Switch_ID][tmp_hy2_vertical_v_valid_index_min[Hyperx_Switch_ID]],hy2_vertical_v_value_in[Hyperx_Switch_ID][tmp_hy2_vertical_v_valid_index_min[Hyperx_Switch_ID]],hy2_vertical_push_flag_in[Hyperx_Switch_ID][tmp_hy2_vertical_v_valid_index_min[Hyperx_Switch_ID]],tmp_hy2_vertical_v_valid_index_min[Hyperx_Switch_ID],Hyperx_Switch_ID%CORE_NUM/HORIZONTAL_LEN);
        #endif
    }

    /*horizontal_buffer_receive*/
    if (rst_rd) {
    }else if(tmp_hy2_horizontal_v_valid_in_min[Hyperx_Switch_ID]){//这个标记是否会增加依赖链路
        push_flag_buffer_horizontal[Hyperx_Switch_ID].push(hy2_horizontal_push_flag_in[Hyperx_Switch_ID][tmp_hy2_horizontal_v_valid_index_min[Hyperx_Switch_ID]]);
        id_buffer_horizontal[Hyperx_Switch_ID].push(hy2_horizontal_v_id_in[Hyperx_Switch_ID][tmp_hy2_horizontal_v_valid_index_min[Hyperx_Switch_ID]]);
        value_buffer_horizontal[Hyperx_Switch_ID].push(hy2_horizontal_v_value_in[Hyperx_Switch_ID][tmp_hy2_horizontal_v_valid_index_min[Hyperx_Switch_ID]]);
        pull_first_flag_buffer_horizontal[Hyperx_Switch_ID].push(hy2_horizontal_pull_first_flag_in[Hyperx_Switch_ID][tmp_hy2_horizontal_v_valid_index_min[Hyperx_Switch_ID]]);
        #if(debug_8)
        if(hy2_horizontal_v_id_in[Hyperx_Switch_ID][tmp_hy2_horizontal_v_valid_index_min[Hyperx_Switch_ID]] == 1047122)
            printf("module_8 Horizontal Receive clk: %d v_id:%d v_value:%d push_flag:%d col:%d horizontal_mov -> col:%d\n",clk,hy2_horizontal_v_id_in[Hyperx_Switch_ID][tmp_hy2_horizontal_v_valid_index_min[Hyperx_Switch_ID]],hy2_horizontal_v_value_in[Hyperx_Switch_ID][tmp_hy2_horizontal_v_valid_index_min[Hyperx_Switch_ID]],hy2_horizontal_push_flag_in[Hyperx_Switch_ID][tmp_hy2_horizontal_v_valid_index_min[Hyperx_Switch_ID]],tmp_hy2_horizontal_v_valid_index_min[Hyperx_Switch_ID],Hyperx_Switch_ID%CORE_NUM%HORIZONTAL_LEN);
        #endif
    }

    /*in->vertical/horizontal_buffer*/
    if (rst_rd) {
        /*buffer initilization*/
    }
    else {
        if(V_Valid_In){
            push_flag_buffer_input[Hyperx_Switch_ID].push(Push_Flag);
            id_buffer_input[Hyperx_Switch_ID].push(V_ID_In);
            // if( clk > 73000 && id_buffer_input[Hyperx_Switch_ID].front() == 1048614 ){
            //     printf("line %d find 1048614 in clk:%d in channel:%d\n",__LINE__,clk,Hyperx_Switch_ID);
            // }
            value_buffer_input[Hyperx_Switch_ID].push(V_Value_In);
            pull_first_flag_buffer_input[Hyperx_Switch_ID].push(Pull_First_Flag);
            // printf("clk:%d v_id:%d in\n",clk,V_ID_In);
        }
    }

    #if(debug_8_0)
    if(rst_rd){
        if(!rst_hy2){
            for(int i = 0 ; i< 1048576 ; i++){
                mp_hy2[i] = 0;
            }
            rst_hy2 = 1;
        }
    }
    if(V_Valid_In){
        mp_hy2[V_ID_In] ++;
    }
    #endif

    #if(debug_8_0)
    if(Hy_Update_V_DValid[Hyperx_Switch_ID]){
        mp_hy2[Hy_Update_V_ID[Hyperx_Switch_ID]] --;
        if(mp_hy2[Hy_Update_V_ID[Hyperx_Switch_ID]] < 0){
            printf("clk:%d,hy2 produce unknown point %d\n",clk,Hy_Update_V_ID[Hyperx_Switch_ID]);
        }
        if(Hy_Update_V_ID[Hyperx_Switch_ID] % CORE_NUM != Hyperx_Switch_ID){
            printf("clk:%d,hy2 error channel for id:%d in channel:%d\n",clk,Hy_Update_V_ID[Hyperx_Switch_ID],Hyperx_Switch_ID);
        }
    }
    #endif

    vertical_buffer_full[Hyperx_Switch_ID] = (id_buffer_vertical[Hyperx_Switch_ID].size() >= FIFO_SIZE);
    horizontal_buffer_full[Hyperx_Switch_ID] = (id_buffer_horizontal[Hyperx_Switch_ID].size() >= FIFO_SIZE);
    input_buffer_full[Hyperx_Switch_ID] = (id_buffer_input[Hyperx_Switch_ID].size() >= FIFO_SIZE);
    for(int i = 0 ; i< VERTICAL_LEN ; i++){
        Vertical_Stage_Full[i] = vertical_buffer_full[Hyperx_Switch_ID];
    }
    for(int i = 0 ; i< HORIZONTAL_LEN ; i++){
        Horizontal_Stage_Full[i] = horizontal_buffer_full[Hyperx_Switch_ID];
    }

    Stage_Full_Hy[Hyperx_Switch_ID] = input_buffer_full[Hyperx_Switch_ID] ;

    *combine_vertical_buffer_size = id_buffer_vertical[Hyperx_Switch_ID].size();
    *combine_id_buffer_vertical_front = id_buffer_vertical[Hyperx_Switch_ID].front();
    *combine_push_flag_buffer_vertical_front = push_flag_buffer_vertical[Hyperx_Switch_ID].front();
    *combine_value_buffer_vertical_front = value_buffer_vertical[Hyperx_Switch_ID].front();
    *combine_pull_first_flag_buffer_vertical_front = pull_first_flag_buffer_vertical[Hyperx_Switch_ID].front();
    
    *combine_horizontal_buffer_size = id_buffer_horizontal[Hyperx_Switch_ID].size();
    *combine_id_buffer_horizontal_front = id_buffer_horizontal[Hyperx_Switch_ID].front();
    *combine_push_flag_buffer_horizontal_front = push_flag_buffer_horizontal[Hyperx_Switch_ID].front();
    *combine_value_buffer_horizontal_front = value_buffer_horizontal[Hyperx_Switch_ID].front();
    *combine_pull_first_flag_buffer_horizontal_front = pull_first_flag_buffer_horizontal[Hyperx_Switch_ID].front();
    
    *combine_input_buffer_size = id_buffer_input[Hyperx_Switch_ID].size();
    *combine_id_buffer_input_front = id_buffer_input[Hyperx_Switch_ID].front();
    *combine_push_flag_buffer_input_front = push_flag_buffer_input[Hyperx_Switch_ID].front();
    *combine_value_buffer_input_front = value_buffer_input[Hyperx_Switch_ID].front();
    *combine_pull_first_flag_buffer_input_front = pull_first_flag_buffer_input[Hyperx_Switch_ID].front();

}

