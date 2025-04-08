#include "core.h"
#include "parameter.h"
#include "debug.h"
#include <bits/stdc++.h>

#if DEBUG
    debug_Vertex_RAM_Controller debug_M10;
#endif

extern int clk;
extern int rst_rd;
// extern FILE* debug_fp;

using namespace std;

void Vertex_RAM_Controller_Single(
        int Core_ID,
        int Rd_Addr_Src, int Rd_Valid_Src,
        int Wr_Addr_Dst, V_VALUE_TP Wr_V_Value_Dst, int Wr_Valid_Dst,
        int Front_Iteration_End, int Front_Iteration_End_DValid,
        int Front_Iteration_ID,

        V_VALUE_TP Delta1_Bram_Data, int Delta1_Bram_Data_Valid,
        V_VALUE_TP Delta2_Bram_Data, int Delta2_Bram_Data_Valid,
        V_VALUE_TP PR_Uram_Data, int PR_Uram_Data_Valid,
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

// contain 2 delta bram and 1 pr uram
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
        int *Iteration_End, int *Iteration_End_DValid) {

    for (int i = 0; i < CORE_NUM; ++ i) {
        Vertex_RAM_Controller_Single(
            i,
            Rd_Addr_Src[i], Rd_Valid_Src[i],
            Wr_Addr_Dst[i], Wr_V_Value_Dst[i], Wr_Valid_Dst[i],
            Front_Iteration_End[i], Front_Iteration_End_DValid[i],
            Front_Iteration_ID[i],

            Delta1_Bram_Data[i], Delta1_Bram_Data_Valid[i],
            Delta2_Bram_Data[i], Delta2_Bram_Data_Valid[i],
            PR_Uram_Data[i], PR_Uram_Data_Valid[i],
            &Delta1_Bram_Rd_Addr[i], &Delta1_Bram_Rd_Valid[i],
            &Delta2_Bram_Rd_Addr[i], &Delta2_Bram_Rd_Valid[i],
            &Delta1_Bram_Wr_Addr[i], &Delta1_Bram_Wr_Value[i], &Delta1_Bram_Wr_Valid[i],
            &Delta2_Bram_Wr_Addr[i], &Delta2_Bram_Wr_Value[i], &Delta2_Bram_Wr_Valid[i],
            &PR_Uram_Rd_Addr[i], &PR_Uram_Rd_Valid[i],
            &PR_Uram_Wr_Addr[i], &PR_Uram_Wr_Value[i], &PR_Uram_Wr_Valid[i],

            &Stage_Full[i],
            &Src_Recv_Update_V_Value[i], &Src_Recv_Update_V_DValid[i],
            &Backend_Active_V_ID[i], &Backend_Active_V_Updated[i], &Backend_Active_V_DValid[i],
            &Iteration_End[i], &Iteration_End_DValid[i]);
    }
}

void Vertex_RAM_Controller_Single(
        int Core_ID,
        int Rd_Addr_Src, int Rd_Valid_Src,
        int Wr_Addr_Dst, V_VALUE_TP Wr_V_Value_Dst, int Wr_Valid_Dst,
        int Front_Iteration_End, int Front_Iteration_End_DValid,
        int Front_Iteration_ID,

        V_VALUE_TP Delta1_Bram_Data, int Delta1_Bram_Data_Valid,
        V_VALUE_TP Delta2_Bram_Data, int Delta2_Bram_Data_Valid,
        V_VALUE_TP PR_Uram_Data, int PR_Uram_Data_Valid,
        int *Delta1_Bram_Rd_Addr, int *Delta1_Bram_Rd_Valid,
        int *Delta2_Bram_Rd_Addr, int *Delta2_Bram_Rd_Valid,
        int *Delta1_Bram_Wr_Addr, V_VALUE_TP *Delta1_Bram_Wr_Value, int *Delta1_Bram_Wr_Valid,
        int *Delta2_Bram_Wr_Addr, V_VALUE_TP *Delta2_Bram_Wr_Value, int *Delta2_Bram_Wr_Valid,
        int *PR_Uram_Rd_Addr, int *PR_Uram_Rd_Valid,
        int *PR_Uram_Wr_Addr, V_VALUE_TP *PR_Uram_Wr_Value, int *PR_Uram_Wr_Valid,

        int *Stage_Full,
        V_VALUE_TP *Src_Recv_Update_V_Value, int *Src_Recv_Update_V_DValid,
        int *Backend_Active_V_ID, int *Backend_Active_V_Updated, int *Backend_Active_V_DValid,
        int *Iteration_End, int *Iteration_End_DValid) {
    //HPX1 (src) only read
    //HPX2 (dst) write-after-read
    // delta1 and delta2 will exchange in each iteration
    static queue<int> wr_delta_addr_buffer[CORE_NUM];
    static queue<V_VALUE_TP> wr_delta_value_buffer[CORE_NUM];
    static queue<int> wr_pr_addr_buffer[CORE_NUM];
    static queue<V_VALUE_TP> wr_pr_value_buffer[CORE_NUM];
    static queue<int> active_addr_buffer[CORE_NUM];
    static queue<V_VALUE_TP> active_delta_buffer[CORE_NUM], active_pr_buffer[CORE_NUM];
    static int delta_buffer_empty[CORE_NUM], pr_buffer_empty[CORE_NUM];
    static int active_addr_buffer_empty[CORE_NUM], active_delta_buffer_empty[CORE_NUM], active_pr_buffer_empty[CORE_NUM];
    // record wr sequence, delay read when conflict occur
    static int conflict_check_buffer[CORE_NUM][FLOAT_ADD_DELAY], conflict_valid_buffer[CORE_NUM][FLOAT_ADD_DELAY];

    #if DEBUG
        debug_M10._wr_delta_addr_buffer = wr_delta_addr_buffer;
        debug_M10._wr_delta_value_buffer = wr_delta_value_buffer;
        debug_M10._wr_pr_addr_buffer = wr_pr_addr_buffer;
        debug_M10._wr_pr_value_buffer = wr_pr_value_buffer;
        debug_M10._active_addr_buffer = active_addr_buffer;
        debug_M10._active_delta_buffer = active_delta_buffer;
        debug_M10._active_pr_buffer = active_pr_buffer;
    #endif

    delta_buffer_empty[Core_ID] = (wr_delta_addr_buffer[Core_ID].size() == 0);
    pr_buffer_empty[Core_ID] = (wr_pr_addr_buffer[Core_ID].size() == 0);
    active_addr_buffer_empty[Core_ID] = (active_addr_buffer[Core_ID].size() == 0);
    active_delta_buffer_empty[Core_ID] = (active_delta_buffer[Core_ID].size() == 0);
    active_pr_buffer_empty[Core_ID] = (active_pr_buffer[Core_ID].size() == 0);

    bool conflict = 0;
    if (!delta_buffer_empty[Core_ID]) {
        for (int i = 0; i < FLOAT_ADD_DELAY; i++) {
            conflict |= (conflict_valid_buffer[Core_ID][i] && conflict_check_buffer[Core_ID][i] == wr_delta_addr_buffer[Core_ID].front());
        }
    }

    // if (Core_ID == 25 && conflict_valid_buffer[25][FLOAT_ADD_DELAY - 1]) {
        // fprintf(debug_fp, "clk %d dst %d\n", clk, conflict_check_buffer[25][FLOAT_ADD_DELAY - 1]);
    // }

    // 1. output 
    if (rst_rd) {
        *Src_Recv_Update_V_Value = 0;
        *Src_Recv_Update_V_DValid = 0;
    } else {
        if (Delta1_Bram_Data_Valid && Front_Iteration_ID % 2 == 0) {
            *Src_Recv_Update_V_Value = Delta1_Bram_Data;
            *Src_Recv_Update_V_DValid = 1;
        } else if (Delta2_Bram_Data_Valid && Front_Iteration_ID % 2 == 1) {
            *Src_Recv_Update_V_Value = Delta2_Bram_Data;
            *Src_Recv_Update_V_DValid = 1;
        } else {
            *Src_Recv_Update_V_Value = 0;
            *Src_Recv_Update_V_DValid = 0;
        }
    }

    // 2. active vertex
    if (rst_rd) {
        *Backend_Active_V_ID = 0;
        *Backend_Active_V_Updated = 0;
        *Backend_Active_V_DValid = 0;
    } else {
        if (!active_addr_buffer_empty[Core_ID] && !active_delta_buffer_empty[Core_ID] && !active_pr_buffer_empty[Core_ID]) {
            if (Front_Iteration_ID == 0) {
                // use float ip in rtl
                *Backend_Active_V_Updated = V_VALUE_TP(fabs(active_delta_buffer[Core_ID].front()._v_value)) > E2 * active_pr_buffer[Core_ID].front();
            } else {
                *Backend_Active_V_Updated = V_VALUE_TP(fabs(active_delta_buffer[Core_ID].front()._v_value)) > E2 * (active_pr_buffer[Core_ID].front() - active_delta_buffer[Core_ID].front());
            }
            *Backend_Active_V_ID = active_addr_buffer[Core_ID].front();
            *Backend_Active_V_DValid = 1;
            // cout << "clk: " << clk << ", M10 update active bitmap, id: " << *Backend_Active_V_ID <<
                // ", active: " << *Backend_Active_V_Updated << endl;

            active_addr_buffer[Core_ID].pop();
            active_delta_buffer[Core_ID].pop();
            active_pr_buffer[Core_ID].pop();
        } else {
            *Backend_Active_V_ID = 0;
            *Backend_Active_V_Updated = 0;
            *Backend_Active_V_DValid = 0;
        }
    }

    // 2. send rd request to delta & pr bram
    // iteration_id % 2 == 0 : send rd request to delta 1 bram; send wr request to delta 2 bram, rd first
    // iteration_id % 2 == 1 : send wr request to delta 1 bram, rd first; send rd request to delta 2 bram
    if (rst_rd) {
        *Delta1_Bram_Rd_Addr = 0;
        *Delta1_Bram_Rd_Valid = 0;

        *Delta2_Bram_Rd_Addr = 0;
        *Delta2_Bram_Rd_Valid = 0;

        *PR_Uram_Rd_Addr = 0;
        *PR_Uram_Rd_Valid = 0;

        for (int i = 0; i < FLOAT_ADD_DELAY; i++) {
            conflict_check_buffer[Core_ID][i] = 0;
            conflict_valid_buffer[Core_ID][i] = 0;
        }
    } else {
        // shift left conflict buffer
        for (int i = FLOAT_ADD_DELAY - 1; i > 0; i--) {
            conflict_check_buffer[Core_ID][i] = conflict_check_buffer[Core_ID][i - 1];
            conflict_valid_buffer[Core_ID][i] = conflict_valid_buffer[Core_ID][i - 1];
        }
        // delta 1
        if (Front_Iteration_ID % 2 == 0 && Rd_Valid_Src) {
            *Delta1_Bram_Rd_Addr = Rd_Addr_Src;
            *Delta1_Bram_Rd_Valid = 1;
            // cout << "send rd request to delta1" << endl;
        } else if (Front_Iteration_ID % 2 == 1 && !delta_buffer_empty[Core_ID] && !conflict) { // rd after conflict check
            *Delta1_Bram_Rd_Addr = wr_delta_addr_buffer[Core_ID].front() / CORE_NUM;
            *Delta1_Bram_Rd_Valid = 1;
            // cout << "send wr_rd request to delta1" << endl;
        } else {
            *Delta1_Bram_Rd_Addr = 0;
            *Delta1_Bram_Rd_Valid = 0;
        }
        // delta 2
        if (Front_Iteration_ID % 2 == 1 && Rd_Valid_Src) {
            *Delta2_Bram_Rd_Addr = Rd_Addr_Src;
            *Delta2_Bram_Rd_Valid = 1;
        } else if (Front_Iteration_ID % 2 == 0 && !delta_buffer_empty[Core_ID] && !conflict) { // rd after conflict check
            *Delta2_Bram_Rd_Addr = wr_delta_addr_buffer[Core_ID].front() / CORE_NUM;
            *Delta2_Bram_Rd_Valid = 1;
        } else {
            *Delta2_Bram_Rd_Addr = 0;
            *Delta2_Bram_Rd_Valid = 0;
        }
        // pr
        if (!delta_buffer_empty[Core_ID] && !conflict) {
            *PR_Uram_Rd_Addr = wr_delta_addr_buffer[Core_ID].front() / CORE_NUM;
            *PR_Uram_Rd_Valid = 1;

            conflict_check_buffer[Core_ID][0] = wr_delta_addr_buffer[Core_ID].front();
            conflict_valid_buffer[Core_ID][0] = 1;
            wr_delta_addr_buffer[Core_ID].pop();
            wr_delta_value_buffer[Core_ID].pop();
        } else {
            *PR_Uram_Rd_Addr = 0;
            *PR_Uram_Rd_Valid = 0;
            conflict_check_buffer[Core_ID][0] = 0;
            conflict_valid_buffer[Core_ID][0] = 0;
        }

        if (Wr_Valid_Dst) {
            wr_delta_addr_buffer[Core_ID].push(Wr_Addr_Dst);
            wr_delta_value_buffer[Core_ID].push(Wr_V_Value_Dst);
            wr_pr_addr_buffer[Core_ID].push(Wr_Addr_Dst);
            wr_pr_value_buffer[Core_ID].push(Wr_V_Value_Dst);
        }
        /*
        if (Core_ID == 0 && (*PR_Uram_Rd_Valid || *Delta1_Bram_Rd_Valid || *Delta2_Bram_Rd_Valid)) {
            cout << "clk " << clk << " send rd request, core_id=" << Core_ID << ", valid=" << *PR_Uram_Rd_Valid <<
                    *Delta1_Bram_Rd_Valid << *Delta2_Bram_Rd_Valid << endl;
        }
        */
    }

    // 3. send wr request to delta & pr bram
    // iteration_id % 2 == 0 : send wr request to delta 2 bram
    // iteration_id % 2 == 1 : send wr request to delta 1 bram
    if (rst_rd) {
        *Delta1_Bram_Wr_Addr = 0;
        *Delta1_Bram_Wr_Value = 0;
        *Delta1_Bram_Wr_Valid = 0;

        *Delta2_Bram_Wr_Addr = 0;
        *Delta2_Bram_Wr_Value = 0;
        *Delta2_Bram_Wr_Valid = 0;

        *PR_Uram_Wr_Addr = 0;
        *PR_Uram_Wr_Value = 0;
        *PR_Uram_Wr_Valid = 0;

        for (int i = 0; i < FLOAT_ADD_DELAY; i++) {
            conflict_check_buffer[Core_ID][i] = 0;
            conflict_valid_buffer[Core_ID][i] = 0;
        }
    } else {
        if (Front_Iteration_ID % 2 == 0 && Delta2_Bram_Data_Valid) {
            // delta 2
            if (wr_pr_addr_buffer[Core_ID].empty()) {
                cout << "iteration " << Front_Iteration_ID << " error 1 in stage 10, core_id=" << Core_ID << endl;
                cout << "Front_Iteration_ID=" << Front_Iteration_ID << ", valid2=" << Delta2_Bram_Data_Valid << endl;
                exit(-1);
            }
            *Delta2_Bram_Wr_Addr = wr_pr_addr_buffer[Core_ID].front() / CORE_NUM;
            *Delta2_Bram_Wr_Value = wr_pr_value_buffer[Core_ID].front() +
                ((Delta2_Bram_Data._iteration_id == Front_Iteration_ID) ? Delta2_Bram_Data : V_VALUE_TP(0.0)); // use float add ip in rtl
            Delta2_Bram_Wr_Value->_iteration_id = Front_Iteration_ID;
            *Delta2_Bram_Wr_Valid = 1;

            *Delta1_Bram_Wr_Addr = 0;
            *Delta1_Bram_Wr_Value = 0;
            *Delta1_Bram_Wr_Valid = 0;

            active_addr_buffer[Core_ID].push(wr_pr_addr_buffer[Core_ID].front());
            active_delta_buffer[Core_ID].push(*Delta2_Bram_Wr_Value);
        } else if (Front_Iteration_ID % 2 == 1 && Delta1_Bram_Data_Valid) {
            // delta 1
            if (wr_pr_addr_buffer[Core_ID].empty()) {
                cout << "iteration " << Front_Iteration_ID << " error 1 in stage 10, core_id=" << Core_ID << endl;
                cout << "valid1=" << Delta1_Bram_Data_Valid << endl;
                exit(-1);
            }
            *Delta1_Bram_Wr_Addr = wr_pr_addr_buffer[Core_ID].front() / CORE_NUM;
            *Delta1_Bram_Wr_Value = wr_pr_value_buffer[Core_ID].front() +
                ((Delta1_Bram_Data._iteration_id == Front_Iteration_ID) ? Delta1_Bram_Data : V_VALUE_TP(0.0)); // use float add ip in rtl
            Delta1_Bram_Wr_Value->_iteration_id = Front_Iteration_ID;
            *Delta1_Bram_Wr_Valid = 1;

            *Delta2_Bram_Wr_Addr = 0;
            *Delta2_Bram_Wr_Value = 0;
            *Delta2_Bram_Wr_Valid = 0;

            active_addr_buffer[Core_ID].push(wr_pr_addr_buffer[Core_ID].front());
            active_delta_buffer[Core_ID].push(*Delta1_Bram_Wr_Value);
        } else {
            *Delta1_Bram_Wr_Addr = 0;
            *Delta1_Bram_Wr_Value = 0;
            *Delta1_Bram_Wr_Valid = 0;

            *Delta2_Bram_Wr_Addr = 0;
            *Delta2_Bram_Wr_Value = 0;
            *Delta2_Bram_Wr_Valid = 0;
        }
        // pr
        if (PR_Uram_Data_Valid) {
            *PR_Uram_Wr_Addr = wr_pr_addr_buffer[Core_ID].front() / CORE_NUM;
            *PR_Uram_Wr_Value = wr_pr_value_buffer[Core_ID].front() + PR_Uram_Data;
            *PR_Uram_Wr_Valid = 1;
            // cout << "clk: " << clk << ", M10 write pr, addr: " << wr_pr_addr_buffer[Core_ID].front() << ", value: " <<
                // *(float *)PR_Uram_Wr_Value << ", front_v: " << *(float *)&wr_pr_value_buffer[Core_ID].front() <<
                // ", front_u: " << *(float *)&PR_Uram_Data << endl;

            active_pr_buffer[Core_ID].push(*PR_Uram_Wr_Value);
        } else {
            *PR_Uram_Wr_Addr = 0;
            *PR_Uram_Wr_Value = 0;
            *PR_Uram_Wr_Valid = 0;
        }

        if (PR_Uram_Data_Valid && ((Front_Iteration_ID % 2 == 0 && Delta2_Bram_Data_Valid) ||
                (Front_Iteration_ID % 2 == 1 && Delta1_Bram_Data_Valid))) {
            wr_pr_addr_buffer[Core_ID].pop();
            wr_pr_value_buffer[Core_ID].pop();
        } else {
            if (PR_Uram_Data_Valid && !((Front_Iteration_ID % 2 == 0 && Delta2_Bram_Data_Valid) ||
                    (Front_Iteration_ID % 2 == 1 && Delta1_Bram_Data_Valid))) {
                cout << "error 2 in stage 10, core_id=" << Core_ID << ", valid=" << PR_Uram_Data_Valid
                        << Delta1_Bram_Data_Valid << Delta2_Bram_Data_Valid << endl;
                exit(-1);
            }
        }
        /*
        if (Core_ID == 0 && (*PR_Uram_Wr_Valid || *Delta1_Bram_Wr_Valid || *Delta2_Bram_Wr_Valid)) {
            cout << "clk " << clk << " send wr request, core_id=" << Core_ID << ", valid=" << *PR_Uram_Wr_Valid <<
                    *Delta1_Bram_Wr_Valid << *Delta2_Bram_Wr_Valid << endl;
        }
        */
    }

    delta_buffer_empty[Core_ID] = (wr_delta_addr_buffer[Core_ID].size() == 0);
    pr_buffer_empty[Core_ID] = (wr_pr_addr_buffer[Core_ID].size() == 0);
    active_addr_buffer_empty[Core_ID] = (active_addr_buffer[Core_ID].size() == 0);
    active_delta_buffer_empty[Core_ID] = (active_delta_buffer[Core_ID].size() == 0);
    active_pr_buffer_empty[Core_ID] = (active_pr_buffer[Core_ID].size() == 0);

    static int end_ct[CORE_NUM];
    if (!rst_rd && Front_Iteration_End && Front_Iteration_End_DValid && delta_buffer_empty[Core_ID] && pr_buffer_empty[Core_ID] && active_addr_buffer_empty[Core_ID]) {
        if (end_ct[Core_ID] >= WAIT_END_DELAY) {
            *Iteration_End = 1;
            *Iteration_End_DValid = 1;
        } else {
            end_ct[Core_ID]++;
            *Iteration_End = 0;
            *Iteration_End_DValid = 0;
        }
    }
    else {
        end_ct[Core_ID] = 0;
        *Iteration_End = 0;
        *Iteration_End_DValid = 0;
    }

    *Stage_Full = (wr_pr_addr_buffer[Core_ID].size() >= 32);
}