#include "thread.h"
#include "log.h"
#include"util.h"
namespace sylar{
    //ThreadLocal中填充的变量属于当前线程，该变量对其他线程而言是隔离的，也就是说该变量是当前线程独有的变量。
    //ThreadLocal为变量在每个线程中都创建了一个副本，那么每个线程可以访问自己内部的副本变量
    static thread_local Thread* t_thread = nullptr;
    static thread_local std::string t_thread_name = "UNKNOW";
    //系统日志统一打印到system
    static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");


    Semaphore::Semaphore(uint32_t count){
        if (sem_init(&m_semaphore, 0, count)){
            throw std::logic_error("sem_init error");
        }
    }
    Semaphore::~Semaphore(){
        sem_destroy(&m_semaphore);
    }

    void Semaphore::wait(){
        if (sem_wait(&m_semaphore)){
            return;
        }
    }
    void Semaphore::notify(){
        if (sem_post(&m_semaphore)){
            throw std::logic_error("sem_post error");
        }
    }

    Thread* Thread::getThis(){
        return t_thread;
    }

    const std::string& Thread::GetName(){
        return t_thread_name;
    }

    void Thread::setName(const std::string &name){
        if (t_thread){
            t_thread->m_name = name;
        }
        t_thread_name = name;
    }

    Thread::Thread (std::function<void()> cb, const std::string& name):
        m_cb(cb), m_name(name){
        if (name.empty()){
            m_name = "UNKNOW";
        }
        int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
        if (rt){
            SYLAR_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << rt
                <<" name = " << name;
            throw std::logic_error("pthread_create error");
        }
        m_semaphore.wait();
    }
    Thread::~Thread(){
        if (m_thread){
            pthread_detach(m_thread);
        }
    }
   
    void Thread::join(){
        if (m_thread){
            int rt = pthread_join(m_thread, nullptr);
            if (rt){
                SYLAR_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << rt
                <<" name = " << m_name;
                throw std::logic_error("pthread_join error");
            }
            m_thread = 0;
        }
    }
    void* Thread::run(void* arg){
        Thread* thread = (Thread*)arg;
        t_thread = thread;
        t_thread_name = thread->m_name;
        thread->m_id = sylar::GetThreadId();
        pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

        std::function<void()> cb;
        cb.swap(thread->m_cb);

       thread->m_semaphore.notify();

        cb();
        return 0;
    }
}
