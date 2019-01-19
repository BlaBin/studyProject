//Author: xcw
//Email:  xcw_coder@qq.com
//2018年12月09日15:18:03
#include "Util.h"
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

const int MAX_BUF = 4096;
ssize_t readn(int fd, void* buf, size_t n)
{
    size_t nleft = n;
    ssize_t nread = 0;
    ssize_t readSum = 0;
    char* ptr = (char*)buf;
    while(nleft > 0)
    {
        if((nread = ::read(fd, ptr, nleft)) < 0)
        {
            if(errno == EINTR)
                nread = 0;
            else if(errno == EAGAIN)
                return readSum;
            else
                return -1;
        }
        else if(nread == 0)
            break;
        readSum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readSum;
}

ssize_t readn(int fd, std::string& inBuffer, bool& zero)
{
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while(true)
    {
        char buf[MAX_BUF];
        if((nread = ::read(fd, buf, MAX_BUF)) < 0)
        {
            if(errno == EINTR)
                continue;
            else if(errno == EAGAIN)
                return readSum;
            else
            {
                perror("read error");
                return -1;
            }
        }
        else if(nread == 0)
        {
            zero = true;
            break;
        }
        readSum += nread;
        inBuffer += std::string(buf, buf+nread);
    }
    return readSum;
}

ssize_t readn(int fd, std::string& inBuffer)
{
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while(true)
    {
        char buf[MAX_BUF];
        if((nread = ::read(fd, buf, MAX_BUF)) < 0)
        {
            if(errno == EINTR)
                continue;
            else if(errno == EAGAIN)
                return readSum;
            else
            {
                perror("read error");
                return -1;
            }
        }
        else if(nread == 0)
            break;
        readSum += nread;
        inBuffer += std::string(buf, buf+nread);
    }
    return readSum;
}

ssize_t writen(int fd, void* buf, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    char* ptr = (char*)buf;

    while(nleft > 0)
    {
        if((nwritten = ::write(fd, ptr, nleft)) <= 0)
        {
            if(nwritten < 0)
            {
                if(errno == EINTR)
                {
                    nwritten = 0;
                    continue;
                }
                else if(errno == EAGAIN)
                    return writeSum;
                else 
                    return -1;
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writeSum;
}

ssize_t writen(int fd, std::string& buf)
{
    size_t nleft = buf.size();
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    const char* ptr = buf.c_str();
    while(nleft > 0)
    {
        if((nwritten = ::write(fd, ptr, nleft)) <= 0)
        {
            if(nwritten < 0)
            {
                if(errno == EINTR)
                {
                    nwritten = 0;
                    continue;
                }
                else if(errno == EAGAIN)
                    return writeSum;
                else 
                    return -1;
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writeSum;
}

void handle_for_sigpipe()
{
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGPIPE, &act, 0);
}

int setSocketNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1)
        return -1;
    flags |= O_NONBLOCK;
    int ret = ::fcntl(fd, F_SETFL, flags); 
    if(ret == -1)
        return -1;

    flags = fcntl(fd, F_GETFD, 0);
    if(flags == -1)
        return -1;
    flags |= FD_CLOEXEC;
    ret = ::fcntl(fd, F_SETFD, flags);
    if(ret == -1)
        return -1;
    return ret;
}

//禁止nagle算法
void setSocketNoDelay(int fd)
{
    int enable = 1;
    ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof enable);
}

void setScoketNoLinger(int fd)
{
    struct linger linger_;
    linger_.l_onoff = 1;
    linger_.l_linger = 30;
    ::setsockopt(fd, SOL_SOCKET, SO_LINGER, &linger_, sizeof linger_);
}

void shutDownWR(int fd)
{
    ::shutdown(fd, SHUT_WR);
}

int socket_bind_listen(int port)
{
    if(port < 0 || port > 65535)
        return -1;
    
    int listenFd = 0;
    if((listenFd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;
    
    //消除bind时， "Address already in use" 错误
    int optval = 1;
    if(::setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) == -1)
        return -1;
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned int) port);
    if(::bind(listenFd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        return -1;

    if(::listen(listenFd, 2018) == -1)
        return -1;
    
    return listenFd;
}