//Author: xcw
//Email:  xcw_coder@qq.com
//2018年12月09日16:51:14
#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <memory>
#include <sys/epoll.h>
#include <functional>
#include <unistd.h>

class EventLoop;
class TimerNode;
class Channel;

enum ProcessState
{
    STATE_PARSE_URI = 1,
    STATE_PARSE_HEADERS,
    STATE_RECV_BODY,
    STATE_ANALYSIS,
    STATE_FINISH
};

enum URIState
{
    PARSE_URI_AGAIN = 1,    //没有读取到完整请求行
    PARSE_URI_ERROR,
    PARSE_URI_SUCCESS
};

enum HeaderState
{
    PARSE_HEADER_SUCCESS = 1,
    PARSE_HEADER_AGAIN,
    PARSE_HEADER_ERROR
};

enum AnalysisState
{
    ANALYSIS_SUCCESS = 1,
    ANALYSIS_ERROR
};

enum ParseState 
{
    H_START = 0,
    H_KEY,
    H_COLON,
    H_SPACES_AFTER_COLON,
    H_VALUE,
    H_CR,
    H_LF,
    H_END_CR,
    H_END_LF
};

enum ConnectionState
{
    H_CONNECTED = 0,
    H_DISCONNECTING,
    H_DISCONNECTED
};

enum HttpMethod
{
    METHOD_POST = 1,
    METHOD_GET,
    METHOD_HEAD
};

enum HttpVersion
{
    HTTP_10 = 1,
    HTTP_11
};

class MimeType
{
public: 
    static std::string getMime(const std::string& suffix);

private: 
    static void init();
    static std::unordered_map<std::string, std::string> mime;
    static pthread_once_t once_control;
    MimeType();
    MimeType(const MimeType& m);
    
};

class HttpData: public std::enable_shared_from_this<HttpData>
{
public: 
    HttpData(EventLoop* loop, int connfd);
    ~HttpData() { close(fd_); }
    void reset();
    void seperateTimer();
    void linkTimer(std::shared_ptr<TimerNode> mtimer)
    {
        timer_ = mtimer;
    }
    std::shared_ptr<Channel> getChannel() { return channel_; }
    EventLoop* getLoop() {return loop_; }
    void handleClose();
    void newEvent();

private: 
    EventLoop* loop_;
    std::shared_ptr<Channel> channel_;
    int fd_;
    std::string inBuffer_;
    std::string outBuffer_;
    bool error_;
    ConnectionState connectionState_;

    HttpMethod method_;       //请求方法
    HttpVersion version_;     //HTTP版本
    std::string filename_;
    std::string path_;
    int nowReadPos_;
    ProcessState state_;
    ParseState hState_;
    bool keepAlive_;
    std::map<std::string, std::string> headers_;
    std::weak_ptr<TimerNode> timer_;

    void handleRead();
    void handleWrite();
    void handleConn();
    void handleError(int fd, int err_num, std::string short_msg);
    URIState parseURI();
    HeaderState parseHeaders();
    AnalysisState analysisRequest();
};