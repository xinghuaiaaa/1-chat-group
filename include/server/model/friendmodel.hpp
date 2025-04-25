#ifndef  ADD_FRIEND_H
#define ADD_FRIEND_H
#include "user.hpp"
#include <vector>

// 维护好友信息的操作接口方法
class FriendModel
{
public:
    // 添加好友
    void insert(int userid, int friendid);

    // 返回好友列表 要显示好友的信息
    // 两个表的 联合查询
    vector<User> query(int userid);

};


#endif