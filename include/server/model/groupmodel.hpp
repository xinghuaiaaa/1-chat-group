#ifndef GROUPMODEL_HPP
#define GROUPMODEL_HPP

#include "group.hpp"

class GroupModel
{
    public:
    bool createGroup(Group &group); // 创建群组

    // 加入群组
    bool addGroup(int groupid, int userid, string role);

    // 查询用户所在群组
    vector<Group> queryGroups(int userid);

    // 根据指定群组id查询群组用户id列表, 除了自己, 主要用户群聊业务
    vector<int> queryGroupUsers(int userid, int groupid);
};
#endif