#include "friendmodel.hpp"
#include "db.h"

// 添加好友
void FriendModel::insert(int userid, int friendid)
{
    // 1. 创建sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into friend (userid, friendid) values (%d, %d)", userid, friendid);

    // 2. 执行sql语句
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 返回好友列表 要显示好友的信息
// 两个表的 联合查询
vector<User> FriendModel::query(int userid)
{
    // 1. 创建sql语句
    char sql[1024] = {0};
    sprintf(sql, "select a.id, a.name, a.state from user a inner join friend b on b.friendid = a.id where b.userid = %d", userid); 

    // 2. 执行sql语句
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            // 3. 解析结果
            MYSQL_ROW row;
            vector<User> vec;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vector<User>();  //比vec好点
}