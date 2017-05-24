/*======================================================
    > File Name: loop.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2017年05月16日 星期二 20时57分49秒
 =======================================================*/

#include<iostream>
//#include"../event_loop.h"
#include"../net_socket.h"
#include<unistd.h>

void callback()
{
    std::cout << "hanve new connection" << std::endl;
}


int main()
{
//    lnet::EventLoop  loop;
    lnet::ServerSocket   servSocket(10000,"127.0.0.1");
    servSocket.listen();
    /*
    lnet::IOEventHandlerPtr  acceptHandler(
            new lnet::IOEventHandler(
                servSocket.getSockfd(),EV_READ));
    acceptHandler->setReadCallback(callback);
    loop.addIOHandler(acceptHandler);
    loop.waitEvent();
    */
    char buf[20] = "hello";
    struct sockaddr_in addr;
    socklen_t    len = sizeof(addr);
    std::cout << servSocket.getSockfd() << std::endl;
    //int connfd = accept(servSocket.getSockfd(),(struct sockaddr*)&addr,&len);
    //std::cout << connfd << std::endl;
    lnet::Socket conn = servSocket.accept();
    std::cout << conn.getSockfd() << std::endl;
    int ret = write(conn.getSockfd(),buf,6);
    std::cout << ret << std::endl;
    return 0;
}
