/*======================================================
    > File Name: servant.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2017年05月15日 星期一 15时26分23秒
 =======================================================*/

#include "servant.h"

using namespace lnet;

void Servant::sendResponse(const char *resp,const int len)
{
    //获取临时conn对象
    if(auto conn = this->_conn.lock())
    {
        conn->sendData(resp,len);
    }
}

void Servant::sendResponseOther(Servant *servant,
        const char *resp,
        const int len)
{
    //会在不同线程中调用
    if(auto conn = servant->_conn.lock())
    {
        conn->sendDataFromOther(resp,len);
    }
}

Servant* Servant::getServantSelf()
{
    return this;
}

void Servant::setTcpConn(lnet::TcpConnectionPtr conn)
{
    this->_conn = conn;
}
