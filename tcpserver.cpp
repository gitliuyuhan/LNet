/*======================================================
    > File Name: tcpserver.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月21日 星期一 16时58分31秒
 =======================================================*/

#include "tcpserver.h"
#include <functional>

using namespace lnet;

TcpServer::TcpServer(const int port,const char* ip)
    :_serverSocket(port,ip),
    _acceptHandler(new IOEventHandler(
                _serverSocket.getSockfd(),
                EV_READ)),
    _wokerThreadGroup(0),
    _wokerThreadNum(0)
{
    _acceptHandler->setReadCallback(
            std::bind(&TcpServer::acceptCallback,this));
    _masterLoop.addIOHandler(_acceptHandler);
}
//析构函数
TcpServer::~TcpServer()
{}

//启动server
void TcpServer::start(int num)
{
    this->_wokerThreadNum = num;
    this->_wokerThreadGroup.resize(_wokerThreadNum);
    //启动工作线程
    for(int i=0;i<_wokerThreadNum;i++)
    {
        _wokerThreadGroup[i]->_pipe1Handler->setReadCallback(
                std::bind(&TcpServer::handleNewConnection(),this));
        _wokerThreadGroup[i]->start();
    }
}

void TcpServer::acceptCallback()
{
    lnet::Socket  connSocket = _serverSocket.accept();
    if(connSocket.getSockfd() != -1)
    {
        std::cout << "new connection" << std::endl;
    }
}

void TcpServer::handleNewConnection()
{
    const int len = sizeof(lnet::Socket);
    char buf[len];
    int ret = read(_wokerThreadGroup);
}

void TcpServer::setReadMsgCallback(std::function<void ()> cb)
{
    this->_handleReadMsg = cb;
}
