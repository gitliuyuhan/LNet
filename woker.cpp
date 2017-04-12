/*======================================================
    > File Name: woker.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月21日 星期一 18时52分28秒
 =======================================================*/

#include "woker.h"

#include <sys/types.h>
#include <sys/socket.h>

using namespace lnet;

Woker::Woker():_wokerLoop(NULL)
{
    int ret = ::socketpair(PF_UNIX,SOCK_STREAM,0,_pipefd);
    assert(ret == 0);
    _pipe1Handler.reset(new IOEventHandler(_pipefd[1],EV_READ));
    _wokerLoop.addIOHandler(_pipe1Handler);
}

Woker::~Woker()
{}

EventLoop* Woker::start()
{
    //创建线程
    _thread.reset(new std::thread(&Woker::threadFunc,this));
    {
        std::lock_guard<std::mutex>   locker(_mutex);
        while(_wokerLoop == NULL)
        {
            _cond.wait();
        }
    }
    return _wokerLoop;
}

void Woker::threadFunc()
{
    EventLoop    loop;
    {//保证Loop能先启动
        std::lock_guard<std::mutex>   locker(this->_mutex);
        _wokerLoop = &loop;
        this->_cond.notify_one();
    }
    loop.waitEvent();
}

EventLoop* Woker::getWokerLoop()
{
    return this->_wokerLoop;
}
