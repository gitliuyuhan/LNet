/*======================================================
    > File Name: woker.h
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月21日 星期一 18时29分27秒
 =======================================================*/

#ifndef WORKER_H
#define WORKER_H

#include <iostream>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "event_loop.h"
#include "tcpconnection.h"
#include "buffer.h"

namespace lnet
{
class TcpConnection;
/*
 * Woker线程
 */
class Woker
{
public:
    Woker();
    ~Woker();
    //线程函数
    void threadFunc();
    void start();
    EventLoop* getWokerLoop();

    //添加用户连接
    void addTcpConnection(int fd,lnet::TcpConnectionPtr conn);
    //删除用户连接
    void delTcpConnection(int fd);

    int                 _pipefd[2];//用于和master通信
    IOEventHandlerPtr   _pipe1Handler;
    lnet::Buffer        _pipe1_recv_buf;

    //处理pipe1中的新连接
    template<typename ServantManager>
    void doPipe1Msg()
    {
        //接收pipe1消息
        this->recvPipe1Msg();
        //处理新连接
        const int len = sizeof(lnet::Socket);
        int read_len = _pipe1_recv_buf.readableSize();
        while(read_len >= len)
        {
            lnet::Socket   sock;
            _pipe1_recv_buf.readBuffer<Socket>(&sock,len);
            lnet::TcpConnectionPtr  conn(new lnet::TcpConnection(sock,
                        _wokerLoop,
                        this));
            std::shared_ptr<ServantManager>  servant(
                    new ServantManager());
            conn->setServant(servant);
            servant->setTcpConn(conn);
            this->addTcpConnection(sock.getSockfd(),conn);
            read_len = _pipe1_recv_buf.readableSize();
        }
    }
private:
    //读取pipe1中的数据
    void recvPipe1Msg();
private:
    std::unique_ptr<std::thread>     _thread;
    //loop指向的是栈变量，自动释放
    EventLoop*                       _wokerLoop;
    std::mutex                       _mutex;
    std::condition_variable          _cond;
    //保存Worker下的所有用户连接
    std::map<int,lnet::TcpConnectionPtr>  _connList;
};
typedef std::shared_ptr<lnet::Woker>   WokerPtr;

}

#endif
