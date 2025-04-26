#ifndef REDIS_H
#define REDIS_H

#include <hiredis/hiredis.h>
#include <thread>
#include <functional>
using namespace std;

class Redis
{
public:
    Redis();
    ~Redis();

    // 链接redis服务器
    bool connect();

    // 向redis指定的频道channel发布消息
    bool publish(const int channel, const string message);

    // 向redis指定的频道channel订阅消息
    bool subscribe(const int channel);

    // 向redis指定的频道channel取消订阅
    bool unsubscribe(const int channel);

    // 在独立线程中接受订阅频道的消息
    void oberver_channel_message();

    // 初始化向业务层上报消息的回调函数
    void init_notify_message_handler(function<void(int, string)> fn);

private:
    // hiredis 同步上下文对象, 负责publish
    redisContext *_publish_context;

    // hiredis 同步上下文对象, 负责subscribe
    redisContext *_subscribe_context;

    // 回调操作, 收到订阅消息, 给service层上报
    function<void(int, string)> _notify_message_handler;
    /*
    int, string
    对应 redis 回应的 (2)(3)
    1) "message"
    2) "13"
    3) "hello"
    */
};

#endif