// 映射类
// user的 ORM类, 用于映射字段

#ifndef USER_H
#define USER_H
#include <string>
using namespace std;



class User
{
public:
    User(int id=-1, string name="", string password = "", string state="offline")
    {
        this->id = id;
        this->name = name;
        this->password=password;
        this->state=state;
    }

    // 修改接口
    void setId(int id) {this->id = id;}
    void setName(string name) {this->name = name;}
    void setPwd(string pwd) {this->password = pwd;}
    void setState(string state) {this->state = state;}

    // 获取
    int getId() {return this->id ;}
    string getName() {return this->name;}
    string getPwd() {return this->password;}
    string getState() {return this->state;}


protected:
    int id;
    string name;
    string password;
    string state;
};

#endif