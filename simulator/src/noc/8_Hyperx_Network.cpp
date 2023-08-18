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
    int HPX_Row_ID, int HPX_Column_ID,
    int Front_V_ID, V_VALUE_TP Front_V_Value, int Front_V_Valid, 
    int Front_Iteration_End, int Front_Iteration_End_DValid, int Front_Iteration_ID,
    int Next_Stage_Full, 

    int Read_Row,
    int Front_Row_V_ID, V_VALUE_TP Front_Row_V_Value, int Front_Row_V_Valid,

    int Read_Column,
    int Front_Column_V_ID, V_VALUE_TP Front_Column_V_Value, int Front_Column_V_Valid,
                        
    int *Row_V_ID, V_VALUE_TP *Row_V_Value, int *Row_V_Valid,
    int *Row_Buffer_Full,

    int *Column_V_ID, V_VALUE_TP *Column_V_Value, int *Column_V_Valid,
    int *Column_Buffer_Full, int *Local_Buffer_Full,

    int *Update_V_ID, V_VALUE_TP *Update_V_Value, int *Update_V_DValid, 
    int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID,
    int *Stage_Full);

void Hyperx_Network_Row_Crossbar_Single(
    int Column_ID,
    int Front_V_ID[], V_VALUE_TP Front_V_Value[], int Front_V_Valid[],
    int Column_Buffer_Full[],

    int *Read_Row,
    int *V_ID, V_VALUE_TP *V_Value, int *V_Valid);

void Hyperx_Network_Column_Crossbar_Single(
    int Row_ID,
    int Front_V_ID[], V_VALUE_TP Front_V_Value[], int Front_V_Valid[],
    int Local_Buffer_Full[],

    int *Read_Column,
    int *V_ID, V_VALUE_TP *V_Value, int *V_Valid);

void Hyperx_Network(
        int Hy1_Front_Update_V_ID[], V_VALUE_TP Hy1_Front_Update_V_Value[], int Hy1_Front_Update_V_DValid[],
        int Hy1_Front_Iteration_End[], int Hy1_Front_Iteration_End_DValid[], int Hy1_Front_Iteration_ID[],
        int Dest_Core_Full[],

        int *Hy1_Update_V_ID, V_VALUE_TP *Hy1_Update_V_Value, int *Hy1_Update_V_DValid,
        int *Hy1_Iteration_End, int *Hy1_Iteration_End_DValid, int *Hy1_Iteration_ID,
        int *Hy1_Stage_Full) {

    // crossbar wire
    int hy1_row_crossbar_v_id[HPX_COLUMN_NUM][HPX_ROW_NUM], hy1_row_crossbar_v_valid[HPX_COLUMN_NUM][HPX_ROW_NUM];
    V_VALUE_TP hy1_row_crossbar_v_value[HPX_COLUMN_NUM][HPX_ROW_NUM];

    int hy1_column_crossbar_v_id[HPX_ROW_NUM][HPX_COLUMN_NUM], hy1_column_crossbar_v_valid[HPX_ROW_NUM][HPX_COLUMN_NUM];
    V_VALUE_TP hy1_column_crossbar_v_value[HPX_ROW_NUM][HPX_COLUMN_NUM];

    int Read_Row[HPX_COLUMN_NUM][HPX_ROW_NUM];
    int Read_Column[HPX_ROW_NUM][HPX_COLUMN_NUM];
    // output
    static int hy1_row_v_id_rd[HPX_COLUMN_NUM][HPX_ROW_NUM], hy1_row_v_valid_rd[HPX_COLUMN_NUM][HPX_ROW_NUM], hy1_row_buffer_full_rd[HPX_COLUMN_NUM][HPX_ROW_NUM];
    static V_VALUE_TP hy1_row_v_value_rd[HPX_COLUMN_NUM][HPX_ROW_NUM];
    static int hy1_row_v_id_wr[HPX_COLUMN_NUM][HPX_ROW_NUM], hy1_row_v_valid_wr[HPX_COLUMN_NUM][HPX_ROW_NUM], hy1_row_buffer_full_wr[HPX_COLUMN_NUM][HPX_ROW_NUM];
    static V_VALUE_TP hy1_row_v_value_wr[HPX_COLUMN_NUM][HPX_ROW_NUM];

    static int hy1_column_v_id_rd[HPX_ROW_NUM][HPX_COLUMN_NUM], hy1_column_v_valid_rd[HPX_ROW_NUM][HPX_COLUMN_NUM];
    static V_VALUE_TP hy1_column_v_value_rd[HPX_ROW_NUM][HPX_COLUMN_NUM];
    static int hy1_column_v_id_wr[HPX_ROW_NUM][HPX_COLUMN_NUM], hy1_column_v_valid_wr[HPX_ROW_NUM][HPX_COLUMN_NUM];
    static V_VALUE_TP hy1_column_v_value_wr[HPX_ROW_NUM][HPX_COLUMN_NUM];

    static int hy1_column_buffer_full_rd[HPX_COLUMN_NUM][HPX_ROW_NUM];
    static int hy1_column_buffer_full_wr[HPX_COLUMN_NUM][HPX_ROW_NUM];
    static int hy1_local_buffer_full_rd[HPX_ROW_NUM][HPX_COLUMN_NUM];
    static int hy1_local_buffer_full_wr[HPX_ROW_NUM][HPX_COLUMN_NUM];

    for (int i = 0; i < HPX_ROW_NUM; i++) {
        Hyperx_Network_Column_Crossbar_Single(
            i,
            hy1_column_v_id_rd[i], hy1_column_v_value_rd[i], hy1_column_v_valid_rd[i],
            hy1_local_buffer_full_rd[i],

            Read_Column[i],
            hy1_column_crossbar_v_id[i], hy1_column_crossbar_v_value[i], hy1_column_crossbar_v_valid[i]
        );
    }
    for (int i = 0; i < HPX_COLUMN_NUM; i++) {
        Hyperx_Network_Row_Crossbar_Single(
            i,
            hy1_row_v_id_rd[i], hy1_row_v_value_rd[i], hy1_row_v_valid_rd[i],
            hy1_column_buffer_full_rd[i],

            Read_Row[i],
            hy1_row_crossbar_v_id[i], hy1_row_crossbar_v_value[i], hy1_row_crossbar_v_valid[i]
        );
    }
    // bind port
    // each hyperx node catch row buffer signal of its row and column buffer signal of its column
    for (int i = 0; i < HPX_ROW_NUM; i++) { // row
        for (int j = 0; j < HPX_COLUMN_NUM; j++) { // column
            Hyperx_Network_Single(
                i, j,
                Hy1_Front_Update_V_ID[i * HPX_COLUMN_NUM + j], Hy1_Front_Update_V_Value[i * HPX_COLUMN_NUM + j], Hy1_Front_Update_V_DValid[i * HPX_COLUMN_NUM + j],
                Hy1_Front_Iteration_End[i * HPX_COLUMN_NUM + j], Hy1_Front_Iteration_End_DValid[i * HPX_COLUMN_NUM + j], Hy1_Front_Iteration_ID[i * HPX_COLUMN_NUM + j],
                Dest_Core_Full[i * HPX_COLUMN_NUM + j],
                               
                // group signal from row crossbar
                Read_Row[j][i],
                hy1_row_crossbar_v_id[j][i], hy1_row_crossbar_v_value[j][i], hy1_row_crossbar_v_valid[j][i],

                // group signal from column crossbar
                Read_Column[i][j],
                hy1_column_crossbar_v_id[i][j], hy1_column_crossbar_v_value[i][j], hy1_column_crossbar_v_valid[i][j],

                // single signal to row
                &hy1_row_v_id_wr[j][i], &hy1_row_v_value_wr[j][i], &hy1_row_v_valid_wr[j][i],
                &hy1_row_buffer_full_wr[j][i],

                // single signal to column
                &hy1_column_v_id_wr[i][j], &hy1_column_v_value_wr[i][j], &hy1_column_v_valid_wr[i][j],
                &hy1_column_buffer_full_wr[j][i], &hy1_local_buffer_full_wr[i][j],

                &Hy1_Update_V_ID[i * HPX_COLUMN_NUM + j], &Hy1_Update_V_Value[i * HPX_COLUMN_NUM + j], &Hy1_Update_V_DValid[i * HPX_COLUMN_NUM + j],
                &Hy1_Iteration_End[i * HPX_COLUMN_NUM + j], &Hy1_Iteration_End_DValid[i * HPX_COLUMN_NUM + j], &Hy1_Iteration_ID[i * HPX_COLUMN_NUM + j],
                &Hy1_Stage_Full[i * HPX_COLUMN_NUM + j]
            );
            /*
            if (Hy1_Update_V_DValid[i * HPX_COLUMN_NUM + j] && Hy1_Update_V_ID[i * HPX_COLUMN_NUM + j] == 0) {
                cout << "hyperx 0 value " << Hy1_Update_V_Value[i * HPX_COLUMN_NUM + j] << endl;
            }
            */
        }
    }

    for (int i = 0; i < HPX_ROW_NUM; i++) { // row
        for (int j = 0; j < HPX_COLUMN_NUM; j++) { // column
            hy1_row_v_id_rd[j][i] = hy1_row_v_id_wr[j][i];
            hy1_row_v_value_rd[j][i] = hy1_row_v_value_wr[j][i];
            hy1_row_v_valid_rd[j][i] = hy1_row_v_valid_wr[j][i];
            hy1_row_buffer_full_rd[j][i] = hy1_row_buffer_full_wr[j][i];

            hy1_column_v_id_rd[i][j] = hy1_column_v_id_wr[i][j];
            hy1_column_v_value_rd[i][j] = hy1_column_v_value_wr[i][j];
            hy1_column_v_valid_rd[i][j] = hy1_column_v_valid_wr[i][j];

            hy1_column_buffer_full_rd[j][i] = hy1_column_buffer_full_wr[j][i];
            hy1_local_buffer_full_rd[i][j] = hy1_local_buffer_full_wr[i][j];
        }
    }
}

void Hyperx_Network_Row_Crossbar_Single(
        int Column_ID,
        int Front_V_ID[], V_VALUE_TP Front_V_Value[], int Front_V_Valid[],
        int Column_Buffer_Full[],

        int *Read_Row,
        int *V_ID, V_VALUE_TP *V_Value, int *V_Valid) {

    // row -> row
    int shuffle_signal[HPX_ROW_NUM][HPX_ROW_NUM];

    for (int i = 0; i < HPX_ROW_NUM; i++) {
        int front_row_id = (Front_V_ID[i] % CORE_NUM) / HPX_COLUMN_NUM;
        int front_col_id = Front_V_ID[i] % HPX_COLUMN_NUM;
        for (int j = 0; j < HPX_ROW_NUM; j++) {
            shuffle_signal[i][j] = (front_row_id == j && Front_V_Valid[i]);
        }
        V_ID[i] = 0;
        V_Value[i] = 0;
        V_Valid[i] = 0;
        Read_Row[i] = 0;
    }
    int start_pe_id = clk % HPX_ROW_NUM;
    for (int j = 0; j < HPX_ROW_NUM; j++) {
        for (int i = 0; i < HPX_ROW_NUM; i++) {
            int cu_pe_id = (start_pe_id + i) % HPX_ROW_NUM;
            if (shuffle_signal[cu_pe_id][j] && !Column_Buffer_Full[j]) {
                V_ID[j] = Front_V_ID[cu_pe_id];
                V_Value[j] = Front_V_Value[cu_pe_id];
                V_Valid[j] = 1;
                Read_Row[cu_pe_id] = 1;
                break;
            }
        }
    }
}

void Hyperx_Network_Column_Crossbar_Single(
        int Row_ID,
        int Front_V_ID[], V_VALUE_TP Front_V_Value[], int Front_V_Valid[],
        int Local_Buffer_Full[],

        int *Read_Column,
        int *V_ID, V_VALUE_TP *V_Value, int *V_Valid) {

    // column -> column
    int shuffle_signal[HPX_COLUMN_NUM][HPX_COLUMN_NUM];

    for (int i = 0; i < HPX_COLUMN_NUM; i++) {
        int front_row_id = (Front_V_ID[i] % CORE_NUM) / HPX_COLUMN_NUM;
        int front_col_id = Front_V_ID[i] % HPX_COLUMN_NUM;
        for (int j = 0; j < HPX_COLUMN_NUM; j++) {
            shuffle_signal[i][j] = (front_col_id == j && Front_V_Valid[i]);
        }
        V_ID[i] = 0;
        V_Value[i] = 0;
        V_Valid[i] = 0;
        Read_Column[i] = 0;
    }
    int start_pe_id = clk % HPX_COLUMN_NUM;
    for (int j = 0; j < HPX_COLUMN_NUM; j++) {
        for (int i = 0; i < HPX_COLUMN_NUM; i++) {
            int cu_pe_id = (start_pe_id + i) % HPX_COLUMN_NUM;
            if (shuffle_signal[cu_pe_id][j] && !Local_Buffer_Full[j]) {
                V_ID[j] = Front_V_ID[cu_pe_id];
                V_Value[j] = Front_V_Value[cu_pe_id];
                V_Valid[j] = 1;
                Read_Column[cu_pe_id] = 1;
                break;
            }
        }
    }
}

void Hyperx_Network_Single(
        int HPX_Row_ID, int HPX_Column_ID,
        int Front_V_ID, V_VALUE_TP Front_V_Value, int Front_V_Valid, 
        int Front_Iteration_End, int Front_Iteration_End_DValid, int Front_Iteration_ID,
        int Next_Stage_Full, 

        int Read_Row,
        int Front_Row_V_ID, V_VALUE_TP Front_Row_V_Value, int Front_Row_V_Valid,

        int Read_Column,
        int Front_Column_V_ID, V_VALUE_TP Front_Column_V_Value, int Front_Column_V_Valid,
                        
        int *Row_V_ID, V_VALUE_TP *Row_V_Value, int *Row_V_Valid,
        int *Row_Buffer_Full,

        int *Column_V_ID, V_VALUE_TP *Column_V_Value, int *Column_V_Valid,
        int *Column_Buffer_Full, int *Local_Buffer_Full,

        int *Update_V_ID, V_VALUE_TP *Update_V_Value, int *Update_V_DValid, 
        int *Iteration_End, int *Iteration_End_DValid, int *Iteration_ID,
        int *Stage_Full) {

    static queue<int> row_v_id_buffer[HPX_ROW_NUM][HPX_COLUMN_NUM];
    static queue<V_VALUE_TP> row_v_value_buffer[HPX_ROW_NUM][HPX_COLUMN_NUM]; // from row
    static queue<int> column_v_id_buffer[HPX_ROW_NUM][HPX_COLUMN_NUM];
    static queue<V_VALUE_TP> column_v_value_buffer[HPX_ROW_NUM][HPX_COLUMN_NUM]; // from column
    static queue<int> v_id_buffer[HPX_ROW_NUM][HPX_COLUMN_NUM];
    static queue<V_VALUE_TP> v_value_buffer[HPX_ROW_NUM][HPX_COLUMN_NUM]; // from scheduler
    static int iteration_end[HPX_ROW_NUM][HPX_COLUMN_NUM], iteration_end_dvalid[HPX_ROW_NUM][HPX_COLUMN_NUM], iteration_id[HPX_ROW_NUM][HPX_COLUMN_NUM];
    static int row_buffer_empty[HPX_ROW_NUM][HPX_COLUMN_NUM], row_buffer_full[HPX_ROW_NUM][HPX_COLUMN_NUM];
    static int column_buffer_empty[HPX_ROW_NUM][HPX_COLUMN_NUM], column_buffer_full[HPX_ROW_NUM][HPX_COLUMN_NUM];
    static int buffer_empty[HPX_ROW_NUM][HPX_COLUMN_NUM], buffer_full[HPX_ROW_NUM][HPX_COLUMN_NUM];

    int Core_ID = HPX_Row_ID * HPX_COLUMN_NUM + HPX_Column_ID;
   
    #if DEBUG
        debug_M8._row_v_id_buffer = row_v_id_buffer;
        debug_M8._row_v_value_buffer = row_v_value_buffer;
        debug_M8._column_v_id_buffer = column_v_id_buffer;
        debug_M8._column_v_value_buffer = column_v_value_buffer;
        debug_M8._v_id_buffer = v_id_buffer;
        debug_M8._v_value_buffer = v_value_buffer;
    #endif

    row_buffer_empty[HPX_Row_ID][HPX_Column_ID] = (row_v_id_buffer[HPX_Row_ID][HPX_Column_ID].size() == 0);
    column_buffer_empty[HPX_Row_ID][HPX_Column_ID] = (column_v_id_buffer[HPX_Row_ID][HPX_Column_ID].size() == 0);
    buffer_empty[HPX_Row_ID][HPX_Column_ID] = (v_id_buffer[HPX_Row_ID][HPX_Column_ID].size() == 0);

    int front_row_id = (Front_V_ID % CORE_NUM) / HPX_COLUMN_NUM;
    int front_col_id = Front_V_ID % HPX_COLUMN_NUM;
    int row_row_id = (Front_Row_V_ID % CORE_NUM) / HPX_COLUMN_NUM;
    int row_col_id = Front_Row_V_ID % HPX_COLUMN_NUM;
    int column_row_id = (Front_Column_V_ID % CORE_NUM) / HPX_COLUMN_NUM;
   int column_col_id = Front_Column_V_ID % HPX_COLUMN_NUM;

    if (!rst_rd && iteration_end[HPX_Row_ID][HPX_Column_ID] && iteration_end_dvalid[HPX_Row_ID][HPX_Column_ID] &&
            buffer_empty[HPX_Row_ID][HPX_Column_ID] && column_buffer_empty[HPX_Row_ID][HPX_Column_ID] &&
            row_buffer_empty[HPX_Row_ID][HPX_Column_ID]){
        *Iteration_End = 1;
        *Iteration_End_DValid = 1;
        *Iteration_ID = Front_Iteration_ID;
    } else {
        *Iteration_End = 0;
        *Iteration_End_DValid = 0;
        *Iteration_ID = Front_Iteration_ID;
    }
    
    if(!rst_rd && Front_Iteration_End && Front_Iteration_End_DValid && buffer_empty[HPX_Row_ID][HPX_Column_ID] &&
            row_buffer_empty[HPX_Row_ID][HPX_Column_ID] && column_buffer_empty[HPX_Row_ID][HPX_Column_ID]) {
        iteration_end[HPX_Row_ID][HPX_Column_ID] = 1;
        iteration_end_dvalid[HPX_Row_ID][HPX_Column_ID] = 1;
    } else {
        iteration_end[HPX_Row_ID][HPX_Column_ID] = 0;
        iteration_end_dvalid[HPX_Row_ID][HPX_Column_ID] = 0;
    }

    // v_id out
    if (rst_rd || Next_Stage_Full || buffer_empty[HPX_Row_ID][HPX_Column_ID]) {
        *Update_V_ID = 0;
        *Update_V_Value = 0;
        *Update_V_DValid = 0;
    } else {
        *Update_V_ID = v_id_buffer[HPX_Row_ID][HPX_Column_ID].front();
        *Update_V_Value = v_value_buffer[HPX_Row_ID][HPX_Column_ID].front();
        *Update_V_DValid = 1;
        if (*Update_V_ID == 1) {
            // cout << "core " << Core_ID << " local send 1 value " << *Update_V_Value << endl;
        }
        
        v_id_buffer[HPX_Row_ID][HPX_Column_ID].pop();
        v_value_buffer[HPX_Row_ID][HPX_Column_ID].pop();
    }

    // row v_id_buffer out
    if (rst_rd || row_buffer_empty[HPX_Row_ID][HPX_Column_ID]) {
    } else {
        // row buffer will transform to other column buffer, check status
        if (Read_Row) {
            if (row_buffer_empty[HPX_Row_ID][HPX_Column_ID]) {
                cout << "[ERROR] read row buffer but buffer is empty in core " << Core_ID << endl;
                exit(-1);
            }
            if (row_v_id_buffer[HPX_Row_ID][HPX_Column_ID].front() == 1) {
                // cout << "core " << Core_ID << " row send 1 value " << *Row_V_Value << endl;
            }
            row_v_id_buffer[HPX_Row_ID][HPX_Column_ID].pop();
            row_v_value_buffer[HPX_Row_ID][HPX_Column_ID].pop();
        }
    }

    // column v_id_buffer out
    if (rst_rd) {
    } else {
        // Column buffer will transform to other output buffer, check status
        if (Read_Column) {
            if (column_buffer_empty[HPX_Row_ID][HPX_Column_ID]) {
                cout << "[ERROR] read column buffer but buffer is empty in core " << Core_ID << endl;
                exit(-1);
            }
            if (column_v_id_buffer[HPX_Row_ID][HPX_Column_ID].front() == 1) {
                // cout << "core " << Core_ID << " column send 1 value " << *Column_V_Value << endl;
            }
            column_v_id_buffer[HPX_Row_ID][HPX_Column_ID].pop();
            column_v_value_buffer[HPX_Row_ID][HPX_Column_ID].pop();
        }
    }

    // buffer input
    // just column input to local buffer
    if (rst_rd) {

    } else {
        // should store signal even buffer is full
        if (Front_Column_V_Valid) {
            if (column_row_id != HPX_Row_ID || column_col_id != HPX_Column_ID) {
                printf("[ERROR] core %d: store %d to local buffer\n", Front_Column_V_ID);
                exit(-1);
            }
            if (Front_Column_V_ID == 1) {
                // cout << "core " << Core_ID << " local store 1 value " << Front_Column_V_Value << endl;
            }
            v_id_buffer[HPX_Row_ID][HPX_Column_ID].push(Front_Column_V_ID);
            v_value_buffer[HPX_Row_ID][HPX_Column_ID].push(Front_Column_V_Value);
        }
    }

    // row buffer input
    // just scheduler input signal to row buffer
    if (rst_rd) {

    } else {
        if (Front_V_Valid) {
            if (Front_V_ID == 1) {
                // cout << "core " << Core_ID << " row store 1 value " << Front_V_Value << endl;
            }
            row_v_id_buffer[HPX_Row_ID][HPX_Column_ID].push(Front_V_ID);
            row_v_value_buffer[HPX_Row_ID][HPX_Column_ID].push(Front_V_Value);
        }
    }

    // column buffer input
    // row input signal to column buffer
    if (rst_rd) {

    } else {
        // should store signal even buffer is full
        if (Front_Row_V_Valid) {
            if (row_row_id != HPX_Row_ID) {
                printf("[ERROR] clk %d core %d: store %d to column buffer\n", clk, Front_Row_V_ID);
                exit(-1);
            }
            if (Front_Row_V_ID == 1) {
                // cout << "core " << Core_ID << " column store 1 value " << Front_Row_V_Value << endl;
            }
            column_v_id_buffer[HPX_Row_ID][HPX_Column_ID].push(Front_Row_V_ID);
            column_v_value_buffer[HPX_Row_ID][HPX_Column_ID].push(Front_Row_V_Value);
        }
    }

    // assign output
    *Row_V_ID = row_v_id_buffer[HPX_Row_ID][HPX_Column_ID].front();
    *Row_V_Value = row_v_value_buffer[HPX_Row_ID][HPX_Column_ID].front();
    *Row_V_Valid = !row_v_value_buffer[HPX_Row_ID][HPX_Column_ID].empty();
    *Column_V_ID = column_v_id_buffer[HPX_Row_ID][HPX_Column_ID].front();
    *Column_V_Value = column_v_value_buffer[HPX_Row_ID][HPX_Column_ID].front();
    *Column_V_Valid = !column_v_id_buffer[HPX_Row_ID][HPX_Column_ID].empty();

    *Local_Buffer_Full = (v_id_buffer[HPX_Row_ID][HPX_Column_ID].size() >= FIFO_SIZE);
    *Row_Buffer_Full = (row_v_id_buffer[HPX_Row_ID][HPX_Column_ID].size() >= FIFO_SIZE);
    *Column_Buffer_Full = (column_v_id_buffer[HPX_Row_ID][HPX_Column_ID].size() >= FIFO_SIZE);
    *Stage_Full = (*Row_Buffer_Full);

}