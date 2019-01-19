set(SRCS
    Channel.cc
    Epoll.cc
    EventLoop.cc
    EventLoopThread.cc
    EventLoopThreadPool.cc
    HttpData.cc
    Main.cc
    Server.cc
    ThreadPool.cc
    Timer.cc
    Util.cc
)
include_directories(${PROJECT_SOURCE_DIR}/base)


add_executable(WebServer ${SRCS})
target_link_libraries(WebServer server_base)


add_subdirectory(base)
add_subdirectory(tests)

#Author: xcw
#Email:  xcw_coder@qq.com
#2018年12月27日22:18:04