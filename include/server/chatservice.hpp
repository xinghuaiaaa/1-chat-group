// 仅需要一个 实例即可 因此使用单例模式

#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "json.hpp"
using namespace nlohmann;

#include "usermodel.hpp"

#include <mutex>

#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"

//表示处理消息的事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)>; // #1

// 聊天服务器 业务类
class ChatService
{
public:
    // 获取单例对象的接口函数 #6
    static ChatService *instance();

    // 处理的登录业务 #2
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理注册业务 #3
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //获取消息对应的处理器 #7
    MsgHandler getHandler(int msgid);

    // 客户端异常退出处理
    void clientCloseException(const TcpConnectionPtr &conn);

    // 一对一 聊天业务
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 服务器异常退出处理, 重置用户状态函数
    void reset();

    // 处理添加好友业务
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time); // conn用来维护用户与其网络连接之间的映射关系 , 快速找到某个用户的连接

    // 处理创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 处理添加群组业务
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 处理群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

private:
    // 单例模式----构造函数私有化,并写一个惟一的实例
    ChatService(); // #5

    // 存储消息id 和 其对应的 业务处理方法
    unordered_map<int, MsgHandler> _msghandlermap; // #4

    // 注册业务测试
    // 数据操作类对象
    UserModel _usermodel;

    //定义互斥锁, 保证_userConnMap安全
    mutex _connMutex;

    //存储在线用户的 连接信息
    unordered_map<int, TcpConnectionPtr> _userConnMap; // 只有登录成功了, 才会存储

    // 离线消息操作对象
    OfflineMessageModel _offlineMsg;

    // 好友操作对象
    FriendModel _friendModel;

    // 处理群组业务
    GroupModel _groupModel;
    
};

#endif