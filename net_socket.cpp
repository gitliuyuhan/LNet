#include <netinet/in.h>
#include "net_base.h"
#include "net_socket.h"

namespace lnet
{
    /*
    * Socket地址结构
    */
    SocketAddr::SocketAddr()
    {}
    SocketAddr::SocketAddr(struct sockaddr_in addr)
    {
        address = addr;
    }
    SocketAddr::SocketAddr(const int port,const char *ip = NULL)
    {
        setSockAddr(port,ip);
    }
    SocketAddr::SocketAddr(const char *str_port,const char *ip = NULL)
    {
        setSockAddr(str_port,ip);
    }
    SocketAddr::~SocketAddr()
    {}
    void SocketAddr::setSockAddr(const int port,const char *ip = NULL)
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
    void SocketAddr::setSockAddr(const char *str_port,const char *ip = NULL)
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
    SocketOption::SocketOption()
    {
        this->createTcpSocket();
    }
    SocketOption::SocketOption(int using_fd)
    {
        this->fd = using_fd;
    }
    SocketOption::~SocketOption()
    {}

    int SocketOption::getSockfd()
    {
        return this->fd;
    }
    //默认创建TCP socket
    int SocketOption::createTcpSocket(int type = SOCK_STREAM)
    {
        fd = socket(PF_INET,type,0);
        return fd;
    }
    //UDP socket
    int SocketOption::createUdpSocket(int type = SOCK_DGRAM)
    {
        fd = socket(PF_INET,type,0);
        return fd;
    }
    int SocketOption::close()
    {
        return close(fd);
    }
    int SocketOption::shutdown(int howto = SHUT_RDWR)
    {
        return shutdown(fd,howto);
    }
    int SocketOption::setNonBlocking()
    {
        return lnet::setNonBlockFd(this->fd);
    }
    ssize_t SocketOption::recv(void *buf,size_t len,int flags = 0)
    {
        return recv(fd,buf,len,flags);
    }
    ssize_t SocketOption::send(void *buf,size_t len,int flags = 0)
    {
        return send(fd,buf,len,flags);
    }
    int SocketOption::getsockopt(
            int level,
            int optname,
            void* optvalue,
            socklen_t* optlen)
    {
        return setsockopt(fd,level,optname,optvalue,optlen);
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
        return setsockopt(fd,level,optname,optvalue,optlen);
    }
    int SocketOption::setSockReuseAddr(int yes = 1)
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
    int SocketOption::setSockLinger(int onoff = 0,int linger = 0)
    {
        struct linger lger;
        lger.l_onoff = onoff;
        lger.l_linger = linger;
        return this->setsockopt(
                SOL_SOCKET,
                SO_LINGER,
                &lger
                sizeof(lger));
    }

    /*
     * Socket对象
     */
    Socket::Socket() : SocketOption(-1)
    {}
    Socket::Socket(SocketAddr serv_addr)
    {
        this->setServerAddr(serv_addr);
        this->createTcpSocket();
    }
    Socket::Socket(const int port,const char *ip = NULL)
    {
        this->setServerAddr(port,ip);
        this->createTcpSocket();
    }
    int Socket::bindClientAddr(SocketAddr addr)
    {
        this->client_addr = addr;
        return bind(
                this->fd,
                client_addr.getSockAddr(),
                sizeof(client_addr.getSockAddr()));
    }
    int Socket::bindClientAddr(const int port,const char *ip = NULL)
    {
        return bindClientAddr(SocketAddr(port,ip));
    }
    SocketAddr Socket::getServerAddr()
    {
        this->server_addr;
    }
    SocketAddr Socket::getClientAddr()
    {
        this->client_addr;
    }
    int Socket::connect()
    {
        return connect(
                this->fd,
                &server_addr.getSockAddr(),
                sizeof(server_addr.getSockAddr));
    }
    void Socket::setServerAddr(const int port,const char *ip = NULL)
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
    ServerSocket::ServerSocket() : SocketOption(-1)
    {}
    ServerSocket::ServerSocket(const int port,const char *ip = NULL)
    {
        setServerAddr(port,ip);
        this->createTcpSocket();
        this->bind();
    }
    ServerSocket::ServerSocket(SocketAddr serv_addr)
    {
        setServerAddr(port,ip);
        this->createTcpSocket();
        this->bind();
    }
    int ServerSocket::bind()
    {
        return bind(
                this->fd,
                (struct sockaddr*)&server_addr.getSockAddr(),
                sizeof(server_addr.getSockAddr()));
    }
    void ServerSocket::setServerAddr(SocketAddr addr)
    {
        server_addr = addr;
    }
    void ServerSocket::setServerAddr(const int port,const char *ip = NULL)
    {
        server_addr.setSockAddr(port,ip);
    }
    int ServerSocket::listen(int backlog = 10)
    {
        return listen(this->fd,backlog);
    }
    Socket ServerSocket::accept()
    {
        struct sockaddr_in    addr;
        socklen_t    addrlen = sizeof(addr);
        int connfd = accept(this->fd,(struct sockaddr*)&addr,&addrlen);
        Socket sock(connfd);
        sock.setServerAddr(server_addr);
        sock.setClientAddr(client_addr);
        return sock;
    }
    int ServerSocket::accept(struct sockaddr *addr,socklen_t *addrlen)
    {
        return accept(this->fd,addr,addrlen);
    }

}
