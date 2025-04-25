#ifndef PUBLIC_H
#define PUBLIC_H
/*
server 和 client 的公共文件
*/

enum EnMsgType  // 枚举
{
    LOGIN_MSG=1,   // 与业务的login函数连接
    LOGIN_MSG_ACK,
    REG_MSG,  //注册消息
    REG_MSG_ACK, // 注册响应消息
    ONE_CHAT_MSG, //一对一聊天 
    ADD_FRIEND_MSG, // 添加好友

    CREATE_GROUP_MSG, // 创建群组
    ADD_GROUP_MSG, // 添加群组
    GROUP_CHAT_MSG, // 群聊
};


#endif