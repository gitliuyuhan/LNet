/*======================================================
    > File Name: tcpserver.h
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月21日 星期一 16时37分37秒
 =======================================================*/

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <iostream>

#include "net_socket.h"
#include "event_loop.h"
#include "woker.h"

namespace lnet
{
/*
 * TCP Server类
 */
template<class ServantManager>
class TcpServer
{
public:
    TcpServer(const int port,const char* ip = NULL);
    ~TcpServer();
    //启动服务器
    void start(int num = 4);
private:
    //有新连接
    void acceptCallback();  
    //处理新连接
    void handleNewConnection(int i)
    {
        _wokerThreadGroup[i]->doPipe1Msg<ServantManager>();
    }
    //void setReadMsgCallback(std::function<void ()> cb);   
private:
    ServerSocket                    _serverSocket;
    IOEventHandlerPtr               _acceptHandler;
    EventLoop                       _masterLoop;//主线程事件循环
    std::vector<lnet::WokerPtr>     _wokerThreadGroup;//工作线程组
    int                             _wokerThreadNum;//工作线程数
    //std::function<void ()>          _handleReadMsg;//处理读消息
};

}
#endif
