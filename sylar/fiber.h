#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__

#include<ucontext.h> //协程
#include<memory>
#include "thread.h"
#include<functional>
//#include "scheduler.h"

namespace sylar{

class Scheduler;
class Fiber : public std::enable_shared_from_this<Fiber>{  //继承为了使用share_from_this,返回自己的智能指针
friend class sylar::Scheduler;
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State{
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };

private:
    Fiber();

public:
    Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
    ~Fiber();

    //重置协程函数， 并重置状态
    void reset(std::function<void()> cb);
    void SwapIn();  //切换到当前协程执行
    void SwapOut();  //把当前协程切换到后台
    void call();
    void back();

    uint64_t getId() const { return m_id; }
    /**
     * @brief 返回协程状态
     */
    State getState() const { return m_state;}

public:
    //设置当前协程
    static void SetThis(Fiber* f);
    //返回当前协程
    static Fiber::ptr GetThis();
    static void YieldToReady();  //让出执行权，并设置自己的状态为Ready
    static void YieldToHold();     //让出执行权，并设置自己的状态为Hold
    //总协程数
    static uint64_t TotalFibers();
    /**
     * @brief 协程执行函数
     * @post 执行完成返回到线程主协程
     */
    static void MainFunc();
    /**
     * @brief 协程执行函数
     * @post 执行完成返回到线程调度协程
     */
    static void CallerMainFunc();

    static uint64_t GetFiberId();

private:
    uint64_t m_id;
    uint32_t m_stacksize;
    State m_state = INIT;

    ucontext_t m_ctx;
    void* m_stack = nullptr;
    //协程执行函数
    std::function<void()> m_cb;
};

}

#endif