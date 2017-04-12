/*======================================================
    > File Name: main.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月20日 星期日 16时14分29秒
 =======================================================*/

#include<iostream>
#include"../event_loop.h"
#include"../net_socket.h"

void callback()
{
    std::cout << "hanve new connection" << std::endl;
}

int main()
{
    lnet::EventLoop  loop;
    lnet::ServerSocket   servSocket(8000,"127.0.0.1");
    servSocket.listen();
    lnet::IOEventHandlerPtr  acceptHandler(
            new lnet::IOEventHandler(
                servSocket.getSockfd(),EV_READ));
    acceptHandler->setReadCallback(callback);
    loop.addIOHandler(acceptHandler);
    loop.waitEvent();
    return 0;
}
