#ifndef DB_H
#define DB_H

#include <mysql/mysql.h> //只有安装了 库, 才有这个头文件
#include <string>
using namespace std;




// 数据库操作类
class MySQL
{
public:
    // 初始化数据库连接
    MySQL();
   
    ~MySQL();
    // 连接数据库
    bool connect();
    
    // 更新操作
    bool update(string sql);
    
    // 查询操作
    MYSQL_RES *query(string sql);
    
    // 获取mysql 连接, 测试用
    MYSQL* getConnection();

private:
    MYSQL *_conn;
};

#endif