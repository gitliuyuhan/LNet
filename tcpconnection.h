/*======================================================
    > File Name: tcpconnection.h
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月29日 星期二 15时44分21秒
 =======================================================*/
#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <iostream>
#include "event_loop.h"
#include "net_socket.cpp"

namespace lnet
{
/*
 * TcpConnection 用户连接类
 */
class TcpConnection
{
public:
    TcpConnection(Socket conn,EventLoop* loop);
    ~TcpConnection();
    //设置读消息处理回调
    void setReadMsgCallback(std::function<void ()> cb);
private:
    //设置读事件
    void handleReadEvent();
    //处理写事件
    void handleWriteEvent();
private:
    Socket                  _connSocket;
    IOEventHandlerPtr       _connHandler;
    EventLoop*              _loop;//所属的EventLoop;
    std::function<void ()>  _handleReadMsg;
};

}
#endif
