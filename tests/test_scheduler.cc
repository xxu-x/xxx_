#include "../sylar/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_fiber(){
    SYLAR_LOG_INFO(g_logger) << "test in fiber";
    static int s_count = 0;
    sleep(1);
    while (--s_count >= 0){
        sylar::Scheduler::GetThis()->schedule(&test_fiber);
    }
}

int main(int argc, char** argv){
    sylar::Scheduler sc(3, true, "test");
    sc.start(); 
    SYLAR_LOG_INFO(g_logger) << "schedule";
    sc.schedule(&test_fiber);
    sc.stop();
    SYLAR_LOG_INFO(g_logger) << "over";
    return 0;
}