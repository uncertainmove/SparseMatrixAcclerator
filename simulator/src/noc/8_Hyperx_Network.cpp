#include "network.h"
#include "debug.h"
#include <bits/stdc++.h>

#if DEBUG
    debug_Noc debug_M8;
#endif

extern int clk;
extern int rst_rd;

using namespace std;

void Hyperx_Network_Single(
    int HPX_Vertical_ID, int HPX_Horizontal_ID,
    int Front_V_ID, V_VALUE_TP Front_V_Value, int Front_V_Valid, 
    int Front_Iteration_End, int Front_Iteration_End_DValid, int Front_Iteration_ID,
    int Next_Stage_Full, 

    int Read_Vertical,
    int Front_Vertical_V_ID, V_VALUE_TP Front_Vertical_V_Value, int Front_Vertical_V_Valid,

    int Read_Horizontal,
    int Front_Horizontal_V_ID, V_VALUE_TP Front_Horizontal_V_Value, int Front_Horizontal_V_Valid,
                        
    int *Vertical_V_ID, V_VALUE_TP *Vertical_V_Value, int *Vertical_V_Valid,
    int *Vertical_Buffer_Full,

    int *Horizontal_V_ID, V_VALUE_TP *Horizontal_V_Value, int *Horizontal_V_Valid,
    int *Horizontal_Buffer_Full, int *Local_Buffer_Full,

    int *Update_V_ID, V_VALUE_TP *Update_V_Value, int *Update_V_DValid, 
    int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID,
    int *Stage_Full);

void Hyperx_Network_Vertical_Crossbar_Single(
    int Horizontal_ID,
    int Front_V_ID[], V_VALUE_TP Front_V_Value[], int Front_V_Valid[],
    int Horizontal_Buffer_Full[],

    int *Read_Vertical,
    int *V_ID, V_VALUE_TP *V_Value, int *V_Valid);

void Hyperx_Network_Horizontal_Crossbar_Single(
    int Vertical_ID,
    int Front_V_ID[], V_VALUE_TP Front_V_Value[], int Front_V_Valid[],
    int Local_Buffer_Full[],

    int *Read_Horizontal,
    int *V_ID, V_VALUE_TP *V_Value, int *V_Valid);

void Hyperx_Network(
        int Hy1_Front_Update_V_ID[], V_VALUE_TP Hy1_Front_Update_V_Value[], int Hy1_Front_Update_V_DValid[],
        int Hy1_Front_Iteration_End[], int Hy1_Front_Iteration_End_DValid[], int Hy1_Front_Iteration_ID[],
        int Dest_Core_Full[],

        int *Hy1_Update_V_ID, V_VALUE_TP *Hy1_Update_V_Value, int *Hy1_Update_V_DValid,
        int *Hy1_Iteration_End, int *Hy1_Iteration_End_DValid, int *Hy1_Iteration_ID,
        int *Hy1_Stage_Full) {

    // crossbar wire
    int hy1_vertical_crossbar_v_id[HPX_HORIZONTAL_NUM][HPX_VERTICAL_NUM], hy1_vertical_crossbar_v_valid[HPX_HORIZONTAL_NUM][HPX_VERTICAL_NUM];
    V_VALUE_TP hy1_vertical_crossbar_v_value[HPX_HORIZONTAL_NUM][HPX_VERTICAL_NUM];

    int hy1_horizontal_crossbar_v_id[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM], hy1_horizontal_crossbar_v_valid[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];
    V_VALUE_TP hy1_horizontal_crossbar_v_value[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];

    int Read_Vertical[HPX_HORIZONTAL_NUM][HPX_VERTICAL_NUM];
    int Read_Horizontal[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];
    // output
    static int hy1_vertical_v_id_rd[HPX_HORIZONTAL_NUM][HPX_VERTICAL_NUM], hy1_vertical_v_valid_rd[HPX_HORIZONTAL_NUM][HPX_VERTICAL_NUM], hy1_vertical_buffer_full_rd[HPX_HORIZONTAL_NUM][HPX_VERTICAL_NUM];
    static V_VALUE_TP hy1_vertical_v_value_rd[HPX_HORIZONTAL_NUM][HPX_VERTICAL_NUM];
    static int hy1_vertical_v_id_wr[HPX_HORIZONTAL_NUM][HPX_VERTICAL_NUM], hy1_vertical_v_valid_wr[HPX_HORIZONTAL_NUM][HPX_VERTICAL_NUM], hy1_vertical_buffer_full_wr[HPX_HORIZONTAL_NUM][HPX_VERTICAL_NUM];
    static V_VALUE_TP hy1_vertical_v_value_wr[HPX_HORIZONTAL_NUM][HPX_VERTICAL_NUM];

    static int hy1_horizontal_v_id_rd[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM], hy1_horizontal_v_valid_rd[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];
    static V_VALUE_TP hy1_horizontal_v_value_rd[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];
    static int hy1_horizontal_v_id_wr[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM], hy1_horizontal_v_valid_wr[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];
    static V_VALUE_TP hy1_horizontal_v_value_wr[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];

    static int hy1_horizontal_buffer_full_rd[HPX_HORIZONTAL_NUM][HPX_VERTICAL_NUM];
    static int hy1_horizontal_buffer_full_wr[HPX_HORIZONTAL_NUM][HPX_VERTICAL_NUM];
    static int hy1_local_buffer_full_rd[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];
    static int hy1_local_buffer_full_wr[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];

    for (int i = 0; i < HPX_VERTICAL_NUM; i++) {
        Hyperx_Network_Horizontal_Crossbar_Single(
            i,
            hy1_horizontal_v_id_rd[i], hy1_horizontal_v_value_rd[i], hy1_horizontal_v_valid_rd[i],
            hy1_local_buffer_full_rd[i],

            Read_Horizontal[i],
            hy1_horizontal_crossbar_v_id[i], hy1_horizontal_crossbar_v_value[i], hy1_horizontal_crossbar_v_valid[i]
        );
    }
    for (int i = 0; i < HPX_HORIZONTAL_NUM; i++) {
        Hyperx_Network_Vertical_Crossbar_Single(
            i,
            hy1_vertical_v_id_rd[i], hy1_vertical_v_value_rd[i], hy1_vertical_v_valid_rd[i],
            hy1_horizontal_buffer_full_rd[i],

            Read_Vertical[i],
            hy1_vertical_crossbar_v_id[i], hy1_vertical_crossbar_v_value[i], hy1_vertical_crossbar_v_valid[i]
        );
    }
    // bind port
    // each hyperx node catch vertical buffer signal of its row and horizontal buffer signal of its column
    for (int i = 0; i < HPX_VERTICAL_NUM; i++) { // row
        for (int j = 0; j < HPX_HORIZONTAL_NUM; j++) { // column
            Hyperx_Network_Single(
                i, j,
                Hy1_Front_Update_V_ID[i * HPX_HORIZONTAL_NUM + j], Hy1_Front_Update_V_Value[i * HPX_HORIZONTAL_NUM + j], Hy1_Front_Update_V_DValid[i * HPX_HORIZONTAL_NUM + j],
                Hy1_Front_Iteration_End[i * HPX_HORIZONTAL_NUM + j], Hy1_Front_Iteration_End_DValid[i * HPX_HORIZONTAL_NUM + j], Hy1_Front_Iteration_ID[i * HPX_HORIZONTAL_NUM + j],
                Dest_Core_Full[i * HPX_HORIZONTAL_NUM + j],
                               
                // group signal from vertical crossbar
                Read_Vertical[j][i],
                hy1_vertical_crossbar_v_id[j][i], hy1_vertical_crossbar_v_value[j][i], hy1_vertical_crossbar_v_valid[j][i],

                // group signal from horizontal crossbar
                Read_Horizontal[i][j],
                hy1_horizontal_crossbar_v_id[i][j], hy1_horizontal_crossbar_v_value[i][j], hy1_horizontal_crossbar_v_valid[i][j],

                // single signal to vertical
                &hy1_vertical_v_id_wr[j][i], &hy1_vertical_v_value_wr[j][i], &hy1_vertical_v_valid_wr[j][i],
                &hy1_vertical_buffer_full_wr[j][i],

                // single signal to horizontal
                &hy1_horizontal_v_id_wr[i][j], &hy1_horizontal_v_value_wr[i][j], &hy1_horizontal_v_valid_wr[i][j],
                &hy1_horizontal_buffer_full_wr[j][i], &hy1_local_buffer_full_wr[i][j],

                &Hy1_Update_V_ID[i * HPX_HORIZONTAL_NUM + j], &Hy1_Update_V_Value[i * HPX_HORIZONTAL_NUM + j], &Hy1_Update_V_DValid[i * HPX_HORIZONTAL_NUM + j],
                &Hy1_Iteration_End[i * HPX_HORIZONTAL_NUM + j], &Hy1_Iteration_End_DValid[i * HPX_HORIZONTAL_NUM + j], &Hy1_Iteration_ID[i * HPX_HORIZONTAL_NUM + j],
                &Hy1_Stage_Full[i * HPX_HORIZONTAL_NUM + j]
            );
            /*
            if (Hy1_Update_V_DValid[i * HPX_HORIZONTAL_NUM + j] && Hy1_Update_V_ID[i * HPX_HORIZONTAL_NUM + j] == 0) {
                cout << "hyperx 0 value " << Hy1_Update_V_Value[i * HPX_HORIZONTAL_NUM + j] << endl;
            }
            */
        }
    }

    for (int i = 0; i < HPX_VERTICAL_NUM; i++) { // row
        for (int j = 0; j < HPX_HORIZONTAL_NUM; j++) { // column
            hy1_vertical_v_id_rd[j][i] = hy1_vertical_v_id_wr[j][i];
            hy1_vertical_v_value_rd[j][i] = hy1_vertical_v_value_wr[j][i];
            hy1_vertical_v_valid_rd[j][i] = hy1_vertical_v_valid_wr[j][i];
            hy1_vertical_buffer_full_rd[j][i] = hy1_vertical_buffer_full_wr[j][i];

            hy1_horizontal_v_id_rd[i][j] = hy1_horizontal_v_id_wr[i][j];
            hy1_horizontal_v_value_rd[i][j] = hy1_horizontal_v_value_wr[i][j];
            hy1_horizontal_v_valid_rd[i][j] = hy1_horizontal_v_valid_wr[i][j];

            hy1_horizontal_buffer_full_rd[j][i] = hy1_horizontal_buffer_full_wr[j][i];
            hy1_local_buffer_full_rd[i][j] = hy1_local_buffer_full_wr[i][j];
        }
    }
}

void Hyperx_Network_Vertical_Crossbar_Single(
        int Horizontal_ID,
        int Front_V_ID[], V_VALUE_TP Front_V_Value[], int Front_V_Valid[],
        int Horizontal_Buffer_Full[],

        int *Read_Vertical,
        int *V_ID, V_VALUE_TP *V_Value, int *V_Valid) {

    // vertical -> vertical
    int shuffle_signal[HPX_VERTICAL_NUM][HPX_VERTICAL_NUM];

    for (int i = 0; i < HPX_VERTICAL_NUM; i++) {
        int front_row_id = (Front_V_ID[i] % CORE_NUM) / HPX_HORIZONTAL_NUM;
        int front_col_id = Front_V_ID[i] % HPX_HORIZONTAL_NUM;
        for (int j = 0; j < HPX_VERTICAL_NUM; j++) {
            shuffle_signal[i][j] = (front_row_id == j && Front_V_Valid[i]);
        }
        V_ID[i] = 0;
        V_Value[i] = 0;
        V_Valid[i] = 0;
        Read_Vertical[i] = 0;
    }
    for (int j = 0; j < HPX_VERTICAL_NUM; j++) {
        for (int i = 0; i < HPX_VERTICAL_NUM; i++) {
            if (shuffle_signal[i][j] && !Horizontal_Buffer_Full[j]) {
                V_ID[j] = Front_V_ID[i];
                V_Value[j] = Front_V_Value[i];
                V_Valid[j] = 1;
                Read_Vertical[i] = 1;
                break;
            }
        }
    }
}

void Hyperx_Network_Horizontal_Crossbar_Single(
        int Vertical_ID,
        int Front_V_ID[], V_VALUE_TP Front_V_Value[], int Front_V_Valid[],
        int Local_Buffer_Full[],

        int *Read_Horizontal,
        int *V_ID, V_VALUE_TP *V_Value, int *V_Valid) {

    // horizontal -> horizontal
    int shuffle_signal[HPX_HORIZONTAL_NUM][HPX_HORIZONTAL_NUM];

    for (int i = 0; i < HPX_HORIZONTAL_NUM; i++) {
        int front_row_id = (Front_V_ID[i] % CORE_NUM) / HPX_HORIZONTAL_NUM;
        int front_col_id = Front_V_ID[i] % HPX_HORIZONTAL_NUM;
        for (int j = 0; j < HPX_HORIZONTAL_NUM; j++) {
            shuffle_signal[i][j] = (front_col_id == j && Front_V_Valid[i]);
        }
        V_ID[i] = 0;
        V_Value[i] = 0;
        V_Valid[i] = 0;
        Read_Horizontal[i] = 0;
    }
    for (int j = 0; j < HPX_HORIZONTAL_NUM; j++) {
        for (int i = 0; i < HPX_HORIZONTAL_NUM; i++) {
            if (shuffle_signal[i][j] && !Local_Buffer_Full[j]) {
                V_ID[j] = Front_V_ID[i];
                V_Value[j] = Front_V_Value[i];
                V_Valid[j] = 1;
                Read_Horizontal[i] = 1;
                break;
            }
        }
    }
}

void Hyperx_Network_Single(
        int HPX_Vertical_ID, int HPX_Horizontal_ID,
        int Front_V_ID, V_VALUE_TP Front_V_Value, int Front_V_Valid, 
        int Front_Iteration_End, int Front_Iteration_End_DValid, int Front_Iteration_ID,
        int Next_Stage_Full, 

        int Read_Vertical,
        int Front_Vertical_V_ID, V_VALUE_TP Front_Vertical_V_Value, int Front_Vertical_V_Valid,

        int Read_Horizontal,
        int Front_Horizontal_V_ID, V_VALUE_TP Front_Horizontal_V_Value, int Front_Horizontal_V_Valid,
                        
        int *Vertical_V_ID, V_VALUE_TP *Vertical_V_Value, int *Vertical_V_Valid,
        int *Vertical_Buffer_Full,

        int *Horizontal_V_ID, V_VALUE_TP *Horizontal_V_Value, int *Horizontal_V_Valid,
        int *Horizontal_Buffer_Full, int *Local_Buffer_Full,

        int *Update_V_ID, V_VALUE_TP *Update_V_Value, int *Update_V_DValid, 
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID,
        int *Stage_Full) {

    static queue<int> vertical_v_id_buffer[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];
    static queue<V_VALUE_TP> vertical_v_value_buffer[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM]; // from vertical
    static queue<int> horizontal_v_id_buffer[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];
    static queue<V_VALUE_TP> horizontal_v_value_buffer[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM]; // from horizontal
    static queue<int> v_id_buffer[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];
    static queue<V_VALUE_TP> v_value_buffer[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM]; // from scheduler
    static int iteration_end[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM], iteration_end_dvalid[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM], iteration_id[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];
    static int vertical_buffer_empty[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM], vertical_buffer_full[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];
    static int horizontal_buffer_empty[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM], horizontal_buffer_full[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];
    static int buffer_empty[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM], buffer_full[HPX_VERTICAL_NUM][HPX_HORIZONTAL_NUM];

    int Core_ID = HPX_Vertical_ID * HPX_HORIZONTAL_NUM + HPX_Horizontal_ID;
   
    #if DEBUG
        debug_M8._vertical_v_id_buffer = vertical_v_id_buffer;
        debug_M8._vertical_v_value_buffer = vertical_v_value_buffer;
        debug_M8._horizontal_v_id_buffer = horizontal_v_id_buffer;
        debug_M8._horizontal_v_value_buffer = horizontal_v_value_buffer;
        debug_M8._v_id_buffer = v_id_buffer;
        debug_M8._v_value_buffer = v_value_buffer;
    #endif

    vertical_buffer_empty[HPX_Vertical_ID][HPX_Horizontal_ID] = (vertical_v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].size() == 0);
    horizontal_buffer_empty[HPX_Vertical_ID][HPX_Horizontal_ID] = (horizontal_v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].size() == 0);
    buffer_empty[HPX_Vertical_ID][HPX_Horizontal_ID] = (v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].size() == 0);

    int front_row_id = (Front_V_ID % CORE_NUM) / HPX_HORIZONTAL_NUM;
    int front_col_id = Front_V_ID % HPX_HORIZONTAL_NUM;
    int vertical_row_id = (Front_Vertical_V_ID % CORE_NUM) / HPX_HORIZONTAL_NUM;
    int vertical_col_id = Front_Vertical_V_ID % HPX_HORIZONTAL_NUM;
    int horizontal_row_id = (Front_Horizontal_V_ID % CORE_NUM) / HPX_HORIZONTAL_NUM;
    int horizontal_col_id = Front_Horizontal_V_ID % HPX_HORIZONTAL_NUM;

    if (!rst_rd && iteration_end[HPX_Vertical_ID][HPX_Horizontal_ID] && iteration_end_dvalid[HPX_Vertical_ID][HPX_Horizontal_ID] &&
            buffer_empty[HPX_Vertical_ID][HPX_Horizontal_ID] && horizontal_buffer_empty[HPX_Vertical_ID][HPX_Horizontal_ID] &&
            vertical_buffer_empty[HPX_Vertical_ID][HPX_Horizontal_ID]){
        *Iteration_End = 1;
        *Iteration_End_DValid = 1;
        *Iteration_ID = Front_Iteration_ID;
    } else {
        *Iteration_End = 0;
        *Iteration_End_DValid = 0;
        *Iteration_ID = Front_Iteration_ID;
    }
    
    if(!rst_rd && Front_Iteration_End && Front_Iteration_End_DValid && buffer_empty[HPX_Vertical_ID][HPX_Horizontal_ID] &&
            vertical_buffer_empty[HPX_Vertical_ID][HPX_Horizontal_ID] && horizontal_buffer_empty[HPX_Vertical_ID][HPX_Horizontal_ID]) {
        iteration_end[HPX_Vertical_ID][HPX_Horizontal_ID] = 1;
        iteration_end_dvalid[HPX_Vertical_ID][HPX_Horizontal_ID] = 1;
    } else {
        iteration_end[HPX_Vertical_ID][HPX_Horizontal_ID] = 0;
        iteration_end_dvalid[HPX_Vertical_ID][HPX_Horizontal_ID] = 0;
    }

    // v_id out
    if (rst_rd || Next_Stage_Full || buffer_empty[HPX_Vertical_ID][HPX_Horizontal_ID]) {
        *Update_V_ID = 0;
        *Update_V_Value = 0;
        *Update_V_DValid = 0;
    } else {
        *Update_V_ID = v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].front();
        *Update_V_Value = v_value_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].front();
        *Update_V_DValid = 1;
        if (*Update_V_ID == 1) {
            // cout << "core " << Core_ID << " local send 1 value " << *Update_V_Value << endl;
        }
        
        v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].pop();
        v_value_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].pop();
    }

    // vertical v_id_buffer out
    if (rst_rd || vertical_buffer_empty[HPX_Vertical_ID][HPX_Horizontal_ID]) {
    } else {
        // vertical buffer will transform to other horizontal buffer, check status
        if (Read_Vertical) {
            if (vertical_buffer_empty[HPX_Vertical_ID][HPX_Horizontal_ID]) {
                cout << "[ERROR] read vertical buffer but buffer is empty in core " << Core_ID << endl;
                exit(-1);
            }
            if (vertical_v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].front() == 1) {
                // cout << "core " << Core_ID << " vertical send 1 value " << *Vertical_V_Value << endl;
            }
            vertical_v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].pop();
            vertical_v_value_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].pop();
        }
    }

    // horizontal v_id_buffer out
    if (rst_rd) {
    } else {
        // Horizontal buffer will transform to other output buffer, check status
        if (Read_Horizontal) {
            if (horizontal_buffer_empty[HPX_Vertical_ID][HPX_Horizontal_ID]) {
                cout << "[ERROR] read horizontal buffer but buffer is empty in core " << Core_ID << endl;
                exit(-1);
            }
            if (horizontal_v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].front() == 1) {
                // cout << "core " << Core_ID << " horizontal send 1 value " << *Horizontal_V_Value << endl;
            }
            horizontal_v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].pop();
            horizontal_v_value_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].pop();
        }
    }

    // buffer input
    // just horizontal input to local buffer
    if (rst_rd) {

    } else {
        // should store signal even buffer is full
        if (Front_Horizontal_V_Valid) {
            if (horizontal_row_id != HPX_Vertical_ID || horizontal_col_id != HPX_Horizontal_ID) {
                printf("[ERROR] core %d: store %d to local buffer\n", Front_Horizontal_V_ID);
                exit(-1);
            }
            if (Front_Horizontal_V_ID == 1) {
                // cout << "core " << Core_ID << " local store 1 value " << Front_Horizontal_V_Value << endl;
            }
            v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].push(Front_Horizontal_V_ID);
            v_value_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].push(Front_Horizontal_V_Value);
        }
    }

    // vertical buffer input
    // just scheduler input signal to vertical buffer
    if (rst_rd) {

    } else {
        if (Front_V_Valid) {
            if (Front_V_ID == 1) {
                // cout << "core " << Core_ID << " vertical store 1 value " << Front_V_Value << endl;
            }
            vertical_v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].push(Front_V_ID);
            vertical_v_value_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].push(Front_V_Value);
        }
    }

    // horizontal buffer input
    // vertical input signal to horizontal buffer
    if (rst_rd) {

    } else {
        // should store signal even buffer is full
        if (Front_Vertical_V_Valid) {
            if (vertical_row_id != HPX_Vertical_ID) {
                printf("[ERROR] clk %d core %d: store %d to horizontal buffer\n", clk, Front_Vertical_V_ID);
                exit(-1);
            }
            if (Front_Vertical_V_ID == 1) {
                // cout << "core " << Core_ID << " horizontal store 1 value " << Front_Vertical_V_Value << endl;
            }
            horizontal_v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].push(Front_Vertical_V_ID);
            horizontal_v_value_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].push(Front_Vertical_V_Value);
        }
    }

    // assign output
    *Vertical_V_ID = vertical_v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].front();
    *Vertical_V_Value = vertical_v_value_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].front();
    *Vertical_V_Valid = !vertical_v_value_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].empty();
    *Horizontal_V_ID = horizontal_v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].front();
    *Horizontal_V_Value = horizontal_v_value_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].front();
    *Horizontal_V_Valid = !horizontal_v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].empty();

    *Local_Buffer_Full = (v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].size() >= FIFO_SIZE);
    *Vertical_Buffer_Full = (vertical_v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].size() >= FIFO_SIZE);
    *Horizontal_Buffer_Full = (horizontal_v_id_buffer[HPX_Vertical_ID][HPX_Horizontal_ID].size() >= FIFO_SIZE);
    *Stage_Full = (*Vertical_Buffer_Full);

}