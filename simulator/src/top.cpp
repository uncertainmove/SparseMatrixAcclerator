#define main_part

#include "accelerator.h"
#include "structure.h"
#include "self_config.h"
#include <bits/stdc++.h>

using namespace std;

int MAX_CYCLE = 100000000;
int clk;
int rst_rd;
int MAX_ITERATION = -1;
int VTX_NUM = -1, EDGE_NUM = -1;

extern int v_updated;
extern int iteration_end_flag;
extern int noc_end;

void top(int argc, char **argv)
{
    if (argc < 8) {
        cout << "Usage: ./acc_sim off_file list_file max_iteration v_num e_num has_header debug_file" << endl;
        cout << argc << endl;
        for (int i = 0; i < argc; i++) {
            cout << argv[i] << endl;
        }
        return;
    }
    VTX_NUM = atoi (argv[4]);
    EDGE_NUM = atoi (argv[5]);
    int has_header = atoi (argv[6]);
    Initialize_Input_Graph(VTX_NUM, EDGE_NUM, argv[1], argv[2], has_header);
    MAX_ITERATION = atoi (argv[3]);
    cout << "Load Graph, vertex_num=" << VTX_NUM << ", edge_num=" << EDGE_NUM << endl;
    Initialize_Offset_Uram(VTX_NUM);
    Initialize_Delta_Bram();
    Initialize_Vertex_Degree(VTX_NUM);
    Initialize_PR_Uram();

    for (clk = 0; clk < MAX_CYCLE; ++ clk) {
        // rst_rd must be reset only one clk after the reseting of rst_root
        rst_rd = (clk < 100) ? 1 : 0;

        if (clk % 100000 == 0) {
            cout << "clk " << clk << " v_update = " << v_updated << endl;
            cout << "clk " << clk << " noc_transfer_ct = " << v_updated << endl;
        }
        if (clk % 10000 == 0 && noc_end) {
            cout << "clk " << clk << " noc end" << endl;
        }
        if (iteration_end_flag) {
            cout << EDGE_NUM << " " << 1.0 * EDGE_NUM * MAX_ITERATION  * 0.13 / clk << "GTEPS" << endl;
            return;
        }

        Compute_Core(&pipe_mg);
        NoC(&pipe_mg);
        Memory(&pipe_mg);
        pipe_mg.run();
    }
}