#include "accelerator.h"
#include "parameter.h"
#include "memory.h"
#include <bits/stdc++.h>

using namespace std;

extern int VTX_NUM, EDGE_NUM;
extern V_VALUE_TP PR_URAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1][2];

int main(int argc, char **argv)
{
    FILE *res_fp;
    top(argc, argv);
    res_fp = fopen("simulator_res.txt", "w");
    if (res_fp == NULL) {
        cout << "[ERROR] Failed to open res file!" << endl;
        return -1;
    }
    for (int i = 0; i < VTX_NUM; i++) {
        fprintf(res_fp, "%d %x\n", i, PR_URAM[i % CORE_NUM][i / CORE_NUM][0]);
    }
    fclose(res_fp);
    cout << "[INFO] Store res file complete!" << endl;
    cout << "Start Check Graph Result" << endl;
    FILE *in_debug_res = fopen(argv[7], "r");
    if (!in_debug_res) {
      cout << "[ERROR] Failed to open " << argv[7] << endl;
      exit(-1);
    }
    int debug_res_id, debug_res_data;
    int error_counter = 0;
    for (int i = 0; i < VTX_NUM; i++) {
        fscanf(in_debug_res, "%d %x", &debug_res_id, &debug_res_data);
        float ret_pr_v = *(float *)&PR_URAM[i % CORE_NUM][i / CORE_NUM][0];
        float debug_pr_v = *(float *)&debug_res_data;
        float divation = ret_pr_v - debug_pr_v > 0 ? ret_pr_v - debug_pr_v : debug_pr_v - ret_pr_v;
        float delta = debug_pr_v - ret_pr_v;
        if (divation / debug_pr_v < 0 || divation / debug_pr_v > 0.0005) {
            error_counter++;
            printf("id: %d, debug_res: %f, acc_res: %f, delta: %x, err: %f.\n", i, debug_pr_v, *(int *)&delta, ret_pr_v, divation / debug_pr_v);
        }
        if (error_counter > 5) {
            cout << "Too many errors found. Exiting check of result." << endl;
            break;
        }
    }
    cout << "Complete Check Graph Result" << endl;
    return 0;
}