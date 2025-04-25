// group的ORM类

#ifndef GROUP_HPP
#define GROUP_HPP

#include <string>
#include <vector>
using namespace std;
#include "groupuser.hpp"

class Group
{
    public:
    // 群组的构造函数
    Group(int id=-1, string name="", string desc="")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }

    void setId(int id)
    {
        this->id = id;
    }
    void setName(string name)
    {
        this->name = name;
    }
    void setDesc(string desc)
    {
        this->desc = desc;
    }
    int getId()
    {
        return this->id;
    }
    string getName()
    {
        return this->name;
    }
    string getDesc()
    {
        return this->desc;
    }
    // 群组的成员列表
    vector<GroupUser> &getUsers()
    {
        return this->users;
    }


private:
    int id; // 群组id
    string name; // 群组名称
    string desc; // 群组描述
    vector<GroupUser> users; // 群组成员id列表

};



#endif