#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

// 聊天服务器主类
class ChatServer
{
public:
    // 初始化聊天服务器对象
    ChatServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const string &nameArg);
    // 启动服务
    void start();

private:
    // 上报连接 相关信息的回调
    void onConnect(const TcpConnectionPtr& conn);
    // 上报读写事件相关信息的 回调
    void onMessage(const TcpConnectionPtr& conn,
        Buffer* buf,
        Timestamp time);

    TcpServer _server; // 组合的muduo库, 实现服务器功能的 类对象
    EventLoop *_loop;  // 指向事件循环对象的 指针
};

#endif 