/*======================================================
    > File Name: woker.h
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月21日 星期一 18时29分27秒
 =======================================================*/

#ifndef WORKER_H
#define WORKER_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "event_loop.h"

namespace lnet
{
/*
 * Woker线程
 */
class Woker
{
public:
    Woker();
    ~Woker();
    //线程函数
    void threadFunc();
    void start();
    EventLoop* getWokerLoop();

    int                 _pipefd[2];//用于和master通信
    IOEventHandlerPtr   _pipe1Handler;
private:
    std::unique_ptr<std::thread>     _thread;
    EventLoop*                       _wokerLoop;
    std::mutex                       _mutex;
    std::condition_variable          _cond;
};
typedef std::shared_ptr<lnet::Woker>   WokerPtr;

}

#endif
