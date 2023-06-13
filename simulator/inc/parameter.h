#ifndef PARAMETER_H
#define PARAMETER_H

#include <iostream>

using std::ostream;

struct V_VALUE_TP {
    float _v_value;
    int _iteration_id;
    V_VALUE_TP(float value) { _v_value = value; _iteration_id = 0; }
    V_VALUE_TP() { _v_value = 0.0; _iteration_id = 0; }
    /*
    V_VALUE_TP operator+(const V_VALUE_TP& v_value) const {
        return _v_value + v_value._v_value;
    }
    V_VALUE_TP operator-(const V_VALUE_TP& v_value) const {
        return _v_value - v_value._v_value;
    }
    V_VALUE_TP operator*(const V_VALUE_TP& v_value) const {
        return _v_value * v_value._v_value;
    }
    V_VALUE_TP operator/(const V_VALUE_TP& v_value) const {
        return _v_value / v_value._v_value;
    }
    bool operator>(const V_VALUE_TP& v_value) const {
        return _v_value > v_value._v_value;
    }
    bool operator<(const V_VALUE_TP& v_value) const {
        return _v_value < v_value._v_value;
    }
    */
    operator float() const { return _v_value; }
    friend ostream &operator<<(ostream &out, const V_VALUE_TP& v) {
        out << v._v_value;
        return out;
    }
};

// typedef float V_VALUE_TP;
typedef int V_ID_TP;

const int OMEGA_DEPTH = 9;
const int OMEGA_SWITCH_NUM = 256;
const int CORE_NUM = 96;
const int FIFO_SIZE = 16;
const int MAX_TASK_NUM = FIFO_SIZE * 2 / 2;
const int OM_FIFO_SIZE = 4;

const int MAX_VERTEX_NUM = 1024 * 1024;
const int MAX_EDGE_NUM = 100000000;
const int CACHELINE_LEN = 16;
const int POWER_LAW_V_NUM = 32;

const int BitMap_Compressed_Length = 32;
const int BitMap_Compressed_NUM = MAX_VERTEX_NUM / (CORE_NUM * BitMap_Compressed_Length) + 1;

const int PSEUDO_CHANNEL_NUM = 6;
const int GROUP_CORE_NUM = CORE_NUM / PSEUDO_CHANNEL_NUM;
const int MAX_EDGE_ADDR = MAX_VERTEX_NUM * 32 / PSEUDO_CHANNEL_NUM;

const int HPX_VERTICAL_NUM = PSEUDO_CHANNEL_NUM; // row
const int HPX_HORIZONTAL_NUM = CACHELINE_LEN; // column

const int FLOAT_ADD_DELAY = 12;
const V_VALUE_TP DAMPING = 0.85;
const V_VALUE_TP ONE_OVER_N = 1.0 / MAX_VERTEX_NUM;
const V_VALUE_TP E1 = 1e-7;
const V_VALUE_TP E2 = 1e-2;
const V_VALUE_TP ADDED_CONST = (V_VALUE_TP(1.0) - DAMPING) * ONE_OVER_N;

#endif // PARAMETER_H