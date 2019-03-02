//2018年12月04日22:58:41
#pragma once
#include "LogStream.h"
#include <pthread.h>
#include <string>
#include <stdio.h>

class AsyncLogging;

class Logger
{
public: 
    Logger(const char* filename, int line);
    ~Logger();
    LogStream& stream() { return impl_.stream_; }
    
    static void setLogFileName(std::string filename)
    {
        logFileName_ = filename;
    }
    static std::string getLogFileName()
    {
        return logFileName_;
    }

private: 
    class Impl 
    {
    public: 
        Impl(const char* filename, int line);
        void formatTime();

        LogStream stream_;
        int line_;
        std::string basename_;
    };
    Impl impl_;
    static std::string logFileName_;
};

//对外接口 LOG
#define LOG Logger(__FILE__, __LINE__).stream()
//Logger -> Impl -> LogStream -> operator<< FixBuffer(4KB) -> output -> flush