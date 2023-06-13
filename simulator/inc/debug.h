#ifndef DEBUG_H
#define DEBUG_H

#include "parameter.h"
#include "structure.h"

// set debug to open debug mode
#define DEBUG 1

using namespace std;

class debug_RD_ACTIVE_VERTEX {
public:
    int *_iteration_id;
    int *_init_bitmap_id;
    int *_iteration_end;

    bitmap (*_vis_bitmap)[2][BitMap_Compressed_NUM + 1];
    int *_vis_bitmap_index;
    bitmap *_vis_bitmap_now;
};

class debug_RD_ACTIVE_VERTEX_OFFSET {
public:
    queue<int> *_v_id_buffer;
};

class debug_RD_Offset_Uram {
};

class debug_RD_Active_Vertex_Edge {
public:
    queue<int> *_v_id_buffer;
    queue<V_VALUE_TP> *_v_value_buffer;
    queue<int> *_v_loffset_buffer;
    queue<int> *_v_roffset_buffer;
    int *_now_loffset;
};

class debug_Generate_HBM_Edge_Rqst {
public:
    queue<int> (*_edge_addr_buffer)[GROUP_CORE_NUM];
    queue<BitVector_16> (*_edge_mask_buffer)[GROUP_CORE_NUM];
    queue<int> (*_v_id_buffer)[GROUP_CORE_NUM];
    queue<V_VALUE_TP> (*_v_value_buffer)[GROUP_CORE_NUM];
};

class debug_HBM_Interface {
public:
    queue<int> *_edge_addr_buffer;
    queue<Cacheline_16> *_edge_buffer;
};

class debug_Schedule {
public:
    queue<int> *_v_id_buffer;
    queue<V_VALUE_TP> *_v_value_buffer;
    queue<int> *_edge_buffer;
};

class debug_Noc {
public:
    queue<int> (*_vertical_v_id_buffer)[HPX_HORIZONTAL_NUM];
    queue<V_VALUE_TP> (*_vertical_v_value_buffer)[HPX_HORIZONTAL_NUM];
    queue<int> (*_horizontal_v_id_buffer)[HPX_HORIZONTAL_NUM];
    queue<V_VALUE_TP> (*_horizontal_v_value_buffer)[HPX_HORIZONTAL_NUM];
    queue<int> (*_v_id_buffer)[HPX_HORIZONTAL_NUM];
    queue<V_VALUE_TP> (*_v_value_buffer)[HPX_HORIZONTAL_NUM];
};

class debug_Backend_Core_Process {
public:
};

class debug_Vertex_RAM_Controller {
public:
    queue<int> *_wr_delta_addr_buffer;
    queue<V_VALUE_TP> *_wr_delta_value_buffer;
    queue<int> *_wr_pr_addr_buffer;
    queue<V_VALUE_TP> *_wr_pr_value_buffer;
    queue<int> *_active_addr_buffer;
    queue<V_VALUE_TP> *_active_delta_buffer;
    queue<V_VALUE_TP> *_active_pr_buffer;
};

#endif