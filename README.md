# High-performance web server written in C++
[![Build Status](https://travis-ci.org/xcw-coder/WebServer.svg?branch=master)](https://travis-ci.org/xcw-coder/WebServer)
![](https://img.shields.io/cran/l/devtools.svg)

本项目已经在 https://travis-ci.org 网站上通过了编译，详情可点击左上角绿色的passing图标进行查看。

## 项目简介
本项目为C++11编写的Web服务器，解析了get、head请求，可处理静态资源，支持HTTP长连接，支持管线化请求，并实现了异步日志，记录服务器运行状态。

测试页：http://www.xiongcw.xyz/

|Part I|Part II|Part III|Part IV|Part V|Part VI|Part VII|
|---|---|----|---|---|---|---|
|[并发模型](https://github.com/xcw-coder/WebServer/blob/master/%E5%B9%B6%E5%8F%91%E6%A8%A1%E5%9E%8B.md)|[连接维护](https://github.com/xcw-coder/WebServer/blob/master/%E8%BF%9E%E6%8E%A5%E7%BB%B4%E6%8A%A4.md)|[测试及改进](https://github.com/xcw-coder/WebServer/blob/master/%E6%B5%8B%E8%AF%95%E5%8F%8A%E6%94%B9%E8%BF%9B.md)|[项目目的](https://github.com/xcw-coder/WebServer/blob/master/%E9%A1%B9%E7%9B%AE%E7%9B%AE%E7%9A%84.md)|[遇到的困难](https://github.com/xcw-coder/WebServer/blob/master/%E9%81%87%E5%88%B0%E7%9A%84%E5%9B%B0%E9%9A%BE.md)| [版本历史](https://github.com/xcw-coder/WebServer/blob/master/%E7%89%88%E6%9C%AC%E5%8E%86%E5%8F%B2.md) | [面试问题](https://github.com/xcw-coder/WebServer/blob/master/%E9%9D%A2%E8%AF%95%E9%97%AE%E9%A2%98.md) |

## 开发环境  
* OS: Ubuntu 16.04
* Complier: g++ 5.4

## 编译
```
./build.sh
```
## 使用方法
```
./WebServer [-t thread_numbers] [-p port] [-l log_file_path(should begin with '/')]
```

## 技术要点
* 使用Epoll边沿触发的IO多路复用技术，非阻塞IO，使用Reactor模式
* 使用多线程充分利用多核CPU，并使用线程池避免线程频繁创建销毁的开销
* 使用基于小根堆的定时器关闭超时请求
* 主线程只负责accept请求，并以Round Robin的方式分发给其它IO线程(兼计算线程)，锁的争用只会出现在主线程和某一特定线程中
* 使用eventfd实现了线程的异步唤醒
* 使用双缓冲区技术实现了简单的异步日志系统
* 为减少内存泄漏的可能，使用智能指针等RAII机制
* 使用状态机解析了HTTP请求,支持管线化
* 支持优雅关闭连接 

## 模型选择
并发模型为Reactors+非阻塞IO，新连接采用Round Robin(轮叫)的方式分配，详细介绍请参考[并发模型](https://github.com/xcw-coder/WebServer/blob/master/%E5%B9%B6%E5%8F%91%E6%A8%A1%E5%9E%8B.md)

![并发模型](https://github.com/xcw-coder/WebServer/blob/master/pictures/model.png)

## 代码统计
![codeStatistics](https://github.com/xcw-coder/WebServer/blob/master/pictures/codeStatistics.png)

## 其他
除了项目基本的代码，用开源测试工具[Webbench](https://github.com/xcw-coder/WebBench)来对服务器进行压测。
