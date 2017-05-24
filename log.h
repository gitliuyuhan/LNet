/*======================================================
    > File Name: log.h
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2017年05月23日 星期二 01时35分53秒
 =======================================================*/

#ifndef LOG_H
#define LOG_H

#include<iostream>
#include<string>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

#include"singleton.h"

#define LOG_DEBUG(msg) (lnet::Singleton<lnet::Log>::getInstance()->append(lnet::DEBUG,msg) )
#define LOG_INFO(msg) (lnet::Singleton<lnet::Log>::getInstance()->append(lnet::INFO,msg) )
#define LOG_WARN(msg) (lnet::Singleton<lnet::Log>::getInstance()->append(lnet::WARN,msg) )
#define LOG_ERROR(msg) (lnet::Singleton<lnet::Log>::getInstance()->append(lnet::ERROR,msg) )

namespace lnet
{
//日志级别
enum LogLevel
{
    DEBUG,
    INFO,
    WARN,
    ERROR,
};

class Log
{

public:
    Log();
    ~Log()
    {
        if(_fd != -1)
        {
            ::close(_fd);
        }           
    }
    void init(const int level = 0,const std::string file = "");
    void append(const int level,const std::string msg);
private:
    //日志文件
    int _fd;
    //日志级别
    int _level;
    const char* _level_str[4] = {
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR"};
};
}
#endif

