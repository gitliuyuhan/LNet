/*======================================================
    > File Name: event_loop.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月18日 星期五 21时56分55秒
 =======================================================*/

#include "event_loop.h"

using namespace lnet;

/*
 * IOEventHandler
 */
void IOEventHandler::addEvents(int flag)
{
    _events |= flag;
}
void IOEventHandler::removeEvents(int flag)
{
    _events &= (~flag);
}
void IOEventHandler::registeActiveEvents(int flag)
{
    _active_events |= flag;
}
void IOEventHandler::setReadCallback(std::function<void ()> cb)
{
    _readCallback = cb;
}
void IOEventHandler::setWriteCallback(std::function<void ()> cb)
{
    _writeCallback = cb;
}
void IOEventHandler::setErrCallback(std::function<void ()> cb)
{
    _errCallback = cb;
}
void IOEventHandler::handleEvent()
{
    if(_active_events & EV_READ)
    {
        if(_readCallback)
            _readCallback();
    }
    if(_active_events & EV_WRITE)
    {
        if(_writeCallback)
            _writeCallback();
    }
    if(_active_events & EV_ERR)
    {
        if(_errCallback)
            _errCallback();
    }
    _active_events = 0x0;
}
int IOEventHandler::ioFlagToEpollFlag()
{
    int flag = 0x0;
    if(_events & EV_READ)
        flag |= EPOLLIN;
    if(_events & EV_WRITE)
        flag |= EPOLLOUT;
    return flag;
}
int IOEventHandler::getFd()
{
    return this->_fd;
}

/*
 * 信号事件
 */
void SigEventHandler::setSignalCallback(std::function<void ()> cb)
{
    _signalCallback = cb;
}
void SigEventHandler::handleEvent()
{
    if(_signalCallback)
        _signalCallback();
}

/*
 * 定时事件
 */
void TimerEventHandler::setTimerCallback(std::function<void ()> cb)
{
    _timerCallback = cb;
}
void TimerEventHandler::handleEvent()
{
    if(_timerCallback)
        _timerCallback();
}

/*
 * Epoller
 */
Epoller::Epoller()
     : _max_event_num(1024),
     _events(1024),
     _isET(true)
{
    _epollfd = epoll_create(10);
}

void Epoller::useET(bool yes)
{
    _isET = yes;
}
int Epoller::epollWait(int timeout)
{
    int number = epoll_wait(
            _epollfd,
            &*_events.begin(),
            _max_event_num,
            timeout);
    return number;
}
void Epoller::addEpollEvents(int fd,int flag)
{
    struct epoll_event   event;
    event.data.fd = fd;
    event.events = flag;
    if(_isET)
        flag |= EPOLLET;
    //就绪事件表扩容
    if(fd > _max_event_num)
    {
        _max_event_num *= 2;
        _events.resize(_max_event_num);
    }
    epoll_ctl(_epollfd,EPOLL_CTL_ADD,fd,&event);
    lnet::setNonBlockFd(fd);
}
void Epoller::modEpollEvents(int fd,int flag)
{
    struct epoll_event   event;
    event.data.fd = fd;
    event.events = flag;
    epoll_ctl(_epollfd,EPOLL_CTL_MOD,fd,&event);
}
void Epoller::delEpollEvents(int fd)
{
    epoll_ctl(_epollfd,EPOLL_CTL_DEL,fd,0);
}

/* 
 * EventLoop事件循环
 */
void EventLoop::addIOHandler(IOEventHandlerPtr ptr)
{
    _ioHandlerList.insert(std::make_pair(ptr->getFd(),ptr));
    _epoller.addEpollEvents(ptr->getFd(),ptr->ioFlagToEpollFlag());
}
void EventLoop::removeIOHandler(int fd)
{
    _ioHandlerList.erase(fd);
    _epoller.delEpollEvents(fd);
}
void EventLoop::addIOEvent(int fd,int flag)
{
    std::map<int,IOEventHandlerPtr>::iterator  it;
    it = _ioHandlerList.find(fd);
    if(it != _ioHandlerList.end())
    {
        IOEventHandlerPtr  ptr = it->second;
        ptr->addEvents(flag);
        _epoller.modEpollEvents(ptr->getFd(),ptr->ioFlagToEpollFlag());
    }
}
void EventLoop::removeIOEvent(int fd,int flag)
{
    std::map<int,IOEventHandlerPtr>::iterator  it;
    it = _ioHandlerList.find(fd);
    if(it != _ioHandlerList.end())
    {
        IOEventHandlerPtr   ptr = it->second;
        ptr->removeEvents(flag);
        _epoller.modEpollEvents(ptr->getFd(),ptr->ioFlagToEpollFlag());
    }
}
void EventLoop::waitEvent()
{
    _running = true;
    int timeout = -1;
    while(_running)
    {
        int number = _epoller.epollWait(timeout);
        for(int i=0;i<number;i++)
        {
            int flag = 0x0;
            unsigned int what = _epoller._events[i].events;
            if(what & (EPOLLERR | EPOLLHUP) )
            {
                flag |= EV_ERR;
            }
            if(what & (EPOLLIN))
            {
                flag |= EV_READ;
            }
            if(what & (EPOLLOUT))
            {
                flag |= EV_WRITE;
            }
            IOEventHandlerIt  it = _ioHandlerList.find(
                    _epoller._events[i].data.fd);
            if(it != _ioHandlerList.end())
            {
                IOEventHandlerPtr ptr = it->second;
                ptr->registeActiveEvents(flag);//记录激活事件
                ptr->handleEvent();//事件处理
            }
        }
    }
}
void EventLoop::stopLoop()
{
    _running = false;
}
