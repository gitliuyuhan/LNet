#include <netinet/in.h>
#include <strings.h>
#include "net_base.h"
#include "net_socket.h"

using namespace lnet;

/*
 * Socket地址结构
 */
SocketAddr::SocketAddr()
{}
SocketAddr::SocketAddr(struct sockaddr_in addr)
{
    address = addr;
}
SocketAddr::SocketAddr(const int port,const char *ip)
{
    setSockAddr(port,ip);
}
SocketAddr::SocketAddr(const char *str_port,const char *ip)
{
    setSockAddr(str_port,ip);
}
SocketAddr::~SocketAddr()
{}
void SocketAddr::setSockAddr(const int port,const char *ip)
{
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if(ip)
    {
        inet_pton(AF_INET,ip,&address.sin_addr);
    }
    else
    {
        address.sin_addr.s_addr = htonl(INADDR_ANY);
    }
}
void SocketAddr::setSockAddr(const char *str_port,const char *ip)
{
    int port = atoi(str_port);
    this->setSockAddr(port,ip);
}
struct sockaddr_in SocketAddr::getSockAddr()
{
    return address;
}
int SocketAddr::getPortBySockAddr()
{
    return ntohs(address.sin_port);
}
std::string SocketAddr::getIpBySockAddr()
{
    char str_ip[INET_ADDRSTRLEN] = "";
    inet_ntop(AF_INET,&address.sin_addr,str_ip,INET_ADDRSTRLEN);
    return str_ip;
}

/*
 * 通用socket选项和方法
 */
SocketOption::SocketOption() : fd(-1)
{
    //this->createTcpSocket(SOCK_STREAM);
}
/*
SocketOption::SocketOption(int using_fd)
{
    this->fd = using_fd;
}
*/
SocketOption::~SocketOption()
{}

int SocketOption::getSockfd()
{
    return this->fd;
}
void SocketOption::setSockfd(int using_fd)
{
    this->fd = using_fd;
}
//默认创建TCP socket
int SocketOption::createTcpSocket(int type)
{
    fd = socket(PF_INET,type,0);
    return fd;
}
//UDP socket
int SocketOption::createUdpSocket(int type)
{
    fd = socket(PF_INET,type,0);
    return fd;
}
int SocketOption::close()
{
    return ::close(fd);
}
int SocketOption::shutdown(int howto)
{
    return ::shutdown(fd,howto);
}
int SocketOption::setNonBlocking()
{
    return lnet::setNonBlockFd(this->fd);
}
ssize_t SocketOption::recv(void *buf,size_t len,int flags)
{
    return ::recv(fd,buf,len,flags);
}
ssize_t SocketOption::send(void *buf,size_t len,int flags)
{
    return ::send(fd,buf,len,flags);
}
int SocketOption::getsockopt(
        int level,
        int optname,
        void* optvalue,
        socklen_t* optlen)
{
    return ::getsockopt(fd,level,optname,optvalue,optlen);
}
int SocketOption::getSockRecvBufSize()
{
    int size;
    int len = sizeof(size);
    this->getsockopt(SOL_SOCKET,SO_RCVBUF,&size,(socklen_t*)&len);
    return size;//失败return -1
}
int SocketOption::getSockSendBufSize()
{
    int size;
    int len = sizeof(size);
    this->getsockopt(SOL_SOCKET,SO_SNDBUF,&size,(socklen_t*)&len);
    return size;//失败return -1
}
int SocketOption::getSockLinger(struct linger *lger)
{
    int len = sizeof(struct linger);
    return this->getsockopt(
            SOL_SOCKET,
            SO_LINGER,
            lger,
            (socklen_t*)&len);
}

int SocketOption::setsockopt(
        int level,
        int optname,
        void* optvalue,
        socklen_t optlen)
{
    return ::setsockopt(fd,level,optname,optvalue,optlen);
}
int SocketOption::setSockReuseAddr(int yes)
{
    int reuse = yes;
    return this->setsockopt(
            SOL_SOCKET,
            SO_REUSEADDR,
            &reuse,
            sizeof(reuse));
}
int SocketOption::setSockRecvBufSize(int size)
{
    return this->setsockopt(
            SOL_SOCKET,
            SO_RCVBUF,
            &size,
            sizeof(size));
}
int SocketOption::setSockSendBufSize(int size)
{
    return this->setsockopt(
            SOL_SOCKET,
            SO_SNDBUF,
            &size,
            sizeof(size));
}
int SocketOption::setSockLinger(int onoff,int linger)
{
    struct linger lger;
    lger.l_onoff = onoff;
    lger.l_linger = linger;
    return this->setsockopt(
            SOL_SOCKET,
            SO_LINGER,
            &lger,
            sizeof(lger));
}

/*
 * Socket对象
 */
Socket::Socket()
{}
Socket::Socket(SocketAddr serv_addr)
{
    this->setServerAddr(serv_addr);
    this->createTcpSocket();
}
Socket::Socket(const int port,const char *ip)
{
    this->setServerAddr(port,ip);
    this->createTcpSocket();
}
Socket::~Socket()
{}
int Socket::bindClientAddr(SocketAddr addr)
{
    this->client_addr = addr;
    struct sockaddr_in tmp = client_addr.getSockAddr();
    return ::bind(
            this->fd,
            (struct sockaddr*)&tmp,
            sizeof(tmp));
}
int Socket::bindClientAddr(const int port,const char *ip)
{
    return bindClientAddr(SocketAddr(port,ip));
}
SocketAddr Socket::getServerAddr()
{
    return this->server_addr;
}
SocketAddr Socket::getClientAddr()
{
    return this->client_addr;
}
int Socket::connect()
{
    struct sockaddr_in addr = server_addr.getSockAddr();
    return ::connect(
            this->fd,
            (struct sockaddr*)&addr,
            sizeof(addr));
}
void Socket::setServerAddr(const int port,const char *ip)
{
    server_addr.setSockAddr(port,ip);
}
void Socket::setServerAddr(SocketAddr addr)
{
    server_addr = addr;
}
void Socket::setClientAddr(SocketAddr addr)
{
    client_addr = addr;
}

/*
 * ServerSocket
 */
ServerSocket::ServerSocket()
{}
ServerSocket::ServerSocket(const int port,const char *ip)
{
    setServerAddr(port,ip);
    this->createTcpSocket();
    this->bind();
}
ServerSocket::ServerSocket(SocketAddr serv_addr)
{
    int port = serv_addr.getPortBySockAddr();
    std::string ip = serv_addr.getIpBySockAddr();
    setServerAddr(port,ip.c_str());
    this->createTcpSocket();
    this->bind();
}

ServerSocket::~ServerSocket()
{}

int ServerSocket::bind()
{
    struct sockaddr_in addr = server_addr.getSockAddr();
    return ::bind(
            this->fd,
            (struct sockaddr*)&addr,
            sizeof(addr));
}
void ServerSocket::setServerAddr(SocketAddr addr)
{
    server_addr = addr;
}
void ServerSocket::setServerAddr(const int port,const char *ip)
{
    server_addr.setSockAddr(port,ip);
}
int ServerSocket::listen(int backlog)
{
    return ::listen(this->fd,backlog);
}
Socket ServerSocket::accept()
{
    struct sockaddr_in    addr;
    socklen_t    addrlen = sizeof(addr);
    int connfd = ::accept(this->fd,(struct sockaddr*)&addr,&addrlen);
    Socket sock;
    sock.setSockfd(connfd);
    sock.setServerAddr(server_addr);
    sock.setClientAddr(SocketAddr(addr));
    return sock;
}
int ServerSocket::accept(struct sockaddr *addr,socklen_t *addrlen)
{
    return ::accept(this->fd,addr,addrlen);
}


