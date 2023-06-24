#include "../sylar/hook.h"
#include "../sylar/IOManager.h"
#include "../sylar/log.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<string.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_sleep(){
    sylar::IOManager iom(1);
    iom.schedule([](){
        sleep(2);
        SYLAR_LOG_INFO(g_logger) << "sleep 2";
    });
    iom.schedule([](){
        sleep(3);
        SYLAR_LOG_INFO(g_logger) << "sleep 3";
    });
    SYLAR_LOG_INFO(g_logger) << "test sleep";
}

int sock = 0;
void test_sock(){
    sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "36.152.44.96", &addr.sin_addr.s_addr);

    SYLAR_LOG_INFO(g_logger) << "begin connect";
    int rt = connect(sock,(const sockaddr*)&addr, sizeof(addr));
    SYLAR_LOG_INFO(g_logger) << "connect rt = " << rt << " errno = " << errno;

    if (rt) {
        return ;
    }

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    SYLAR_LOG_INFO(g_logger) << "send rt = " << rt << " errno = " << errno;

    if (rt <= 0){
        return ;
    }

    std::string buf;
    buf.resize(4096);

    rt = recv(sock, &buf[0], buf.size(), 0);
    SYLAR_LOG_INFO(g_logger) << "recv rt = " << rt << " errno = " << errno;

    if (rt <= 0){
        return ;
    }

    buf.resize(rt);
    SYLAR_LOG_INFO(g_logger) << buf;

}

int main(int argc, char** argv){

    //test_sleep();
    sylar::IOManager iom;
    iom.schedule(test_sock); 
    return 0;
}