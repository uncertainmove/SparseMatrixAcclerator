#ifndef ACCELERATOR_H
#define ACCELERATOR_H

#include "parameter.h"
#include "framework.h"
#include "debug.h"
#include <bits/stdc++.h>

using namespace std;

void top(int argc, char **argv);

void Initialize_Input_Graph(int v_num, int e_num, char *off_file, char *edge_file, int has_header);

void Initialize_Offset_Uram(int V_Num);

void Initialize_Delta_Bram();

void Initialize_PR_Uram();

void Initialize_Vertex_Degree(int V_Num);

void Validation_PR_Uram(int root_id);

void Compute_Core(pipe_manager* pipes);

void Memory(pipe_manager* pipes);

void NoC(pipe_manager* pipes);

#endif // ACCELERATOR_H