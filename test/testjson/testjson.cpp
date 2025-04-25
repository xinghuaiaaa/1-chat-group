#include "json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <vector>
#include <map>
#include <string>
using namespace std;

// json序列化示例1
string func1()
{
    json js;
    js["msg_type"] = 2;
    js["from"] = "zhangsan";
    js["to"] = "li si";
    js["msg"] = "hello, waht are you doing?";

    // cout << js << endl;

    // // 转字符串输出
    // string sendbuf = js.dump();
    // cout << sendbuf.c_str() << endl; // 网络传送一般都是 char*, string 转一下
    return js.dump();
}

string func2()
{
    json js;
    // 添加数组
    js["id"] = {1, 2, 3, 4, 5};
    // 添加key-value
    js["name"] = "zhang san";
    // 添加对象
    js["msg"]["zhang san"] = "hello world";
    js["msg"]["liu shuo"] = "hello china";
    // 上面等同于下面这句一次性添加数组对象
    js["msg"] = {{"zhang san", "hello world"}, {"liu shuo", "hello china"}};
    // cout << js << endl;
    return js.dump();
}

string func3()
{
    json js;
    // 直接序列化一个vector容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);
    js["list"] = vec;

    // 直接序列化一个map容器
    map<int, string> m;
    m.insert({1, "黄山"});
    m.insert({2, "华山"});
    m.insert({3, "泰山"});
    js["path"] = m;

    // cout << js << endl;
    
    // string sendbuf = js.dump();
    // cout<<sendbuf.c_str()<<endl;
    return js.dump();
}

int main()
{
    // func1();
    // func2();
    // func3();

    // string recvBuf = func1();
    // json jsbuf = json::parse(recvBuf);
    // cout<<jsbuf["from"]<<endl; 
    // cout<<jsbuf["msg_type"]<<endl; 
    // cout<<jsbuf["to"]<<endl; 

    // //使用 auto 不关注 返回类型, 并且可以存储
    // string recvBuf = func2();
    // json jsbuf = json::parse(recvBuf);
    // auto arr = jsbuf["id"];
    // cout<<arr<<endl;   // [1,2,3,4,5]

    string recvBuf = func3();
    json jsbuf = json::parse(recvBuf);
    vector<int> vec = jsbuf["list"];
    for(int &v:vec)
    {
        cout<<v<<" ";
    }
    cout<<endl;

    map<int, string> mymap = jsbuf["path"];
    for(auto &p:mymap)
    {
        cout<<p.first<<" "<<p.second<<" ";
    }
    cout<<endl;
    

    return 0;
}
