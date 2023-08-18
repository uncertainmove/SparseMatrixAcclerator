#ifndef ACC_H

#include <bits/stdc++.h>

const int MAX_CLK = 100000000;

const int MAX_VERTEX_NUM = 1024 * 1024 * 2;
const int MAX_EDGE_NUM = 1024 * 1024 * 32;

const int CACHELINE_LEN = 16;

const int CORE_NUM = 64;

const int PSEUDO_CHANNEL_NUM = 4;
const int GROUP_CORE_NUM = CORE_NUM / PSEUDO_CHANNEL_NUM;

const int DELAY_CYCLE = 15;
const int ACC_REG_NUM = 8;

const int FIFO_SIZE = 16;

#define DEBUG_MSG(t, msg) do {  \
    if (t) {                    \
        cout << msg << endl;    \
        exit(-1);               \
    }                           \
} while (0)

#endif // ACC_H
