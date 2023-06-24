#include "fiber.h"
#include<atomic>
#include "macro.h"
#include "config.h"
#include "log.h"
#include "scheduler.h"

namespace sylar{

static Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};

//拿到当前协程
static thread_local Fiber* t_fiber = nullptr;
//表示master协程
static thread_local Fiber::ptr t_threadFiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = 
    Config::Lookup<uint32_t>("fiber.stack_size", 128*1024, "fiber stack size");

//栈内存分配器
class MallocStackAllocator {
public:
    static void* Alloc(size_t size){
        return malloc(size);
    }
    static void Dealloc(void* vp, size_t size){
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

uint64_t Fiber::GetFiberId(){
    if (t_fiber){
        return t_fiber->getId();
    }
    return 0;
}


Fiber::Fiber(){
    m_state = EXEC;
    SetThis(this);

    if (getcontext(&m_ctx)){
        SYLAR_ASSERT2(false, "getcontext");
    }
    ++s_fiber_count;

    SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber main";
}


Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller)
:m_id(++s_fiber_id), m_cb(cb){
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

    m_stack = StackAllocator::Alloc(m_stacksize);
    if (getcontext(&m_ctx)){
        SYLAR_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    //m_ctx.uc_link = t_threadFiber->m_ctx.uc_link;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    if (!use_caller)
    {
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    }else{
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }
    SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;

}
Fiber::~Fiber(){
    --s_fiber_count;
    if(m_stack) {
        SYLAR_ASSERT(m_state == TERM
                || m_state == EXCEPT
                || m_state == INIT);

        StackAllocator::Dealloc(m_stack, m_stacksize);
    } else {
        SYLAR_ASSERT(!m_cb);
        SYLAR_ASSERT(m_state == EXEC);

        Fiber* cur = t_fiber;
        if(cur == this) {
            SetThis(nullptr);
        }
    }
    SYLAR_LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id
                              << " total=" << s_fiber_count;
}

//重置协程函数， 并重置状态
//INIT，TERM
void Fiber::reset(std::function<void()> cb){
    SYLAR_ASSERT(m_stack);
    SYLAR_ASSERT(m_state == TERM || m_state == EXCEPT || m_state == INIT);

    m_cb = cb;
    if (getcontext(&m_ctx)){
        SYLAR_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr; //uc.link指向一个要被唤醒的协程
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}
//  * @brief 将当前线程切换到执行状态
//  * @pre 执行的为当前线程的主协程
void Fiber::call() {
    SetThis(this);
    m_state = EXEC;
    if(swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
        SYLAR_ASSERT2(false, "swapcontext");
    }
}

void Fiber::back() {
    SetThis(t_threadFiber.get());
    if(swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
        SYLAR_ASSERT2(false, "swapcontext");
    }
}

//切换到当前协程执行
void Fiber::SwapIn(){
    SetThis(this);
    SYLAR_ASSERT(m_state != EXEC);
    m_state = EXEC;
    if (swapcontext( &Scheduler::GetMainFiber()->m_ctx, &m_ctx)){
        SYLAR_ASSERT2(false, "swapcontext");    
    }
} 
//把当前协程切换到后台 
void Fiber::SwapOut(){
    SetThis(Scheduler::GetMainFiber());
    //SYLAR_ASSERT(t_threadFiber.get());
    
    if (swapcontext( &m_ctx, &Scheduler::GetMainFiber()->m_ctx)){
        SYLAR_ASSERT2(false, "swapcontext");    
    }
}  

//设置当前线程的运行协程
void Fiber::SetThis(Fiber* f){
    t_fiber = f;
}

//返回当前协程
Fiber::ptr Fiber::GetThis(){
    if (t_fiber){
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber);
    SYLAR_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}
//让出执行权，并设置自己的状态为Ready
void Fiber::YieldToReady(){
    Fiber::ptr cur = GetThis();
    cur->m_state = READY;
    cur->SwapOut();
} 
 //让出执行权，并设置自己的状态为Hold
void Fiber::YieldToHold(){
    Fiber::ptr cur = GetThis();
    SYLAR_ASSERT(cur->m_state == EXEC);
    cur->m_state = HOLD;
    cur->SwapOut();
}    
    //总协程数
uint64_t Fiber::TotalFibers(){
    return s_fiber_count;
}

void Fiber::MainFunc(){
    Fiber::ptr cur = GetThis();
    SYLAR_ASSERT(cur);

    try{
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    }catch(std::exception& ex){
        cur->m_state = EXCEPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << sylar::BackTraceToString();;
    }catch(...){
        cur->m_state = EXCEPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except"
            << " fiber_id=" << cur->getId()
            << std::endl
            << sylar::BackTraceToString();;
    }
    //解决协程无法回到主协程问题
    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->SwapOut();

    SYLAR_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}

void Fiber::CallerMainFunc(){
    Fiber::ptr cur = GetThis();
    SYLAR_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << sylar::BackTraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except"
            << " fiber_id=" << cur->getId()
            << std::endl
            << sylar::BackTraceToString();
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->back();
    SYLAR_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}

}