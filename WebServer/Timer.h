//Author: xcw
//Email:  xcw_coder@qq.com
//2019年01月17日20:38:08
#pragma once
#include "HttpData.h"
#include "base/noncopyable.h"
#include "base/MutexLock.h"
#include <unistd.h>
#include <memory>
#include <queue>
#include <deque>

class TimerNode
{
public: 
    typedef std::shared_ptr<HttpData> HttpDataPtr;
    TimerNode(HttpDataPtr requestData, int timeout);
    ~TimerNode();
    TimerNode(TimerNode& tn);
    void update(int timeout);
    bool isValid();
    void clearReq();
    void setDeleted() { deleted_ = true; }
    bool isDeleted() const { return deleted_; }
    size_t getExpTime() const { return expiredTime_; } 

private: 
    bool deleted_;
    size_t expiredTime_;
    HttpDataPtr SPHttpData;    
};

struct TimerCmp
{
    typedef std::shared_ptr<TimerNode> TimerNodePtr;
    bool operator()(TimerNodePtr &a, TimerNodePtr &b) const
    {
        return a->getExpTime() > b->getExpTime();
    }
};

class TimerManager
{
public:
    TimerManager();
    ~TimerManager();
    void addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout);
    void handleExpiredEvent();

private:
    typedef std::shared_ptr<TimerNode> SPTimerNode;
    std::priority_queue<SPTimerNode, std::deque<SPTimerNode>, TimerCmp> timerNodeQueue;
    //MutexLock lock;
};