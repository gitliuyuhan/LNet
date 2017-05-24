/*======================================================
    > File Name: buffer.h
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2017年05月08日 星期一 12时08分03秒
 =======================================================*/

#ifndef BUFFER_H
#define BUFFER_H

#include<iostream>
#include<vector>

namespace lnet
{
class Buffer
{
public:
    //初始化1M
    Buffer(int size=1024*1024)
    {
        _buffer.resize(size);
        _write_index = 0;
        _read_index = 0;
    }
    ~Buffer(){}

    //可写字节数
    int writeableSize();
    //可读字节数
    int readableSize();
    //输出buffer数据
    void printBuffer();
    //重置buffer
    void resetBuffer();
    //往buffer添加数据,空间不够会失败
    void append(const char* data,const int len);
    //写数据,空间不够会扩展
    void writeBuffer(const char *data,const int len);
    //读数据
    template<typename T>
    void readBuffer(T *p,const int len)
    {
        int read_len = copySomeData(p,len);
        _read_index = _read_index + read_len;
    }
    //向外拷贝数据
    template<typename T>
    int copySomeData(T *p,const int len)
    {
        int read_len = this->readableSize();
        if(read_len > len)
        {
            read_len = len;
        }
        std::copy(getReadPtr(),getReadPtr()+read_len,(char*)p);
        return read_len;
    }
    //获取读指针
    char* getReadPtr();
    //移动读指针位置
    void moveReadPtr(const int d);
    //获取头指针
    char* begin();
private:
    //将未读数据移动到起始位置
    void moveNoReadToHead();
    //获取读位置
    int getReadIndex();
private:
    std::vector<char>       _buffer;
    //写数据位置
    int                     _write_index;
    //读数据位置
    int                     _read_index;
};

}

#endif

