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

const int CORE_NUM = 32;
const int FIFO_SIZE = 16;

const int MAX_VERTEX_NUM = 1024 * 1024;
const int MAX_EDGE_NUM = 100000000;
const int CACHELINE_LEN = 16;

const int BitMap_Compressed_Length = 32;
// const int BitMap_Compressed_NUM = MAX_VERTEX_NUM / (CORE_NUM * BitMap_Compressed_Length) + 1;
const int BitMap_Compressed_NUM = 4039 / (CORE_NUM * BitMap_Compressed_Length) + 1;

const int PSEUDO_CHANNEL_NUM = 2;
const int GROUP_CORE_NUM = CORE_NUM / PSEUDO_CHANNEL_NUM;
const int MAX_EDGE_ADDR = MAX_VERTEX_NUM * 32 / PSEUDO_CHANNEL_NUM;

const int HPX_ROW_NUM = PSEUDO_CHANNEL_NUM; // row
const int HPX_COLUMN_NUM = GROUP_CORE_NUM; // column

const int BACKEND_DEST_FIFO_NUM = 4;

const int FLOAT_ADD_DELAY = 14;
const V_VALUE_TP DAMPING = 0.85;
const V_VALUE_TP E1 = 1e-7;
const V_VALUE_TP E2 = 1e-2;

const int WAIT_END_DELAY = 20;

#endif // PARAMETER_H