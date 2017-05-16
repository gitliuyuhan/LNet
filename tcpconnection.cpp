/*======================================================
    > File Name: tcpconnection.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年12月16日 星期五 20时15分06秒
 =======================================================*/

#include "tcpconnection.h"
#include "woker.h"
#include "servant.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

using namespace lnet;

TcpConnection::TcpConnection(Socket conn,
        EventLoop* loop,
        Woker* worker)
    :_recv_buf(65535),_send_buf(65535),_send_buf_from_other(65535)
{
    _connSocket = conn;
    _loop = loop;
    _worker = worker;
    _connHandler.reset(new IOEventHandler(
                _connSocket.getSockfd(),
                EV_READ));
    _connHandler->setReadCallback(
            std::bind(&TcpConnection::handleReadEvent,this));
    //设置写回调，但是不注册事件
    _connHandler->setWriteCallback(
            std::bind(&TcpConnection::handleWriteEvent,this));
    _loop->addIOHandler(_connHandler);
}

TcpConnection::~TcpConnection()
{}

void TcpConnection::setServant(lnet::ServantPtr servant)
{
    this->_servant = servant;
}

void TcpConnection::destroy()
{
    _connSocket.close();
    _worker->delTcpConnection(_connSocket.getSockfd());
}

void TcpConnection::handleReadEvent()
{
    #define BUF_SIZE  65535
    char  buf[BUF_SIZE];
    while(1)
    {
        memset(buf,'\0',BUF_SIZE);
        int ret = recv(_connSocket.getSockfd(),buf,BUF_SIZE,0);
        if(ret < 0)
        {
            if((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                std::cout << "read end" << std::endl;
                break;
            }
            //关闭sock
            destroy();
            break;
        }
        else if(ret == 0)
        {
            //关闭sock
            destroy();
        }
        else
        {
            _recv_buf.writeBuffer(buf,ret);
        }
    }
    int data_len = 0;
    while(_recv_buf.readableSize() > sizeof(int))
    {
        _recv_buf.copySomeData<int>(&data_len,sizeof(int));
        if(_recv_buf.readableSize() < (data_len + sizeof(int)))
        {
            break;
        }
        //是一条完整请求
        //取出请求头:记录实际数据长度
        _recv_buf.readBuffer<int>(&data_len,sizeof(int));
        const int len = data_len;
        char req[len];
        //读取请求内容
        _recv_buf.readBuffer<char>(req,data_len);
        //处理请求
        this->_servant->doRequest(req);      
    }
}

void TcpConnection::handleWriteEvent()
{
    int len = _send_buf.readableSize();
    if(len > 0)
    {
        _send_buf.resetBuffer();
        //已经设置了写事件，发送失败不需要再设置
        this->send(_send_buf,_send_buf.begin(),len);
    }
    std::lock_guard<std::mutex>  locker(_mutex);
    len = _send_buf_from_other.readableSize();
    if(len > 0)
    {
        _send_buf_from_other.resetBuffer();
        this->send(_send_buf_from_other,
                _send_buf_from_other.begin(),
                len);
    }
}
/*
 * if error return -1
 */
int TcpConnection::send(Buffer &send_buf,
        const char* data,
        const int len)
{
    int ret = ::send(_connSocket.getSockfd(),data,len,0);
    int send_len = 0;
    if(ret < 0)
    {
        //socket缓存已满
        if((errno == EAGAIN) || (errno == EWOULDBLOCK))
        {
            send_len = 0;
        }
        else
        {
            destroy();
            return -1;
        }
    }
    else if(ret == 0)
    {
        destroy();
        return -1;
    }
    else
    {
        send_len = ret;
    }
    //全部发送成功，撤销写事件
    if(send_len == len)
    {
        _connHandler->removeEvents(EV_WRITE);
    }
    //将未发送成功的数据暂存入buffer
    else
    {
        send_buf.writeBuffer(data+send_len,len-send_len);
    }
    return send_len;
}

void TcpConnection::sendData(const char *data,const int len)
{
    //如果buffer有数据，追加
    if(_send_buf.readableSize() > 0)
    {
        _send_buf.writeBuffer(data,len);
        return;
    }
    int ret = this->send(this->_send_buf,data,len);
    //未发送完整，设置写事件
    if(ret > 0 && ret < len)
    {
        _connHandler->addEvents(EV_WRITE);
    }
}

void TcpConnection::sendDataFromOther(const char* data,const int len)
{
    //可能会在不同线程中调用，需要加锁保护
    std::lock_guard<std::mutex>   locker(_mutex);
    _send_buf_from_other.writeBuffer(data,len);
    //设置写事件，通知conn去发送
    _connHandler->addEvents(EV_WRITE);
}
/*
void TcpConnection::setReadMsgCallback(std::function<void ()> cb)
{
    this->_handleReadMsg = cb;
}
*/
