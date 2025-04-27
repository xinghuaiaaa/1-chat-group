#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <ctime>
using namespace std;

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "json.hpp"
using json = nlohmann::json;

#include "user.hpp"
#include "public.hpp"
#include "group.hpp"

// 记录当前系统登录的用户信息
User g_currentUser;
// 记录当前登录用户的好友列表
vector<User> g_currentUserFriendsList;
// 记录当前登录用户的群组列表
vector<Group> g_currentUserGroupsList;
// 显示当前登录用户的基本信息
void showCurrentUserInfo();

// 接收线程----一共两个线程, 接收和发送, 是并行的   --- main主线程用于发送
void readTaskHandler(int clientfd);
// 获取系统时间(聊天信息添加时间信息)
string getCurrentTime();
// 主聊天页面程序
void mainMenu(int clientfd);
// 控制聊天页面--注销需要退出聊天页面
bool isMainMenuRunning = false;

// 聊天客户端程序实现, main线程用作发送线程, 子线程用作接受线程

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cerr << "command invalid example ./bin/chatserver 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    // 创建socket
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);

    if (clientfd < 0)
    {
        cerr << "socket error" << endl;
        exit(-1);
    }

    // 设置服务器地址结构
    // sockaddr_in serverAddr;
    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in)); // 清空结构体, 确保没有脏数据

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    // 连接服务器
    if (connect(clientfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        cerr << "connect error" << endl;
        close(clientfd);
        exit(-1);
    }

    // main线程用于发送数据
    for (;;)
    {
        // 显示登录首页面 登录, 注册, 退出
        cout << "========================================" << endl;
        cout << "1. login" << endl;
        cout << "2. register" << endl;
        cout << "3. exit" << endl;
        cout << "========================================" << endl;
        cout << "please input your choice: ";
        int choice;
        if (!(std::cin >> choice)) {  // 如果输入不是整数
            std::cin.clear();  // 清除错误状态
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // 丢弃整行
            continue;  // 重新尝试输入
        }
        // cin.get(); // 读出 换行
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // 丢弃剩余字符, 直到遇到换行（如换行符）

        
        switch (choice)
        {

        // # 3
        case 1: // 登录  根据业务, 需要id与密码
        {
            int id = 0;
            char password[50] = {0};
            cout << "please input your id: ";
            cin >> id;
            cin.get(); // 清空输入缓冲区
            cout << "please input your password: ";
            cin.getline(password, 50); // 读取一行, 包括空格 cin和scanf不能读空格

            // 组装json数据
            json js;
            js["msgid"] = LOGIN_MSG; // 登录消息
            js["id"] = id;
            js["password"] = password;
            // 发送登录请求
            string request = js.dump();                                                // json转字符串  序列化
            int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0); // 发送数据
            if (len < 0)
            {
                cerr << "send login msg error: " << request << endl;
                cerr << "connect error" << endl;
            }
            else
            {
                char buffer[1024] = {0};               // 接收服务器返回的数据
                len = recv(clientfd, buffer, 1024, 0); // 接收数据

                if (len < 0 || len == 0)
                {
                    if (len == 0)
                    {
                        cout << "server quit!" << endl;
                        return 0;
                    }
                    else
                    {
                        cerr << "recv error" << endl;
                    }
                }
                else
                {
                    // 解析json数据
                    json response = json::parse(buffer); // 反序列化 字符串转json
                    string tmp = response.dump();        // json转字符串  序列化
                    if (response["errno"] == 0)
                    { // 根据业务代码处理 1.登录成功返回 2.好友列表 3.群组列表 4.离线消息
                        cout << "login success" << endl;
                        // 客户端记录登录用户信息
                        g_currentUser.setId(response["id"]);
                        g_currentUser.setName(response["name"]);

                        // if(response["friends"].is_null())
                        // 处理好友列表
                        // if (response["friends"].contains("friends")) // 判断是否包含字段, 跟好点,  而不是看 是不是空
                        if (response.contains("friends"))
                        {
                            vector<string> friends = response["friends"]; // 类型是vector<string>, 不是vector<User>,  根据服务器业务,存的是js.dump() 字符串
                            g_currentUserFriendsList.clear();
                            for (auto &friendUser : friends)
                            {
                                json js = json::parse(friendUser); // 反序列化
                                User user;
                                user.setId(js["id"]);
                                user.setName(js["name"]);
                                user.setState(js["state"]);
                                g_currentUserFriendsList.push_back(user);
                            }
                            for (auto &friendUser : g_currentUserFriendsList)
                            {
                                cout << "friendid: " << friendUser.getId() << " name: " << friendUser.getName() << " state: " << friendUser.getState() << endl;
                            }
                        }
                        else
                        {
                            cout << "friends list is empty" << endl;
                        }

                        // 处理群组列表
                        if (response.contains("groups")) // 判断是否包含字段, 跟好点,  而不是看 是不是空
                        {
                            vector<string> groups = response["groups"]; // 类型是vector<string>, 不是vector<User>,  根据服务器业务,存的是js.dump() 字符串
                            g_currentUserGroupsList.clear();
                            for (auto &groupl : groups)
                            {
                                json js = json::parse(groupl); // 反序列化
                                string tmp = js.dump();
                                Group group;
                                group.setId(js["id"]);
                                group.setName(js["groupname"]);
                                group.setDesc(js["groupdesc"]);

                                // 处理群组成员列表
                                vector<string> users = js["users"];
                                for (auto &userl : users)
                                {
                                    json js = json::parse(userl); // 反序列化
                                    GroupUser user;
                                    user.setId(js["id"]);
                                    user.setName(js["name"]);
                                    user.setState(js["state"]);
                                    user.setRole(js["role"]);
                                    group.getUsers().push_back(user);
                                }
                                g_currentUserGroupsList.push_back(group);
                            }
                            for (auto &group : g_currentUserGroupsList)
                            {
                                cout << "groupid: " << group.getId() << " name: " << group.getName() << " desc: " << group.getDesc() << endl;
                                for (auto &groupUser : group.getUsers())
                                {
                                    cout << "group user id: " << groupUser.getId() << " name: " << groupUser.getName() << " state: " << groupUser.getState() << " role: " << groupUser.getRole() << endl;
                                }
                            }
                        }
                        else
                        {
                            cout << "groups list is empty" << endl;
                        }

                        // 显示当前登录用户的基本信息---包含好友列表和群组列表
                        showCurrentUserInfo();

                        // 处理离线消息
                        if (response.contains("offlinemsg")) // 判断是否包含字段, 跟好点,  而不是看 是不是空
                        {
                            vector<string> offlinemsg = response["offlinemsg"]; // 类型是vector<string>, 不是vector<User>,  根据服务器业务,存的是js.dump() 字符串
                            for (auto &msg : offlinemsg)
                            {
                                json js = json::parse(msg); // 反序列化
                                // 时间+fromid+fromname+msg-----详看笔记 一对一聊天发送的格式

                                // 分一下 个人离线和群组离线
                                if (js["msgid"] == ONE_CHAT_MSG)
                                {
                                    cout << js["time"].get<string>() << "[" << js["id"].get<int>() << "] " << js["name"].get<string>() << " said: " << js["msg"].get<string>() << endl;
                                }
                                if (js["msgid"] == GROUP_CHAT_MSG) // 群组聊天消息
                                {
                                    cout << "群消息-->[" << js["groupid"] << "] " << js["time"].get<string>() << "[" << js["id"] << "] " << js["name"].get<string>() << " said: " << js["msg"].get<string>() << endl;
                                }
                            }
                        }
                        else
                        {
                            cout << "offlinemsg list is empty" << endl;
                        }

                        // 登录成功, 启动接收线程----只要客户端 不完全退出, 就只启动一次!
                        static int threadnum = 0;
                        if (threadnum == 0)
                        {
                            std::thread readTask(readTaskHandler, clientfd); // thread 支持跨平台
                            readTask.detach();
                        }

                        // 分离线程, 让其独立运行, 不阻塞主线程

                        // 主线程继续执行, 进入聊天菜单页面
                        isMainMenuRunning = true;
                        mainMenu(clientfd);
                    }
                    // else if(response["errno"] == 1)
                    // {
                    //     // 用户不存在
                    //     cout << "login failed, error: " << response["errmsg"] << endl;
                    // }
                    // else if(response["errno"] == 2)
                    // {
                    //     // 重复登录
                    //     cout << "login failed, error: " << response["errmsg"] << endl;
                    // }
                    // else if(response["errno"] == 3)
                    // {
                    //     // 密码错误
                    //     cout << "login failed, error: " << response["errmsg"] << endl;
                    // }
                    else // 不分那么细, 服务器已经确定错误信息了
                    {
                        // 登录失败
                        cout << "login failed, error: " << response["errmsg"] << endl;
                        break;
                    }
                }
            }
        }
        break;
        // # 2
        case 2: // 注册
        {
            char name[50]; // 比string更好, 因为string会有内存分配, 还可以限制长度
            char password[50];
            cout << "please input your name: ";
            cin.getline(name, 50); // 读取一行, 包括空格 cin和scanf不能读空格
            cout << "please input your password: ";
            cin.getline(password, 50);

            // 组装json数据
            json js;
            js["msgid"] = REG_MSG; // 注册消息
            js["name"] = name;
            js["password"] = password;

            // 发送注册请求
            string request = js.dump(); // json转字符串  序列化

            // int len = send(clientfd, request.c_str(), request.size(), 0); // 发送数据
            // 第二个参数必须这么写, 因为规定是const void*类型, 不能直接传入string类型
            int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0); // 发送数据 这样的+1, 是加了 \0, strlen不算这个
            if (len < 0)
            {
                cerr << "send error" << request << endl;
            }
            else
            {
                char buffer[1024] = {0};               // 接收服务器返回的数据
                len = recv(clientfd, buffer, 1024, 0); // 接收数据
                if (len < 0)
                {
                    cerr << "recv error" << endl;
                }
                else
                {
                    // 解析json数据
                    json response = json::parse(buffer); // 反序列化 字符串转json
                    if (response["errno"] == 0)
                    { // 根据业务代码处理
                        cout << "register success, userid: " << response["id"] << " do not forget it!" << endl;
                    }
                    else
                    {
                        // 注册失败
                        cout << "register failed, error: " << name << " is already exit!" << endl;
                    }
                }
            }
        }
        break;
        // # 1
        case 3: // 这个最简单 quit业务
        {
            cout << "exit system" << endl;
            close(clientfd);
            exit(0);
        }
        default:
        {
            cout << "input error" << endl;
        }
        break;
        }
    }
}

void showCurrentUserInfo()
{
    cout << "====================login user====================" << endl;
    cout << "current login user => id: " << g_currentUser.getId() << " name: " << g_currentUser.getName() << endl;
    if (!g_currentUserFriendsList.empty())
    {
        cout << "--------------------friends list--------------------" << endl;
        for (auto &friendUser : g_currentUserFriendsList)
        {
            cout << "id: " << friendUser.getId() << " name: " << friendUser.getName() << " state: " << friendUser.getState() << endl;
        }
    }
    else
    {
        cout << "current login user friends list is empty" << endl;
    }
    cout << "--------------------group list--------------------" << endl;
    if (!g_currentUserGroupsList.empty())
    {
        for (auto &group : g_currentUserGroupsList)
        {
            cout << "id: " << group.getId() << " name: " << group.getName() << " desc: " << group.getDesc() << endl;
            for (auto &groupUser : group.getUsers())
            {
                cout << "id: " << groupUser.getId() << " name: " << groupUser.getName() << " state: " << groupUser.getState() << " role: " << groupUser.getRole() << endl;
            }
        }
    }
    else
    {
        cout << "current login user group list is empty" << endl;
    }
    cout << "=====================================================" << endl;
}

// 接收线程---实时就收 服务器返回的数据--包括别人发来的聊天消息
void readTaskHandler(int clientfd)
{
    for (;;)
    {
        char buffer[1024] = {0};                   // 接收服务器返回的数据
        int len = recv(clientfd, buffer, 1024, 0); // 接收数据
        if (len < 0)                               // ==-1
        {
            cerr << "recv error" << endl;
            close(clientfd);
            exit(-1);
        }
        else if (len == 0) // 服务器关闭连接
        {
            cout << "server close" << endl;
            close(clientfd);
            exit(-1);
        }

        // 解析json数据
        json response = json::parse(buffer);   // 反序列化 字符串转json
        if (response["msgid"] == ONE_CHAT_MSG) // 一对一聊天消息
        {
            cout << response["time"].get<string>() << "[" << response["id"] << "] " << response["name"].get<string>() << " said: " << response["msg"].get<string>() << endl;
            continue;
        }

        if (response["msgid"] == GROUP_CHAT_MSG) // 群组聊天消息
        {
            cout << "群消息-->[" << response["groupid"] << "] " << response["time"].get<string>() << "[" << response["id"] << "] " << response["name"].get<string>() << " said: " << response["msg"].get<string>() << endl;
        }
    }
}

// handler合集
// 显示帮助信息
void help(int clientfd = 0, string msg = "");

// 一对一聊天
void chat(int clientfd, string msg);

// 添加好友
void addfriend(int clientfd, string msg);

// 创建群组
void creategroup(int clientfd, string msg);

// 添加群组
void addgroup(int clientfd, string msg);

// 群组聊天
void groupchat(int clientfd, string msg);
// 退出系统
void quit(int clientfd, string msg);

// 系统支持的客户端命令列表
unordered_map<string, string> commandList = {
    {"help", "显示所有支持的命令, 格式help"},
    {"chat", "一对一聊天, 格式chat:friend:msg"},
    {"addfriend", "添加好友, 格式addfriend:friendid"},
    {"creategroup", "创建群组, 格式creategroup:groupname:groupdesc"},
    {"addgroup", "添加群组, 格式addgroup:groupid"},
    {"groupchat", "群组聊天, 格式groupchat:groupid:msg"},
    {"loginout/quit", "退出系统/注销, 格式quit"}};

// 注册系统支持的客户端命令处理
unordered_map<string, function<void(int, string)>> commandHandlerMap = {
    {"help", help},
    {"chat", chat},               // 一对一聊天
    {"addfriend", addfriend},     // 添加朋友
    {"creategroup", creategroup}, // 创建群组
    {"addgroup", addgroup},       // 添加群组
    {"groupchat", groupchat},     // 群组聊天
    {"quit", quit}

};

// 主页面聊天程序
void mainMenu(int clientfd)
{
    help();
    // for (;;)
    while (isMainMenuRunning)
    {
        // 截取输入的 格式
        char buffer[1024] = {0};        // 用户输入的命令
        cin.getline(buffer, 1024);      // 读取一行, 包括空格 cin和scanf不能读空格
        string commandbuf(buffer);      // 转成string类型
        string command;                 // 存储实际命令
        int idx = commandbuf.find(":"); // 查找第一个:的位置

        if (idx == string::npos) // 没有找到  ==-1->不建议用
        {
            command = commandbuf; // 直接赋值
        }
        else
        {
            command = commandbuf.substr(0, idx); // 截取业务命令---"chat"
        }

        auto it = commandHandlerMap.find(command); // 查找命令
        if (it != commandHandlerMap.end())         // 找到命令
        {
            // 调用相应命令的事件处理函数, mainMenu 对修改封闭, 添加功能不需要修改函数
            it->second(clientfd, commandbuf.substr(idx + 1, commandbuf.size() - idx - 1)); // 调用对应的处理函数  出入剩下的字符串
            // 调用对应的处理函数  出入剩下的字符串
        }
        else
        {
            cout << "command invalid" << endl;
        }
    }
}

// help函数
void help(int clientfd, string msg)
{
    cout << "====================command list====================" << endl;
    for (auto &command : commandList)
    {
        cout << command.first << " : " << command.second << endl;
    }
    cout << "=====================================================" << endl;
}

// addfriend函数
void addfriend(int clientfd, string msg)
{
    int friendid = atoi(msg.c_str()); // 转成整型
    json js;
    js["msgid"] = ADD_FRIEND_MSG;     // 添加好友消息
    js["id"] = g_currentUser.getId(); // 当前登录用户id
    js["friendid"] = friendid;

    // 发送添加好友请求
    string request = js.dump();                                                // json转字符串  序列化
    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0); // 发送数据
    if (len < 0)
    {
        cerr << "send friendid is error  ===>  " << request << endl;
    }
}

// chat函数
void chat(int clientfd, string msg)
{
    int idx = msg.find(":"); // 查找第一个:的位置
    if (idx == string::npos) // 没有找到  ==-1->不建议用
    {
        cout << "chat command: friend id is invalid!" << endl;
        return;
    }
    int friendid = atoi(msg.substr(0, idx).c_str());            // 截取好友id
    string message = msg.substr(idx + 1, msg.size() - idx - 1); // 截取聊天信息

    json js;
    js["msgid"] = ONE_CHAT_MSG;           // 一对一聊天消息
    js["id"] = g_currentUser.getId();     // 当前登录用户id
    js["name"] = g_currentUser.getName(); // 当前登录用户姓名
    js["to"] = friendid;                  // 好友id  -- 字段要对应服务器那边的
    js["msg"] = message;                  // 聊天信息
    js["time"] = getCurrentTime();        // 时间

    // 发送聊天请求
    string request = js.dump();                                                // json转字符串  序列化
    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0); // 发送数据
    if (len < 0)
    {
        cerr << "send chat msg error: " << request << endl;
    }
}

// creategroup函数
void creategroup(int clientfd, string msg)
{
    int idx = msg.find(":"); // 查找第一个:的位置
    if (idx == string::npos) // 没有找到  ==-1->不建议用
    {
        cout << "creategroup command: group name is invalid!" << endl;
        return;
    }
    string groupname = msg.substr(0, idx);                        // 截取群组名称
    string groupdesc = msg.substr(idx + 1, msg.size() - idx - 1); // 截取群组描述

    json js;
    js["msgid"] = CREATE_GROUP_MSG;   // 创建群组消息
    js["id"] = g_currentUser.getId(); // 当前登录用户id
    js["groupname"] = groupname;      // 群组名称
    js["groupdesc"] = groupdesc;      // 群组描述

    string request = js.dump();                                                // json转字符串  序列化
    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0); // 发送数据
    if (len < 0)
    {
        cerr << "send creategroup msg error: " << request << endl;
    }
}

// addgroup函数
void addgroup(int clientfd, string msg)
{
    int groupid = atoi(msg.c_str()); // 转成整型

    json js;
    js["msgid"] = ADD_GROUP_MSG;      // 添加群组消息
    js["id"] = g_currentUser.getId(); // 当前登录用户id
    js["groupid"] = groupid;          // 群组id

    string request = js.dump();                                                // json转字符串  序列化
    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0); // 发送数据
    if (len < 0)
    {
        cerr << "send addgroup msg error: " << request << endl;
    }
}

// groupchat函数
void groupchat(int clientfd, string msg)
{
    int idx = msg.find(":"); // 查找第一个:的位置
    if (idx == string::npos) // 没有找到  ==-1->不建议用
    {
        cout << "groupchat command: group id is invalid!" << endl;
        return;
    }
    int groupid = atoi(msg.substr(0, idx).c_str());             // 截取群组id
    string message = msg.substr(idx + 1, msg.size() - idx - 1); // 截取聊天信息

    json js;
    js["msgid"] = GROUP_CHAT_MSG;         // 群组聊天消息
    js["id"] = g_currentUser.getId();     // 当前登录用户id
    js["name"] = g_currentUser.getName(); // 当前登录用户姓名
    js["groupid"] = groupid;              // 群组id  -- 字段要对应服务器那边的
    js["msg"] = message;                  // 聊天信息
    js["time"] = getCurrentTime();        // 时间

    // 发送聊天请求
    string request = js.dump();                                                // json转字符串  序列化
    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0); // 发送数据
    if (len < 0)
    {
        cerr << "send groupchat msg error: " << request << endl;
    }
}

// quit函数
void quit(int clientfd, string msg)
{

    json js;
    js["msgid"] = LOGINOUT_MSG;       // 注销消息
    js["id"] = g_currentUser.getId(); // 当前登录用户id

    string request = js.dump();                                                // json转字符串  序列化
    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0); // 发送数据
    if (len < 0)
    {
        cerr << "send quit msg error: " << request << endl;
    }
    isMainMenuRunning = false; // 退出聊天页面
    // close(clientfd);  放在服务端处理
    // exit(0);
}

// 时间函数
string getCurrentTime()
{
    // 获取当前时间
    time_t now = time(0);                                       // 获取当前时间
    tm *ltm = localtime(&now);                                  // 转换为本地时间
    char buffer[80];                                            // 存储时间字符串
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm); // 格式化时间
    string timeStr(buffer);                                     // 转成string类型
    // cout << "current time: " << timeStr << endl; // 测试用
    return timeStr; // 返回时间字符串
    // return string(); //
    // return " ";
}
