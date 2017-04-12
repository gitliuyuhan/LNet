/*======================================================
    > File Name: tcpconnection.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年12月16日 星期五 20时15分06秒
 =======================================================*/

#include "tcpconnection.h"

using namespace lnet;

TcpConnection::TcpConnection(Socket conn,EventLoop* loop)
    :_handleReadMsg(NULL)
{
    _connSocket = conn;
    _loop = loop;
    _connHandler.reset(new IOEventHandler(
                _connSocket.getSockfd(),
                EV_READ));
    _connHandler->setReadCallback(
            std::bind(&TcpConnection::handleReadEvent,this));
    _loop->addIOHandler(_connHandler);
}

void TcpConnection::handleReadEvent()
{}

void TcpConnection::handleWriteEvent()
{}

void TcpConnection::setReadMsgCallback(std::function<void ()> cb)
{
    this->_handleReadMsg = cb;
}
