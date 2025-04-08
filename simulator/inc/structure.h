#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "parameter.h"
#include <bits/stdc++.h>

typedef struct Cacheline_16 {
    int data[16];
    Cacheline_16() {}
    Cacheline_16(int f) {
        for (int i = 0; i < 16; i++) {
            data[i] = f;
        }
    }
    bool compare(const Cacheline_16 &b) {
        bool res = true;
        for (int i = 0; i < 16; i++) {
            if (data[i] != b.data[i]) res = false;
        }
        return res;
    }
    void show() {
        printf("mask:");
        for (int i = 0; i < 16; i++) {
            printf(" %d", data[i]);
        }
        printf("\n");
    }
} Cacheline_16;

typedef struct bitmap {
    bool v[BitMap_Compressed_Length];
    bitmap() {}
    bitmap(bool f) {
        for (int i = 0; i < BitMap_Compressed_Length; i++) {
            v[i] = f;
        }
    }
    bitmap(const bitmap& b) {
        for (int i = 0; i < BitMap_Compressed_Length; i++) {
            v[i] = b.v[i];
        }
    }
    bool compare(const bitmap& b) {
        bool res = true;
        for (int i = 0; i < BitMap_Compressed_Length; i++) {
            if (v[i] != b.v[i]) res = false;
        }
        return res;
    }
    inline void set(bool f) {
        for (int i = 0; i < BitMap_Compressed_Length; i++) {
            v[i] = f;
        }
    }
    void show() {
        printf("bitmap:");
        for (int i = 0; i < BitMap_Compressed_Length; i++) {
            printf(" %d", v[i] == true);
        }
        printf("\n");
    }
} bitmap;

typedef struct BitVector_16 {
    bool flag[16];
    BitVector_16() {}
    BitVector_16(bool f) {
        for (int i = 0; i < 16; i++) {
            flag[i] = f;
        }
    }
    bool compare(const BitVector_16 &b) {
        bool res = true;
        for (int i = 0; i < 16; i++) {
            if (flag[i] != b.flag[i]) res = false;
        }
        return res;
    }
    void show() {
        printf("mask:");
        for (int i = 0; i < 16; i++) {
            printf(" %d", flag[i]);
        }
        printf("\n");
    }
} BitVector_16;

#endif // STRUCTURE_H