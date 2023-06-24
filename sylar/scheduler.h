#ifndef __SYLAR_SCHEDULER_H__
#define __SYLAR_SCHEDULER_H__

#include "fiber.h"
#include<memory>
#include "thread.h"
#include<list>
#include<iostream>
#include<vector>


namespace sylar{
/**
 * @brief 协程调度器
 * @details 封装的是N-M的协程调度器
 *          内部有一个线程池,支持协程在线程池里面切换
 */
class Scheduler{
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

     /**
     * @brief 构造函数
     * @param[in] threads 线程数量
     * @param[in] use_caller 是否使用当前调用线程
     * @param[in] name 协程调度器名称
     */
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler();

    const std::string& getName() const { return m_name;}
    static Scheduler* GetThis();
    //返回当前协程调度器的调度协程
    static Fiber* GetMainFiber();
    //启动协程调度器
    void start();
    //停止协程调度器
    void stop();

    /**
     * @brief 调度协程
     * @param[in] fc 协程或函数
     * @param[in] thread 协程执行的线程id,-1标识任意线程
     */
    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1){
        bool need_tictle =false;
        {
            MutexType::Lock lock(m_mutex);
            need_tictle = scheduleNoLock(fc, thread);
        }
        if (need_tictle){
            tickle();
        }

    }
     /**
     * @brief 批量调度协程
     * @param[in] begin 协程数组的开始
     * @param[in] end 协程数组的结束
     */
    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end){
        bool need_tictle =false;
        {
            MutexType::Lock lock(m_mutex);
            while (begin != end){
                need_tictle = scheduleNoLock(&*begin, -1) || need_tictle;
                ++begin;
            }
        }
        if (need_tictle){
            tickle();
        }

    }

protected:
    /**
     * @brief 通知协程调度器有任务了
     */
    virtual void tickle();
    //协程调度函数
    void run();
    //返回是否可以停止
    virtual bool stopping();
    //协程无任务可调度时执行idle协程
    virtual void idle();
    void setThis();
private:
    template<class FiberOrCb>
    /**
     * @brief 协程调度启动(无锁)
     */
    bool scheduleNoLock(FiberOrCb fc, int thread){
        bool need_tictle = m_fibers.empty();
        FiberAndThread ft(fc, thread);
        if (ft.fiber || ft.cb){
            m_fibers.push_back(ft);
        }
        return need_tictle;
    }
private:
    struct FiberAndThread
    {
        Fiber::ptr fiber;
        std::function<void()> cb;
        int thread;

        FiberAndThread(Fiber::ptr f, int thr)
        : fiber(f), thread(thr){

        }
        FiberAndThread(Fiber::ptr* f, int thr)
        : thread(thr){
            fiber.swap(*f);
        }

        FiberAndThread(std::function<void()> f, int thr)
        : cb(f), thread(thr){

        }

        FiberAndThread(std::function<void()>* f, int thr)
        : thread(thr){
            cb.swap(*f);
        }
        FiberAndThread()
        : thread(-1){

        }

        void reset(){
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };
    

private:
    MutexType m_mutex;
    std::vector<Thread::ptr> m_threads; //线程池
    std::list<FiberAndThread> m_fibers; //任务队列
    Fiber::ptr m_rootFiber;             // use_caller为true时有效, 调度协程
    std::string m_name;                 //协程调度器名称

protected:
    std::vector<int> m_threadIds;
    size_t m_threadCount = 0;
    std::atomic<size_t> m_activeThreadCount  {0};
    std::atomic<size_t> m_idleThreadCount  {0}; //空闲线程数量

    bool m_stopping = true;         //是否正在停止
    bool m_autoStop = false;        //是否自动停止

    int m_rootThread = 0;//主线程id
};

}

#endif