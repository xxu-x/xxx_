#include "../sylar/sylar.h"
#include "../sylar/iomanager.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

int sock = 0;
void test_fiber(){
    SYLAR_LOG_INFO(g_logger) << "test fiber";

    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "36.152.44.96", &addr.sin_addr.s_addr);

    // connect(sock, (const sockaddr*)&addr, sizeof(addr));
    // iom.addEvent(sock, sylar::IOManager::Event::READ, [](){
    //     SYLAR_LOG_INFO(g_logger) << "connect";
    // });
    // iom.addEvent(sock, sylar::IOManager::Event::WRITE, [](){
    //     SYLAR_LOG_INFO(g_logger) << "connect";
    // });

    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
    } else if(errno == EINPROGRESS) {
        SYLAR_LOG_INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::READ, [](){
            SYLAR_LOG_INFO(g_logger) << "read callback";
        });
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::WRITE, [](){
            SYLAR_LOG_INFO(g_logger) << "write callback";
           //close(sock);
            //sylar::IOManager::GetThis()->cancelEvent(sock, sylar::IOManager::READ);
            //close(sock);
        });
    } else {
        SYLAR_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }
}
void test1(){
    sylar::IOManager iom;
    iom.schedule(&test_fiber);

}

void test_timer(){
    sylar::IOManager iom(2);
    sylar::Timer::ptr  timer = iom.addTimer(1000, [&timer](){
        SYLAR_LOG_INFO(g_logger) << "hello timer";
        static int i = 0;
        if (++i == 5){
            timer->cancel();
        }
    }, true);
}

int main(int argc, char** argv){

    //test1();

    test_timer();
    return 0;
}