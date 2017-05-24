/*======================================================
    > File Name: log.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2017年05月23日 星期二 02时47分48秒
 =======================================================*/

#include "log.h"
#include "net_base.h"

using namespace lnet;

Log::Log():_level(0),_fd(-1)
{}

void Log::init(const int level,const std::string file)
{
    this->_level = level;
    if(file == "")
    {
        _fd = -1;
        return;
    }
    this->_fd = open(file.c_str(),O_CREAT|O_RDWR|O_APPEND,0664);
    if(_fd == -1)
        perror("open log file failed!");
}

void Log::append(const int level,const std::string msg)
{
    if(level < _level)
        return;
    char thread_id[20];
    snprintf(thread_id,sizeof(thread_id),
            "%lu",pthread_self());
    std::string log_str = lnet::nowTimeToString() + "|" 
        + thread_id + "|" 
        + _level_str[level] + "|" 
        + msg + "\n";
    if(_fd != -1)
    {
        int ret = ::write(_fd,log_str.c_str(),log_str.length());
        if(ret == -1)
            perror("write log file failed!");
    }
    else
    {
        //没有日志文件时，输出到屏幕
        std::cout << log_str;
    }
}

