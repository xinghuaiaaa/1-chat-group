#include "chatserver.hpp"
#include "chatservice.hpp"

#include <functional>
using namespace std;
using namespace placeholders;

#include <string>
#include "json.hpp"
using namespace nlohmann;  // 别忘记这个 json作者

// 类外 定义成员函数
ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册连接回调函数
    _server.setConnectionCallback(std::bind(&ChatServer::onConnect, this, _1));

    // 注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置线程数
    _server.setThreadNum(4);
}

void ChatServer::start()
{
    _server.start();
}

void ChatServer::onConnect(const TcpConnectionPtr& conn)
{
    // 客户端断开连接 
    if(!conn->connected())
    {
        //客户端异常退出
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

void ChatServer::onMessage(const TcpConnectionPtr& conn,
    Buffer* buffer,
    Timestamp time)
{   
    string buf = buffer->retrieveAllAsString();
    // 数据反序列化
    json js = json::parse(buf);

     //达到的目的: 完全解耦网络模块的代码 和 业务模块的 代码
     //通过js["msgid"]获取=>业务handler=>传 conn, js, time等信息
    /*
        这样, 业务仅有 两行代码, 没有任何业务层的方法(login,reg等)调用
        仅需在 服务层内部, 做一个 业务相应的 回调
    */
     //获取msgid 对应的处理器
     auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());  //json键是字符串, 要换成整型----get方法, 会用就行---这个表述不对, 笔记有正确的
     // 回调消息绑定好的 事件处理器, 来执行相应的业务处理
    msgHandler(conn, js, time);

}