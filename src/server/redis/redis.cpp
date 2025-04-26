#include "redis.hpp"
#include <iostream>
using namespace std;

Redis::Redis()
    : _publish_context(nullptr), _subscribe_context(nullptr)
{
}

Redis::~Redis()
{
    if (_publish_context != nullptr)
    {
        redisFree(_publish_context);
    }
    if (_subscribe_context != nullptr)
    {
        redisFree(_subscribe_context);
    }
}

bool Redis::connect()
{
    // publish连接redis服务器
    _publish_context = redisConnect("127.0.0.1", 6379);
    if (_publish_context == nullptr || _publish_context->err)
    {
        cout << "connect redis server failed" << endl;
        return false;
    }

    // subscibe连接redis服务器
    _subscribe_context = redisConnect("127.0.0.1", 6379);
    if (_subscribe_context == nullptr || _subscribe_context->err)
    {
        cout << "connect redis server failed" << endl;
        return false;
    }

    // 在独立线程(是线程)中, 监听通道上的事件, 有消息给业务层进行上报
    thread t([&]()
             { oberver_channel_message(); });
    t.detach();

    cout << "connect redis server success" << endl;
    return true;
}

// 向redis指定的频道channel发布消息
bool Redis::publish(const int channel, const string message)
{
    redisReply *reply = (redisReply *)redisCommand(_publish_context, "PUBLISH %d %s", channel, message.c_str());
    if (reply == nullptr)
    {
        cout << "publish message failed" << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

// 向redis指定的频道channel订阅消息
bool Redis::subscribe(const int channel)
{
    // SUBSCRIBE命令本身会造成线程阻塞等待通道里面发生消息,这里只做订阅通道,不接收通道消息
    //  通道消息的接收专门在observer_channel_message函数中的独立线程中进行----这就是接收函数存在的意义
    // 只负责发送命令,不阻塞接收redis server响应消息,否则和notifyMsg线程抢占响应资源
    if (REDIS_ERR == redisAppendCommand(this->_subscribe_context, "SUBSCRIBE %d", channel))
    {
        cerr << "subscribe command failed!" << endl;
        return false;
    }

    //  redisBufferWrite 可以循环发送缓冲区, 直到缓冲区数据发送完毕
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subscribe_context, &done))
        {
            cerr << "subscribe command failed!" << endl;
            return false;
        }
    }
    // redisGetReply
    /*
    redisCommand 包含的 3个 函数:
    redisAppendCommand  （命令格式化 + 缓存到本地）
    redisBufferWrite    （把命令发出去）
    redisGetReply       （阻塞等待服务器返回结果）-- 在单独的一个接收线程上!!!
    */
    return true;
}

// 向redis指定的频道channel取消订阅
bool Redis::unsubscribe(const int channel)
{
    if (REDIS_ERR == redisAppendCommand(this->_subscribe_context, "UNSUBSCRIBE %d", channel))
    {
        cerr << "unsubscribe command failed!" << endl;
        return false;
    }
    //  redisBufferWrite 可以循环发送缓冲区, 直到缓冲区数据发送完毕
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subscribe_context, &done))
        {
            cerr << "subscribe command failed!" << endl;
            return false;
        }
    }
    return true;
}

// 在独立线程中接受订阅频道的消息--存在的意义 看订阅那里
void Redis::oberver_channel_message()
{
    redisReply *reply = nullptr;
    while (REDIS_OK == redisGetReply(this->_subscribe_context, (void **)&reply))
    {
        // 订阅收到的消息 是一个带三个元素的数组
        if (reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr)
        {
            // 给业务层上报通道上发生的消息
            _notify_message_handler(atoi(reply->element[1]->str), reply->element[2]->str);
            /*
            数组的下标1, 2
            对应 redis 回应的 (2)(3)
            1) "message"
            2) "13"
            3) "hello"
            */
        }
        freeReplyObject(reply);
    }

    cerr << ">>>>>>>>>>>>>>>>>observer_channel_message quit<<<<<<<<<<<<<<<<<<<<" << endl;
}

// 初始化向业务层上报消息的回调函数
void Redis::init_notify_message_handler(function<void(int, string)> fn)
{
    this->_notify_message_handler = fn;
}
