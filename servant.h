/*======================================================
    > File Name: servant.h
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2017年05月15日 星期一 11时51分31秒
 =======================================================*/

#ifndef SERVANT_H
#define SERVANT_H

#include"tcpconnection.h"

#include<iostream>
#include<memory>

namespace lnet
{
/*
 * 业务逻辑抽象类
 */
class Servant
{
public:
    Servant(){}
    virtual ~Servant(){}
    //处理请求
    virtual void doRequest(const char *req) = 0;
    //向Client发送消息
    void sendResponse(const char *resp,const int len);
    //向其他servant对应的Client发送消息
    void sendResponseOther(Servant *servant,
            const char *resp,
            const int len);
    //获取自己的servant
    Servant* getServantSelf();
    void setTcpConn(lnet::TcpConnectionPtr conn);
private:
    //对应的用户连接（防止循环引用）
    std::weak_ptr<TcpConnection>     _conn;
};
typedef std::shared_ptr<lnet::Servant>    ServantPtr;

}
#endif
