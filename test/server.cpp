/*======================================================
    > File Name: main.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月20日 星期日 16时14分29秒
 =======================================================*/

#include<iostream>
#include"../tcpserver.cpp"
#include"../servant.h"

class TestServant : public lnet::Servant
{
public:
    void doRequest(const char *req)
    {
        std::cout << req << std::endl;
        this->sendResponse((const char*)"recv success",13);
    }
};

int main()
{
    lnet::TcpServer<TestServant>  s(10000,"127.0.0.1");
    s.start(1);
    return 0;
}
