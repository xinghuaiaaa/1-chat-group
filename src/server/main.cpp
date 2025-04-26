#include "chatserver.hpp"
#include <iostream>
using namespace std;

#include <signal.h>
#include "chatservice.hpp"

void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        cerr << "command invalid example ./bin/chatserver"<<endl;
        exit(-1);
    }
    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    signal(SIGINT, resetHandler);
    EventLoop loop;
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();


    return 0;
}
