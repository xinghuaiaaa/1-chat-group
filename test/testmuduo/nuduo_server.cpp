#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net; // muduo::net::TcpServer

#include <functional> // 内含绑定器 bind

/*基于muduo网络库开发服务器程序
1. 组合TcpServer对象
2. 创建EventLoop事件循环对象的指针
3. 明确TcpServer构造函数需要什么参数, 输出ChatServer的构造函数----需要看源码
4. 在当前服务器类的 构造函数中, 注册 处理连接的 回调函数和 处理读写事件 的回调函数
5. 设置合适的 服务端线程数量, muduo 库会自己划分 i/o线程和 worker线程
*/

class ChatServer
{
public:
    // 构造函数 #3
    ChatServer(EventLoop *loop,               // 时间循环--反应堆
               const InetAddress &listenAddr, // 服务器地址结构--IP+PORT
               const string &nameArg)         // 服务器名字
        : _loop(loop), _server(loop, listenAddr, nameArg)

    {
        // 由于使用了 网络库, 就代表 不需要 自己写网络代码, 只需要关注 业务代码 漏出的 接口

        // 由于不知道什么时候发生, 因此 借助回调函数, 在事件发生后, 去进行回调, 执行回调函数里的代码即可

        // 1. 给服务器注册用户连接的 创建 和 断开 回调 #5
        // void setConnectionCallback(const ConnectionCallback& cb){..}  函数原型
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1)); // 传入的就是 回调函数, 而在这个类里, 写的回调函数是 成员方法, 有this指针, 但是只需要第二个传参,  因此使用 绑定器: this固定, const TcpConnectionPtr& 交给 传入者

        // 2. 给服务器注册用户 读写时间回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

        // 设置 服务器端的 线程数量
        // 设定为 4, 一个 IO线程, 3个worker线程
        _server.setThreadNum(4); // 如果不加, 默认是一个线程, 既要监听, 还要处理 读写----> 如果设置为 2, 则监听占用一个, 剩下一个, 要处理所有的读写事件,  效率都不高
    };

    // 开启事件循环 #4
    void start()
    {
        _server.start();
    }

private:
    // 专门处理用户的连接与断开  仅处理回调接口即可 #4
    //  经过epoll litsenfd accept, 到达accept 说明有新用户连接了
    /* 然而 网络库 已经封装好 socket相关的了, 仅暴露了 回调接口!!!*/
    void onConnection(const TcpConnectionPtr &conn) // 要学会 从源码 找 类型
    // typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
    {
        if (conn->connected()) // bool值
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << "  state:online" << endl;
        }
        else
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << "  state:off" << endl;

            conn->shutdown();   // close(fd)
            // _loop->quit(); //退出整个服务器
        }
    }

    // 专门处理用户的 读写事件
    void onMessage(const TcpConnectionPtr &conn, // 连接
                   Buffer *buffer,               // 缓冲区
                   Timestamp time)               // 时间
    {
        string buf = buffer->retrieveAllAsString();                          // 封装了 把数据 全部放到 字符串中
        cout << "recv data: " << buf << " time:" << time.toString() << endl; // time 也是 封装的, 把时间信息转化为字符串
        conn->send(buf);                                                     // 收到 并处理后返回, 这里测试 使用 原数据返回
    }

    TcpServer _server; // #1
    EventLoop *_loop;  // #2 epoll循环, 可以注册信号,捕捉信号, 时间循环
};

int main()
{
    EventLoop loop;                      // 相当于 创建epoll      muduo::net::EventLoop
    InetAddress addr("127.0.0.1", 6000); //  muduo::net::InetAddress
    ChatServer server(&loop, addr, "ChatServer");

    server.start();  // listenfd --- 使用epoll_ctl 添加到epoll上
    loop.loop(); // epoll_wait 以阻塞方式 等待新用户连接, 已连接用户的读写事件等

    return 0;
}