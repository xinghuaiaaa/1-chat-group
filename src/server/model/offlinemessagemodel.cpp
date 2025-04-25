#include "offlinemessagemodel.hpp"
#include "db.h"
#include <iostream>

// 添加离线消息
void OfflineMessageModel::insert(int userid, const string &msg)
{
    // 1. 创建数据库连接
    // 2. 执行 SQL 语句插入离线消息
    // 3. 关闭数据库连接
    // 4. 返回插入结果
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage (userid, message) values (%d, '%s')", userid, msg.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }

}

// 删除离线消息
void OfflineMessageModel::remove(int userid)
{
    // 1. 创建数据库连接
    // 2. 执行 SQL 语句删除离线消息
    // 3. 关闭数据库连接
    // 4. 返回删除结果
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid = %d", userid);

    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }

}

// 查询离线消息
vector<string> OfflineMessageModel::query(int userid)
{
    // 1. 创建数据库连接
    // 2. 执行 SQL 语句查询离线消息
    // 3. 关闭数据库连接
    // 4. 返回查询结果
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d", userid);

    vector<string> vec;
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);   // 数据库api
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.push_back(row[0]); // 将查询到的消息添加到结果向量中
            }
            mysql_free_result(res); // 释放一下资源, 否则内存不断泄露
        }
        return vec;
    }

    return vec;
}