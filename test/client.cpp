/*======================================================
    > File Name: client.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月20日 星期日 21时18分26秒
 =======================================================*/

#include<iostream>
#include"../net_socket.h"

int main()
{
    lnet::Socket cliSocket(8000,"127.0.0.1");
    cliSocket.connect();
    while(1)
    {}
    return 0;
}
