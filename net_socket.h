#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string>
#include<iostream>

namespace lnet
{
//Socket地址结构
class SocketAddr
{
public:
    SocketAddr();
    SocketAddr(struct sockaddr_in addr);
    SocketAddr(const char *str_port,const char *ip = NULL);
    SocketAddr(const int port,const char *ip = NULL);
    ~SocketAddr();
    //不指定Ip默认绑定任意网络接口
    void setSockAddr(const int port,const char *ip = NULL);
    void setSockAddr(const char *str_port,const char *ip = NULL);
    struct sockaddr_in getSockAddr();
    int getPortBySockAddr();
    std::string getIpBySockAddr();
private:
    struct sockaddr_in  address;
};

//通用socket选项和方法
class SocketOption
{
public:
    SocketOption();
    //SocketOption(int using_fd);
    ~SocketOption();

    int getSockfd();
    void setSockfd(int using_fd);
    //默认创建TCP socket
    int createTcpSocket(int type = SOCK_STREAM);
    //UDP socket
    int createUdpSocket(int type = SOCK_DGRAM);
    int close();
    int shutdown(int howto = SHUT_RDWR);
    int setNonBlocking();
    ssize_t recv(void *buf,size_t len,int flags = 0);
    ssize_t send(void *buf,size_t len,int flags = 0);
    int getsockopt(
            int level,
            int optname,
            void* optvalue,
            socklen_t* optlen);
    int getSockRecvBufSize();
    int getSockSendBufSize();
    int getSockLinger(struct linger *lger);

    int setsockopt(
            int level,
            int optname,
            void* optvalue,
            socklen_t optlen);
    int setSockReuseAddr(int yes = 1);
    int setSockRecvBufSize(int size);
    int setSockSendBufSize(int size);
    int setSockLinger(int onoff = 0,int linger = 0);
protected:
    int                  fd;
};

//Socket对象
class Socket : public SocketOption
{
public:
    //不作初始化
    Socket();
    //默认创建TCP socket,并初始化服务器地址
    Socket(SocketAddr serv_addr);
    Socket(const int port,const char *ip = NULL);
    ~Socket();
    //为客户端绑定地址
    int bindClientAddr(SocketAddr addr);
    int bindClientAddr(const int port,const char *ip = NULL);
    //获取服务器地址信息
    SocketAddr getServerAddr();
    SocketAddr getClientAddr();
    //设置服务器地址信息
    void setServerAddr(const int port,const char *ip = NULL);
    void setServerAddr(SocketAddr addr);
    //设置客户端地址信息
    void setClientAddr(SocketAddr addr);
    //向服务器发起连接
    int connect();
private:
    SocketAddr   client_addr;
    SocketAddr   server_addr;
};

/*
 * ServerSocket 主要用于服务器监听客户端连接
 */
class ServerSocket : public SocketOption
{
public:
    //不作初始化
    ServerSocket();
    //默认TCP socket
    ServerSocket(SocketAddr serv_addr);
    ServerSocket(const int port,const char *ip = NULL);
    ~ServerSocket();
    int bind();
    void setServerAddr(SocketAddr addr);
    void setServerAddr(const int port,const char *ip = NULL);
    int listen(int backlog = 10);
    Socket accept();
    int accept(struct sockaddr *addr,socklen_t *addrlen);
private:
    SocketAddr  server_addr;
};

}

#endif //NET_SOCKET_H
