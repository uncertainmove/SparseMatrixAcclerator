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
    res_fp = fopen("res.txt", "w");
    if (res_fp == NULL) {
        cout << "[ERROR] Failed to open res file!" << endl;
        return -1;
    }
    for (int i = 0; i < VTX_NUM; i++) {
        fprintf(res_fp, "%d %.9f\n", i, double(PR_URAM[i % CORE_NUM][i / CORE_NUM][0]));
    }
    cout << "[INFO] Store res file complete!" << endl;
    return 0;
}