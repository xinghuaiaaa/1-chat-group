#ifndef GROUPUSER_H
#define GROUPUSER_H

#include <string>
using namespace std;
#include "user.hpp"

// 群组用户, 多了一个角色属性, 从User类继承
class GroupUser: public User
{
public:
    void setRole(string role)
    {
        this->role = role;
    }
    string getRole()
    {
        return this->role;
    }

private:
    string role; // 群组角色
    
};

#endif