#include "../sylar/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void run_in_fiber() {
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber begin";
    sylar::Fiber::GetThis()->YieldToHold();
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber end";
    sylar::Fiber::GetThis()->YieldToHold();
}

void test_fiber(){
    SYLAR_LOG_INFO(g_logger) << "main begin -1";
    {
        sylar::Fiber::GetThis();

        SYLAR_LOG_INFO(g_logger) << "main begin";

        sylar::Fiber::ptr fiber(new sylar::Fiber(run_in_fiber));
        fiber->SwapIn();
        SYLAR_LOG_INFO(g_logger) << "main after swapIn";
        fiber->SwapIn();
        SYLAR_LOG_INFO(g_logger) << "main after end";
        fiber->SwapIn();
    }
    SYLAR_LOG_INFO(g_logger) << "main after end2";
}

int main(int argc, char** argv){
    
    sylar::Thread::setName("main");
    
    std::vector<sylar::Thread::ptr> thrs;
    for (int i = 0; i < 3; ++i){
        sylar::Thread::ptr thr(new sylar::Thread(&test_fiber, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }
    for (size_t i = 0; i < thrs.size(); ++i){
        thrs[i]->join();
    }
    
    return 0;
}