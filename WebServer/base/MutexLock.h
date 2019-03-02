//2018年11月23日20:24:57
//封装mutex
#pragma once
#include "noncopyable.h"
#include <pthread.h>

class MutexLock: noncopyable
{
public:
    MutexLock()
    {
        pthread_mutex_init(&mutex_, NULL);     //RAII技法
    }
    ~MutexLock()
    {
        pthread_mutex_lock(&mutex_);
        pthread_mutex_destroy(&mutex_);
    }
    void lock()
    {
        pthread_mutex_lock(&mutex_);
    }
    void unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }
    pthread_mutex_t* get()
    {
        return &mutex_;
    }

private:
    friend class Condition;
    pthread_mutex_t mutex_;
};

class MutexLockGuard: noncopyable
{
public:
    explicit MutexLockGuard(MutexLock& mutex): mutex_(mutex)
    {
        mutex.lock();
    }
    ~MutexLockGuard()
    {
        mutex_.unlock();
    }
    
private:
    MutexLock& mutex_;
};