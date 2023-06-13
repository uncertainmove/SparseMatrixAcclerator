#define main_part

#include "accelerator.h"
#include "structure.h"
#include "self_config.h"
#include <bits/stdc++.h>

using namespace std;

int MAX_CYCLE = 10000000;
int clk;
int rst_rd;
int MAX_ITERATION = 14;
int VTX_NUM = 1048576, EDGE_NUM = 31400738;

extern int v_updated;
extern int iteration_end_flag;

void top(int argc, char **argv)
{
    if (argc < 3) {
        cout << "Usage: ./acc_sim off_file list_file" << endl;
        return;
    }
    Initialize_Input_Graph(argv[1], argv[2]);
    Initialize_Offset_Uram(VTX_NUM);
    Initialize_Delta_Bram();
    Initialize_Vertex_Degree(VTX_NUM);
    Initialize_PR_Uram();

    for (clk = 0; clk < MAX_CYCLE; ++ clk) {
        // rst_rd must be reset only one clk after the reseting of rst_root
        rst_rd = (clk < 100) ? 1 : 0;

        if (clk % 1000000 == 0) {
            cout << "clk " << clk << " v_update = " << v_updated << endl;
        }
        if (iteration_end_flag) {
            return;
        }

        Compute_Core(&pipe_mg);
        NoC(&pipe_mg);
        Memory(&pipe_mg);
        pipe_mg.run();
    }
}