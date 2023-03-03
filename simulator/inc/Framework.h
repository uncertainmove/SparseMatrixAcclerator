#define K 100
class Info {
// 所有的接口必须继Info，Info表示接口具体会传输的信息
public:
// 要求用户实现copy函数
    virtual void copy(Info *info) {
        memcpy(this, info, sizeof(*this));
    }
};
// Pipe为两个模块供交互通道
class Pipe {
public:
    Info * _info_rd;
    Info * _info_wr;
    virtual void bind(Info *info_wr, Info *info_rd) {
        _info_rd = info_rd;
        _info_wr = info_wr;
    }
    virtual void read(Info *info){
        _info_rd->copy(info);
    }
    virtual void write(Info *info) {
        info->copy(_info_wr);
    }
    virtual void exec() {
        _info_wr->copy( _info_rd);
    }
} pipe[K];// 最多允许用户定义 K 个接口
// 1. 用户定义自己的接口
// class InfoSelf : public Info {
// public:
//     int _address;
//     virtual void copy(Info *info) {
//         _address = info->_address;
//     }
//  } info_self_rd,info_self_wr;// 2.绑定接口到Pipe[i]