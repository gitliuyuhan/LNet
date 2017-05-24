/*======================================================
    > File Name: event_loop.h
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月18日 星期五 21时25分06秒
 =======================================================*/

#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <iostream>
#include <map>
#include <functional>
#include <vector>
#include <atomic>
#include <memory>

#include <time.h>
#include <unistd.h>
#include <sys/epoll.h>

#include "net_base.h"
#include "log.h"

#define EV_READ    0x01
#define EV_WRITE   0x02
#define EV_ERR     0x04

//#define EV_SIGNAL  0x04
//#define EV_TIMEOUT 0x08

namespace lnet
{
/*
 * 事件抽象基类
 */
class EventHandler
{
public:
    virtual ~EventHandler(){}
    virtual void handleEvent() = 0;
};
/*
 * IO事件类
 */
class IOEventHandler : public EventHandler
{
public:
    IOEventHandler(int fd,int flag) 
        : _fd(fd),
        _events(flag),
        _active_events(0),
        _readCallback(NULL),
        _writeCallback(NULL),
        _errCallback(NULL)
    {}
    ~IOEventHandler(){}
    //添加需要监听的事件
    void addEvents(int flag);
    //移除正在监听的事件
    void removeEvents(int flag);
    //设置读事件回调
    void setReadCallback(std::function<void ()>  cb);
    //设置写事件回调
    void setWriteCallback(std::function<void ()>  cb);
    //设置错误事件
    void setErrCallback(std::function<void ()> cb);
    //处理激活的事件
    void handleEvent();
    //记录激活的事件
    void registeActiveEvents(int flag);
    //获取fd
    int getFd();

    int ioFlagToEpollFlag();
private:
    int   _fd;
    int   _events;//注册的事件类型
    int   _active_events;//激活的事件类型
    std::function<void ()>   _readCallback;
    std::function<void ()>   _writeCallback;
    std::function<void ()>   _errCallback;
};
/*
 * 信号事件类
 */
class SigEventHandler : public EventHandler
{
public:
    SigEventHandler() : _sig(-1),_signalCallback(NULL)
    {}
    SigEventHandler(int sig,std::function<void ()> cb)
        : _sig(sig),
        _signalCallback(cb)
    {}
    ~SigEventHandler(){}
    //设置信号处理函数
    void setSignalCallback(std::function<void ()> cb);
    //事件处理
    void handleEvent();
private:
    int   _sig;//信号值
    std::function<void ()>   _signalCallback;
};

/*
 * 定时事件类
 */
class TimerEventHandler : public EventHandler
{
public:
    TimerEventHandler()
        : _time({-1,0}),
        _timerCallback(NULL)
    {}
    TimerEventHandler(timeval t,std::function<void ()> cb = NULL)
        : _time(t), 
        _timerCallback(cb)
    {}
    //用秒数构造
    TimerEventHandler(time_t sec,std::function<void ()> cb = NULL)
        : _time({sec,0}),
        _timerCallback(cb)
    {}
    void setTimerCallback(std::function<void ()> cb);
    void handleEvent();
private:
    timeval    _time;//定时时间(秒，微妙)
    std::function<void ()>   _timerCallback;
};

/*
 * Epoller 封装epoll
 */
class Epoller
{
public:
    Epoller();
    ~Epoller()
    {
        LOG_DEBUG("close Epoller");
        ::close(_epollfd);
    }
    //添加epoll事件
    void addEpollEvents(int fd,int flag);
    //修改
    void modEpollEvents(int fd,int flag);
    //删除fd
    void delEpollEvents(int fd);
    //监听事件
    int epollWait(int timeout = -1);
    void useET(bool yes);

    std::vector<struct epoll_event>     _events;//就绪事件表
private:
    int    _max_event_num; //监听的最大事件数，等于_events大小
    int    _epollfd;
    bool   _isET; //默认开启ET模式
};

/*
 * Event Loop 事件循环
 */
typedef std::shared_ptr<IOEventHandler>             IOEventHandlerPtr;
typedef std::map<int,IOEventHandlerPtr>::iterator   IOEventHandlerIt;

class EventLoop
{
public:
    EventLoop(){}
    ~EventLoop(){}
    //添加IO事件
    void addIOEvent(int fd,int flag);
    //移除IO事件
    void removeIOEvent(int fd,int flag);
    void removeIOHandler(int fd);
    void addIOHandler(IOEventHandlerPtr ptr);
    //事件监听
    void waitEvent();
    //停止Loop
    void stopLoop();
private:
    std::map<int,IOEventHandlerPtr>   _ioHandlerList;
    std::atomic_bool                  _running;
    Epoller                           _epoller;
};

}

#endif
