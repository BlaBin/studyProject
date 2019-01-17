//Author: xcw
//Email:  xcw_coder@qq.com
//2019年01月17日23:26:09
#include "Epoll.h"
#include "Util.h"
#include "Channel.h"
#include "base/Logging.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <queue>
#include <deque>
#include <assert.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;

const int EVENTS_NUM = 4096;
const int EPOLLWAIT_TIME = 10000;

typedef shared_ptr<Channel> SP_Channel;

Epoll::Epoll():
    epollFd_(epoll_create1(EPOLL_CLOEXEC)),
    events_(EVENTS_NUM)
{
    assert(epollFd_ > 0);
}
Epoll::~Epoll()
{ }

void Epoll::epoll_add(SP_Channel request, int timeout)
{
    int fd = request->getFd();
    if(timeout > 0)
    {
        add_timer(request, timeout);
        fd2http_[fd] = request->getHolder();
    }
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();

    request->EqualAndUpdateLastEvents();

    fd2chan_[fd] = request;
    if(epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        perror("epoll add error");
        fd2chan_[fd].reset();
    }
}

void Epoll::epoll_del(SP_Channel request)
{
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getLastEvents();
    //event.events = 0;
    // request->EqualAndUpdateLastEvents()
    if(epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0)
    {
        perror("epoll_del error");
    }
    fd2chan_[fd].reset();
    fd2http_[fd].reset();
}

//返回活跃事件
std::vector<SP_Channel> Epoll::poll()
{
    while(true)
    {
        int event_count = epoll_wait(epollFd_, &*events_.begin(), events_.size(), EPOLLWAIT_TIME);
        if(event_count < 0)
            perror("epoll wait error");
        std::vector<SP_Channel> req_data = getEventRequest(event_count);
    }
}

void Epoll::handleExpired()
{
    timerManager_.handleExpiredEvent();
}

//分发处理函数
std::vector<SP_Channel> Epoll::getEventRequest(int events_num)
{
    std::vector<SP_Channel> req_data;
    for(int i = 0; i < events_num; i++)
    {
        //获取有事件产生的文件描述符
        int fd = events_[i].data.fd;
        SP_Channel cur_req = fd2chan_[fd];
        if(cur_req)
        {
            cur_req->setRevents(events_[i].events);
            cur_req->setEvents(0);
            //加入线程池之前将Timer和request分离
            //cur_req->seperateTimer();
            req_data.push_back(cur_req);
        }
        else
        {
            LOG << "SP cur_req is invalid";
        }
    }
    return req_data;
}

void Epoll::add_timer(SP_Channel request_data, int timeout)
{
    shared_ptr<HttpData> t = request_data->getHolder();
    if (t)
        timerManager_.addTimer(t, timeout);
    else
        LOG << "timer add fail";
}