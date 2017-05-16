/*======================================================
    > File Name: buffer.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2017年05月13日 星期六 16时10分59秒
 =======================================================*/

#include "buffer.h"

using namespace lnet;

int Buffer::writeableSize()
{
    return _buffer.size() - _write_index;
}

int Buffer::readableSize()
{
    return _write_index - _read_index;
}

void Buffer::printBuffer()
{
    for(auto i = _buffer.begin();
            i != _buffer.begin() + _write_index;
            i++)
        std::cout << *i;
}

void Buffer::resetBuffer()
{
    _write_index = 0;
    _read_index = 0;
}

void Buffer::append(const char *data,const int len)
{
    std::copy(data,data+len,_buffer.begin() + _write_index);
    _write_index += len;
}

void Buffer::writeBuffer(const char *data,const int len)
{
    int write_len = this->writeableSize();
    if(_read_index > 0 && write_len < len)
    {
        moveNoReadToHead();
    }
    write_len = this->writeableSize();
    //空间不够，扩大
    if(write_len < len)
    {
        _buffer.resize(_buffer.size() + (len - write_len));
    }
    this->append(data,len);
}

void Buffer::moveNoReadToHead()
{
    std::copy(getReadPtr(),
            getReadPtr()+readableSize(),
            _buffer.begin());
    _write_index = 0 + readableSize();
    _read_index = 0;
}

int Buffer::getReadIndex()
{
    return _read_index;
}

char* Buffer::begin()
{
    return &(*_buffer.begin());
}

char* Buffer::getReadPtr()
{
    return this->begin() + _read_index;
}

void Buffer::moveReadPtr(const int d)
{
    this->_read_index + d;
}
