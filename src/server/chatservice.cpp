#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
using namespace muduo;

#include <vector>
#include <iostream>
using namespace std;

// 获取单例对象的 函数接口
ChatService *ChatService::instance()
{
    static ChatService service; // 线程安全 懒汉式单例
    return &service;
}

// 注册消息以及对应的 handler 回调
ChatService::ChatService()
{
    // 网络模块和业务模块 解耦的核心

    _msghandlermap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});

    _msghandlermap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});

    _msghandlermap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});

    _msghandlermap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});

    _msghandlermap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});

    _msghandlermap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});

    _msghandlermap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
}

// 获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid)
{
    // 记录错误日志, msgid 没有对应的事件回调
    auto it = _msghandlermap.find(msgid);
    if (it == _msghandlermap.end())
    {
        // 使用 muduo 自带的日志系统
        // 不要endl, 已经封装了
        // LOG_ERROR<<"msgid: "<<msgid<<"can not find Handler!";

        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
        {
            LOG_ERROR << "msgid: " << msgid << " can not find Handler!";
        };
    }

    else
    {
        return _msghandlermap[msgid];
    }
}

// 处理登录业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // LOG_INFO<<"do login service"; //测试用
    // int id = js["id"];
    int id = js["id"].get<int>(); // js字符串 转成整型
    string pwd = js["password"];

    User user = _usermodel.query(id);
    if (user.getId() == id && user.getPwd() == pwd) // id默认为-1
    {
        // 登陆成功
        if (user.getState() == "online") // 用户在线, 不允许重复登录
        {
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "this count is using, input another!----用户已经登录, 不允许重复登录!";
            conn->send(response.dump());
        }
        else
        {
            // 存储用户登录信息
            // _userConnMap.insert({id, conn});
            {
                lock_guard<mutex> lock(_connMutex); // 自动解锁
                _userConnMap.emplace(id, conn);
            }

            user.setState("online");      // 登录成功, 更新状态
            _usermodel.updateState(user); // 刷新状态
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            // 查询好友列表并返回
            vector<User> uservec = _friendModel.query(id);
            if (!uservec.empty())
            {
                // response["friends"] = uservec; // 这是不行的, 因为是自定义类型
                // map也不行, 因为map的value 不确定
                vector<string> vec;
                for (auto &user : uservec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec.push_back(js.dump());
                }
                response["friends"] = vec; // 好友列表
            }

            // 查询群组列表
            vector<Group> groupVec = _groupModel.queryGroups(id);
            if (!groupVec.empty())
            {
                vector<string> vec;
                for (auto &groupl : groupVec)
                {
                    json js ;
                    js["id"] = groupl.getId();
                    js["groupname"] = groupl.getName();
                    js["groupdesc"] = groupl.getDesc();
                    
                    vector<string> usersvec;
                    for (auto &user : groupl.getUsers())
                    {
                        json jsu;
                        jsu["id"]=user.getId();
                        jsu["name"]=user.getName();
                        jsu["state"]=user.getState();
                        jsu["role"] = user.getRole();
                        usersvec.push_back(jsu.dump());
                    }
                    js["users"] = usersvec;
                    vec.push_back(js.dump());
                }
                response["groups"] = vec; // 群组列表
            }

            // 查询离线消息
            vector<string> vec = _offlineMsg.query(id);
            if (!vec.empty())
            {
                response["offlinemsg"] = vec; // 离线消息  js可以直接序列化容器
                // 离线消息发送完毕, 删除离线消息
                _offlineMsg.remove(id);
                // cout<<"离线消息删除成功!"<<endl;
            }

            conn->send(response.dump());
        }
    }
    else
    {
        // 登录失败
        if (user.getId() == -1) // 没找到
        {
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 1;
            response["errmsg"] = "id or account is invalid!-----用户名不存在!";
            conn->send(response.dump());
        }
        else if (user.getPwd() != pwd)
        {
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 3;
            response["errmsg"] = "password is error!-----密码错误!";
            conn->send(response.dump());
        }
    }
}

// 处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // LOG_INFO<<"do reg service"; ////测试用

    // 注册仅需要 name password
    string name = js["name"];
    string password = js["password"];

    User user;
    user.setName(name);
    user.setPwd(password);
    bool state = _usermodel.insert(user);
    if (state)
    {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();

        conn->send(response.dump());
    }
    else
    {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

// 客户端异常退出处理

void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    if (user.getId() != -1)
    {
        // 更新用户状态信息
        user.setState("offline");
        _usermodel.updateState(user);
    }
}

// 一对一 聊天业务  并进行处理器绑定
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["to"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // 在线, 转发消息
            it->second->send(js.dump());
            return;
        }
    }

    // 不在线, 存储离线消息
    _offlineMsg.insert(toid, js.dump());
}

// 服务器异常退出处理, 重置用户状态函数
void ChatService::reset()
{
    // 更新所有用户的状态----把在线用户都设置为离线
    _usermodel.resetState();
}

// 处理添加好友业务 带msgid
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    // 添加好友   显示好友信息 在登陆成功那里
    _friendModel.insert(id, friendid);
}

// 处理创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int uerid = js["id"].get<int>(); // 这是 哪个用户要创建群组,  不是群组id
    string groupname = js["groupname"];
    string groupdesc = js["groupdesc"];

    // 存储新创建的群组信息-----此时还未添加到 数据库, 群id还未知
    Group group(-1, groupname, groupdesc);

    if (_groupModel.createGroup(group))
    {
        // 创建群后, 存储群组创建人 信息
        _groupModel.addGroup(group.getId(), uerid, "creator");
        // 服务器响应 可以自行添加
    }
}

// 处理添加群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();

    // 添加群组
    _groupModel.addGroup(groupid, userid, "normal");
}

// 处理群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time) // 3个worker线程 在完成这个函数时, 可能会同时访问 _userConnMap
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> userVec = _groupModel.queryGroupUsers(userid, groupid); // 群组用户id列表

    // 群组聊天, 需要将消息转发给群组中的所有用户
    lock_guard<mutex> lock(_connMutex);
    for (int id : userVec)
    {
        // 用户在线, 就直接转发
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            // 在线, 转发消息
            it->second->send(js.dump());
        }
        else
        {
            // 不在线, 存储离线消息
            _offlineMsg.insert(id, js.dump());
        }
    }
}
