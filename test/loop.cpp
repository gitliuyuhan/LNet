/*======================================================
    > File Name: loop.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2017年05月16日 星期二 20时57分49秒
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
