//Author: xcw
//Email:  xcw_coder@qq.com
//2019年01月17日15:06:42
#pragma once
#include <string>
#include <cstdlib>

ssize_t readn(int fd, void* buf, size_t n);
ssize_t readn(int fd, std::string& inBuffer, bool& zero);
ssize_t readn(int fd, std::string& inBuffer);
ssize_t writen(int fd, void* buf, size_t n);
ssize_t writen(int fd, std::string& buf);
void handle_for_sigpipe();
void setSocketNonBlocking(int fd);
void setSocketNoDelay(int fd);
void setSocketNoLinger(int fd);
void shutDownWR(int fd);
int socket_bind_listen(int port);