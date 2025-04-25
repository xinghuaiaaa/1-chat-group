#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H

#include <string>
#include <vector>
using namespace std;

// 提供离线消息表的操作接口方法
class OfflineMessageModel
{
public:
    // 添加离线消息
    void insert(int userid, const string &msg);

    // 删除离线消息
    void remove(int userid);

    // 查询离线消息
    vector<string> query(int userid);
};




#endif