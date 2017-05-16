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
#include <mutex>
#include <memory>
#include "event_loop.h"
#include "net_socket.h"
#include "buffer.h"
//#include "servant.h"
//#include "woker.h"

namespace lnet
{
class Servant;
class Woker;

typedef std::shared_ptr<Woker>        WokerPtr;
typedef std::shared_ptr<Servant>      ServantPtr; 
/*
 * TcpConnection 用户连接类
 */
//业务类声明
class TcpConnection
{
public:
    TcpConnection(Socket conn,EventLoop* loop,Woker* worker);
    ~TcpConnection();
    void setServant(lnet::ServantPtr servant);
    //销毁连接
    void destroy();
    //设置读消息处理回调
    //void setReadMsgCallback(std::function<void ()> cb);
    //发送数据
    void sendData(const char* data,const int len);
    //发送来自外部的消息
    void sendDataFromOther(const char* data,const int len);
private:
    //设置读事件
    void handleReadEvent();
    //处理写事件
    void handleWriteEvent();
    /*
     * 发送
     * @send_buf:暂存未发送成功的数据
     * return:成功返回实际发生大小，失败返回-1
     */
    int send(Buffer &send_buf,const char* data,const int len);
private:
    Socket                  _connSocket;
    IOEventHandlerPtr       _connHandler;
    EventLoop*              _loop;//所属的EventLoop;
    Woker*                  _worker;//所属worker
    //读消息buffer
    Buffer                  _recv_buf;
    //写消息buffer
    Buffer                  _send_buf;
    //来自外部的写消息
    Buffer                  _send_buf_from_other;
    //业务类
    ServantPtr              _servant;
    std::mutex              _mutex;
};
typedef std::shared_ptr<TcpConnection>    TcpConnectionPtr;

}
#endif
