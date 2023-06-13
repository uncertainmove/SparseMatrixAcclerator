#ifndef UTIL_H
#define UTIL_H

#include <bits/stdc++.h>

using namespace std;

template<typename T>
void clear_buffer(queue<T> &q) {
    while (!q.empty()) {
        q.pop();
    }
}

template<typename T>
void sg_view(T sg[], std::string name, int num) {
    cout << setw(15) << name << ":";
    for (int i = 0; i < num; i++) {
        cout << setw(7) << " " << sg[i];
    }
    cout << endl;
}

#endif // UTIL_H