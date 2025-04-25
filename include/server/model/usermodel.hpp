// 实际操作 表 的 类----增删改查

#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

//User 表的 数据操作类
class UserModel
{
    public:
    //user表的 增加方法
    bool insert(User &user);

    //根据用户号码 查询用户信息
    User query(int id);  

    // 更新 用户状态信息
    bool updateState(User user);

    // 重置用户状态
    void resetState();
};

#endif