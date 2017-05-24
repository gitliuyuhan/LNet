/*======================================================
    > File Name: client.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月20日 星期日 21时18分26秒
 =======================================================*/

#include<iostream>
#include<string>
#include<string.h>
#include"../net_socket.h"

int main()
{
    lnet::Socket cliSocket(10000,"127.0.0.1");
    cliSocket.connect();
    while(1)
    {
        std::string  buf;
        std::cin >> buf;
        int len = buf.length();
        std::cout << len << std::endl;
        int ret = send(cliSocket.getSockfd(),(void*)&len,4,0);
        std::cout << ret << std::endl;
        ret = send(cliSocket.getSockfd(),(void*)buf.c_str(),len,0);
        std::cout << ret << std::endl;
        char rbuf[1024];
        ::memset(rbuf,'\0',1024);
        ret = recv(cliSocket.getSockfd(),rbuf,1024,0);
        std::cout << rbuf << std::endl;
    }
    return 0;
}
