/*======================================================
    > File Name: tcpserver.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月21日 星期一 16时58分31秒
 =======================================================*/

#include "tcpserver.h"
#include "log.h"
#include <functional>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

using namespace lnet;

template<class ServantManager>
TcpServer<ServantManager>::TcpServer(const int port,const char* ip)
    :_serverSocket(port,ip),
    _acceptHandler(new IOEventHandler(
                _serverSocket.getSockfd(),
                EV_READ)),
    _wokerThreadGroup(0),
    _wokerThreadNum(0)
{
    //创建日志模块
    lnet::Singleton<lnet::Log>::getInstance()->init(lnet::DEBUG);
    _acceptHandler->setReadCallback(
            std::bind(&TcpServer::acceptCallback,this));
    _masterLoop.addIOHandler(_acceptHandler);

    LOG_INFO("TcpServer create success");
}
//析构函数
template<class ServantManager>
TcpServer<ServantManager>::~TcpServer()
{}

//启动server
template<class ServantManager>
void TcpServer<ServantManager>::start(int num)
{
    this->_wokerThreadNum = num;
    this->_wokerThreadGroup.resize(_wokerThreadNum);
    //启动工作线程
    for(int i=0;i<_wokerThreadNum;i++)
    {
        lnet::WokerPtr  worker(new lnet::Woker());
        _wokerThreadGroup[i] = worker;
        _wokerThreadGroup[i]->_pipe1Handler->setReadCallback(
                std::bind(&TcpServer::handleNewConnection,this,i));
        _wokerThreadGroup[i]->start();
    }
    //启动master事件循环
    _serverSocket.listen();
    LOG_INFO("TcpServer start running");
    _masterLoop.waitEvent();
}

template<class ServantManager>
void TcpServer<ServantManager>::acceptCallback()
{
    lnet::Socket  connSocket = _serverSocket.accept();
    if(connSocket.getSockfd() != -1)
    {
        int i = connSocket.getSockfd() % _wokerThreadNum;
        int ret = write(_wokerThreadGroup[i]->_pipefd[0],
                (const void*)&connSocket,
                sizeof(connSocket));
        LOG_DEBUG("TcpServer accept new connection ret=" 
                + std::to_string(ret));
        LOG_DEBUG(connSocket.getClientAddr().getIpBySockAddr()
                + ":"
                + std::to_string(
                    connSocket.getClientAddr().getPortBySockAddr()));
        if(ret == -1)
            LOG_ERROR("new connection tell Woker failed!");
    }
}

/*
void TcpServer::setReadMsgCallback(std::function<void ()> cb)
{
    this->_handleReadMsg = cb;
}
*/
