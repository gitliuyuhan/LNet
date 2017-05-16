/*======================================================
    > File Name: woker.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月21日 星期一 18时52分28秒
 =======================================================*/

#include "woker.h"
#include "servant.h"
#include "net_socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <assert.h>

using namespace lnet;

Woker::Woker():_wokerLoop(NULL),_pipe1_recv_buf(1024)
{
    int ret = ::socketpair(PF_UNIX,SOCK_STREAM,0,_pipefd);
    assert(ret == 0);
    _pipe1Handler.reset(new IOEventHandler(_pipefd[1],EV_READ));
    //_wokerLoop->addIOHandler(_pipe1Handler);
}

Woker::~Woker()
{}

void Woker::start()
{
    //创建线程
    _thread.reset(new std::thread(&Woker::threadFunc,this));
    {
        //std::lock_guard<std::mutex>   locker(_mutex);
        std::unique_lock<std::mutex>    locker(_mutex);
        while(_wokerLoop == NULL)
        {
            _cond.wait(locker);
        }
    }
    //return _wokerLoop;
}

void Woker::threadFunc()
{
    EventLoop    loop;
    {//保证Loop能先启动
        std::lock_guard<std::mutex>   locker(this->_mutex);
        _wokerLoop = &loop;
        _wokerLoop->addIOHandler(_pipe1Handler);
        this->_cond.notify_one();
    }
    loop.waitEvent();
}

EventLoop* Woker::getWokerLoop()
{
    return this->_wokerLoop;
}

void Woker::addTcpConnection(int fd,lnet::TcpConnectionPtr conn)
{
    this->_connList.insert(
            std::pair<int,lnet::TcpConnectionPtr>(fd,conn));
}

void Woker::delTcpConnection(int fd)
{
    std::map<int,lnet::TcpConnectionPtr>::iterator   it;
    it = _connList.find(fd);
    if(it != _connList.end())
    {
        _connList.erase(it);
    }
}

void Woker::recvPipe1Msg()
{
    char buf[1024];
    while(1)
    {
        memset(buf,'\0',1024);
        int ret = recv(_pipe1Handler->getFd(),buf,1024,0);
        if(ret < 0)
        {
            if((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                std::cout << "read pipe1 end" << std::endl;
                break;
            }
            ::close(_pipe1Handler->getFd());
            break;
        }
        else if(ret == 0)
        {
            ::close(_pipe1Handler->getFd());
        }
        else
        {
            _pipe1_recv_buf.writeBuffer(buf,ret);
        }
    }   
}
