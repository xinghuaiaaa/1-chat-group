#include "groupmodel.hpp"
#include <db.h>


bool GroupModel::createGroup(Group &group) // 创建群组
{   
    // sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname, groupdesc) values('%s', '%s')",
            group.getName().c_str(), group.getDesc().c_str());

    // 连接数据库
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 获取插入的id
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

// 加入群组
bool GroupModel::addGroup(int groupid, int userid, string role)
{
    // sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser(groupid, userid, role) values(%d, %d, '%s')",
            groupid, userid, role.c_str());

    // 连接数据库
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

// 查询用户所在群组---联合查询, 直接取出群组的 全部信息
// 根据用户id查询群组id, 再根据群组id查询群组信息
vector<Group> GroupModel::queryGroups(int userid)
{
    // 1.先查询用户所在的所有群组的 群组信息
    // sql语句
    char sql[1024] = {0};
    sprintf(sql, "select a.id, a.groupname, a.groupdesc from allgroup a inner join groupuser b on a.id = b.groupid where b.userid = %d", userid);

    // 连接数据库
    MySQL mysql;
    vector<Group> groupVec; // 存储群组信息以及群组用户信息
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            
            while (MYSQL_ROW row = mysql_fetch_row(res))
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
            }
            mysql_free_result(res);
            return groupVec;
        }
    }

    // 2.查询每个群组的其他用户信息---群组用户id列表
    for(auto &group : groupVec)  // 注意这里是引用, 不能用auto group : groupVec
    {
        sprintf(sql, "select a.id,a.name, a.state,b.role from user a inner join groupuser b on a.id = b.userid where b.groupid = %d", group.getId());
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            while (MYSQL_ROW row = mysql_fetch_row(res))
            {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]); // 群组角色
                // 将用户添加到群组对象中
                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }

    return vector<Group>();

}

// 根据指定群组id查询群组用户id列表, 除了自己
// 群聊转发业务!!!, 通过群组id查询群组用户id列表
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    // sql语句
    char sql[1024] = {0};
    // 经过上面的查询用户所在群组的函数, 每个群组的用户id都已经存储在了数据库中
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d", groupid, userid);

    // 连接数据库
    MySQL mysql;
    vector<int> userVec; // 存储群组用户id列表
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            while (MYSQL_ROW row = mysql_fetch_row(res))
            {
                userVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
            return userVec;
        }
    }
    return vector<int>();
}