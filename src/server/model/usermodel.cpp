#include "usermodel.hpp"
#include "db.h"

#include <iostream>
using namespace std;

//user表的 增加方法
bool UserModel::insert(User &user)
{
    //1. 组装 sql语句
    char sql[1024]={0};
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s')", user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());  // 注意 %s, 每个要单独''起来, 整个''是连起来的一个字符串

    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            //获取插入成功的 用户数据生成的 主键id
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

User UserModel::query(int id)
{
    //1. 组装 sql语句
    char sql[1024]={0};
    sprintf(sql, "select * from user where id=%d", id);

    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);   // 数据库api
        if(res!=nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);

                mysql_free_result(res); // 释放一下资源, 否则内存不断泄露
                return user;
            }
        }
    }
    return User(); // 返回一个默认构造的对象, id=-1

}

// 更新 用户状态信息
bool UserModel::updateState(User user)
{
    //1. 组装 sql语句
    char sql[1024]={0};
    sprintf(sql, "update user set state='%s' where id=%d",user.getState().c_str(), user.getId());

    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

// 重置用户状态
void UserModel::resetState()
{
    //1. 组装 sql语句
    char sql[1024]={0};
    sprintf(sql, "update user set state='offline' where state='online'");

    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}