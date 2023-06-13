#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include<cstdio>
#include<iostream>
#include <cstring>
#include <vector>
#include <queue>
#include <cstdarg>
#include <map>
#include <assert.h>
#include <sstream>
#include <initializer_list>
#include <functional>

using namespace std;

static const int K = 100;

class Info {
// 所有的接口必须继Info，Info表示接口具体会传输的信息
public:
// 要求用户实现copy函数
    virtual void copy(Info *info) = 0;
    string name;
    Info(string str){name = str;}
};


// Pipe为两个模块供交互通道
class Pipe {
public:
    Info * _info_rd;
    Info * _info_wr;
    Pipe(){
        _info_rd = nullptr;
        _info_wr = nullptr;
    }
    virtual void bind(Info *info_wr, Info *info_rd) {
        if(info_rd != nullptr)
            _info_rd = info_rd;
        if(info_wr != nullptr)
            _info_wr = info_wr;
    }
    virtual void read(Info *info){
        _info_rd->copy(info);
    }
    virtual void write(Info *info) {
        info->copy(_info_wr);
    }
    virtual void exec() {
        if( _info_wr != nullptr && _info_rd != nullptr )
            _info_wr->copy( _info_rd);
    }
};

class pipe_manager{
public:
    Pipe pipe[K];
    map<string,int> mp;
    int k = 0;
    virtual void bind(Info *info_wr, Info *info_rd){
        assert(info_wr != nullptr && info_rd != nullptr);
        pipe[k].bind(info_wr,info_rd);
        mp[(*info_wr).name] = k;
        k++;
    }

    void bind_wr(Info *info_wr,string name){
        assert(info_wr != nullptr);
        pipe[k].bind(info_wr,nullptr);
        mp[name] = k;
        if(pipe[k]._info_rd != nullptr){
            k++;
        }
    }

    void bind_rd(Info *info_rd,string name){
        assert(info_rd != nullptr);
        pipe[k].bind(nullptr,info_rd);
        mp[name] = k;
        if(pipe[k]._info_wr != nullptr){
            k++;
        }
    }

    virtual void read(Info *info){
        assert(mp.find((*info).name) != mp.end());
        int tmp_k = mp[(*info).name];
        // cout << "tmp_k:" << tmp_k <<endl;
        pipe[tmp_k].read(info);
    }

    virtual void write(Info *info){
        if (mp.find((*info).name) == mp.end()) {
            cout << "[ERR] Can't find " << (*info).name << " in pipe mg" << endl;
        }
        assert(mp.find((*info).name) != mp.end());
        int tmp_k = mp[(*info).name];
        // cout << "tmp_k:" << tmp_k <<endl;
        pipe[tmp_k].write(info);
    }

    void run(){
        for(int i = 0 ; i<K ; i++){
            pipe[i].exec();
        }
    }

};

// 最多允许用户定义 K 个接口
// 1. 用户定义自己的接口
#ifdef main_part 
pipe_manager pipe_mg;
#define config(name,var)     \
class Info_##name : public Info { \
public: \
    var \
    virtual void copy(Info *info) { \
        *this = *(dynamic_cast<Info_##name*>(info)); \
    } \
    Info_##name(pipe_manager* pipe_mgr=nullptr,int type=2):Info(#name){ \
        if(type == 0){  \
            pipe_mgr->bind_rd(this,#name);  \
        }else if(type == 1){   \
            pipe_mgr->bind_wr(this,#name);   \
        }  \
    }  \
 }; \
class Info_##name  info_##name##_rd(&pipe_mg,0),info_##name##_wr(&pipe_mg,1); 
#else
#define config(name,var)     \
class Info_##name : public Info { \
public: \
    var \
    virtual void copy(Info *info) { \
        *this = *(dynamic_cast<Info_##name*>(info)); \
    } \
    Info_##name(pipe_manager* pipe_mgr=nullptr,int type=2):Info(#name){ \
        if(type == 0){  \
            pipe_mgr->bind_rd(this,#name);  \
        }else if(type == 1){   \
            pipe_mgr->bind_wr(this,#name);   \
        }  \
    }  \
 }; 
#endif

#endif // FRAMEWORK_H