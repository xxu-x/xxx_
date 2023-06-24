#include "../sylar/sylar.h"
#include <assert.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_assert(){
    SYLAR_LOG_INFO(g_logger) << sylar::BackTraceToString(10);
    SYLAR_ASSERT2(false, "戳啦");
}

int main(int agrc, char** argv){
    
    test_assert();
    return 0;

}