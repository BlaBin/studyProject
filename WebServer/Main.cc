//Author: xcw
//Email:  xcw_coder@qq.com
//2018年12月27日21:20:34
#include "EventLoop.h"
#include "Server.h"
#include "base/Logging.h"
#include <getopt.h>       //for getopt()
#include <string>

int main(int argc, char* argv[])
{
    int threadNum = 4;
    int port = 8888;
    std::string logPath = "./WebServer.log";

    //解析args
    int opt;
    const char* str = "t:l:p";
    while((opt = ::getopt(argc, argv, str)) != -1)
    {
        switch(opt)
        {
            case 't':
            {
                threadNum = atoi(optarg);
                break;
            }
            case 'l': 
            {
                logPath = optarg;
                if(logPath.size() < 2 || optarg[0] != '/')
                {
                    printf("logPath should start with \"/\"\n");
                    abort();
                }
                break;
            }
            case 'p':
            {
                port = atoi(optarg);
                break;
            }
            default: break;
        }
    }
    Logger::setLogFileName(logPath);
    //STL库在多线程上的应用
    #ifndef _PTHREADS
        LOG << "_PTHREADS is not defined !";
    #endif
    EventLoop mainLoop;
    Server myHttpServer(&mainLoop, threadNum, port);
    myHttpServer.start();
    mainLoop.loop();
    return 0;
}