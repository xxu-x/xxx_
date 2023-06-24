#ifndef __SYLAR_LOG_H
#define __SYLAR_LOG_H

#include<string>
#include<stdint.h>
#include<memory>
#include<list>
#include<sstream>
#include<fstream>
#include<iostream>
#include<vector>
#include "singleton.h"
#include<map>
#include "util.h"
#include "thread.h"

/**
 * @brief 使用流式方式将日志级别level的日志写入到logger
 */
#define SYLAR_LOG_LEVEL(logger, level) \
    if (logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger,level \
                        , __FILE__, __LINE__, 0, sylar::GetThreadId(), \
            sylar::GetFiberId(), time(0), sylar::Thread::GetName()))).getSS()

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, sylar::GetThreadId(),\
                sylar::GetFiberId(), time(0), sylar::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别debug的日志写入到logger
 */
#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别info的日志写入到logger
 */
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...)  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别warn的日志写入到logger
 */
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...)  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别error的日志写入到logger
 */
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别fatal的日志写入到logger
 */
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

/**
 * @brief 获取主日志器
 */
#define SYLAR_LOG_ROOT() sylar::LoggerMgr::Getinstance()->getRoot()

/**
 * @brief 获取name的日志器
 */
#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::Getinstance()->getLogger(name)

namespace sylar{

class Logger;

class LoggerManager;

class LogLevel{
public:
    ///日志级别
    enum Level{
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
     /**
     * @brief 将日志级别转成文本输出
     * @param[in] level 日志级别
     */
    static const char* ToString(LogLevel::Level level);
    /**
     * @brief 将文本转换成日志级别
     * @param[in] str 日志级别文本
     */
    static Level FromString(const std::string& str);
};

/**
 * @brief 日志事件
 */
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
     /**
     * @brief 构造函数
     * @param[in] logger 日志器
     * @param[in] level 日志级别
     * @param[in] file 文件名
     * @param[in] line 文件行号
     * @param[in] elapse 程序启动依赖的耗时(毫秒)
     * @param[in] thread_id 线程id
     * @param[in] fiber_id 协程id
     * @param[in] time 日志事件(秒)
     * @param[in] thread_name 线程名称
     */
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level m_level, 
            const char* file, int32_t line, uint32_t elapse, uint32_t thread_id    
            , uint32_t fiber_id, uint64_t time, const std::string& thread_name);

    const char* getFile() const { return m_file; }
    int32_t getLine() const { return m_line; }
    uint32_t getElapse() const{ return m_elapse; }
    uint32_t getThreadId() const{ return m_threadId; }
    uint32_t getFiberId() const{ return m_fiberId; }
    uint64_t getTime() const{ return m_time; }
    std::string getContent() const { return m_ss.str(); }
    std::shared_ptr<Logger> getLogger() const { return m_logger;}
    const std::string getThreadName() const { return m_thread_name; }

    std::stringstream& getSS() { return m_ss;}
    LogLevel::Level getLevel() const {return m_level;}

    /**
     * @brief 格式化写入日志内容
     */
    void format(const char* fmt, ...);

     /**
     * @brief 格式化写入日志内容
     */
    void format(const char* fmt, va_list al);
private:
    //文件名
    const char* m_file = nullptr;  
    //行号
    int32_t m_line = 0;   
    //程序启动到现在的毫秒数         
    uint32_t m_elapse = 0;   
    //线程id      
    uint32_t m_threadId = 0;  
    //协程id      
    uint32_t m_fiberId = 0;        
    //时间戳
    uint64_t m_time = 0;     
    ///日志内容流       
    std::stringstream m_ss;
    ///线程名称
    std::string m_thread_name;
    /// 日志器
    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
};

/**
 * @brief 日志事件包装器
 */
class LogEventWrap{
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();
    LogEvent::ptr getEvent() const { return m_event;}

    /**
     * @brief 获取日志内容流
     */
    std::stringstream& getSS();
private:
    LogEvent::ptr m_event;
};

///  日志格式器
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    /**
     * @brief 构造函数
     * @param[in] pattern 格式模板
     * @details 
     *  %m 消息
     *  %p 日志级别
     *  %r 累计毫秒数
     *  %c 日志名称
     *  %t 线程id
     *  %n 换行
     *  %d 时间
     *  %f 文件名
     *  %l 行号
     *  %T 制表符
     *  %F 协程id
     *  %N 线程名称
     *
     *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
     */
    LogFormatter(const std::string& pattern);
    
    //%t    %thread_id %m%n
    /**
     * @brief 返回格式化日志文本
     * @param[in] logger 日志器
     * @param[in] level 日志级别
     * @param[in] event 日志事件
     */
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
     /**
     * @brief 日志内容项格式化
     */
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        //FormatItem(const std::string& fmt = "");
        virtual ~FormatItem(){}
        /**
         * @brief 格式化日志到流
         * @param[in, out] os 日志输出流
         * @param[in] logger 日志器
         * @param[in] level 日志等级
         * @param[in] event 日志事件
         */
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) = 0;
    };

    /**
     * @brief 初始化,解析日志模板
     */
    void init();

    /**
     * @brief 是否有错误
     */
    bool isError() const { return m_error;}

    /**
     * @brief 返回日志模板
     */
    const std::string getPattern() const { return m_pattern;}
private:
    ///日志格式模板
    std::string m_pattern;
    ///日志解析后格式
    std::vector<FormatItem::ptr> m_items; 
    /// @brief 是否有错误
    bool m_error = false;
};

//日志输出地
class LogAppender{
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;

    virtual ~LogAppender() {}

    /**
     * @brief 写入日志
     * @param[in] logger 日志器
     * @param[in] level 日志级别
     * @param[in] event 日志事件
     */
    virtual void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    /**
     * @brief 将日志输出目标的配置转成YAML String
     */
    virtual std::string toYamlString() = 0;

    /**
     * @brief 更改日志格式器
     */
    void setFormatter(LogFormatter::ptr val);

    /**
     * @brief 获取日志格式器
     */
    LogFormatter::ptr getFormatter() ;

    /**
     * @brief 设置日志级别
     */
    void setLevel(LogLevel::Level val){ m_level = val;}

    /**
     * @brief 获取日志级别
     */
    LogLevel::Level getLevel() const { return m_level;}

protected:
    /// 日志级别
    LogLevel::Level m_level = LogLevel::DEBUG;
    /// 日志格式器
    LogFormatter::ptr m_formatter;
    /// 是否有自己的日志格式器
    bool m_hasFormatter = false;

    Mutex m_mutex;
};

/**
 * @brief 日志器
 */
class Logger : public std::enable_shared_from_this<Logger>{
friend class LoggerManager;
public:
    typedef std::shared_ptr<Logger> ptr;
    
    /**
     * @brief 构造函数
     * @param[in] name 日志器名称
     */
    Logger(const std::string& name = "root");

    /**
     * @brief 写日志
     * @param[in] level 日志级别
     * @param[in] event 日志事件
     */
    void log(LogLevel::Level level, LogEvent::ptr event);

    /**
     * @brief 写debug级别日志
     * @param[in] event 日志事件
     */
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    /**
     * @brief 添加日志目标
     * @param[in] appender 日志目标
     */
    void addAppender(LogAppender::ptr appender);

    /**
     * @brief 删除日志目标
     * @param[in] appender 日志目标
     */
    void delAppender(LogAppender::ptr appender);

    /**
     * @brief 清空日志目标
     */
    void clearAppender();

     /**
     * @brief 返回日志级别
     */
    LogLevel::Level getLevel() const{ return m_level;}

    /**
     * @brief 设置日志级别
     */
    void setLevel(LogLevel::Level val){ m_level = val;}

    /**
     * @brief 返回日志名称
     */
    std::string getName() const  {return m_name;}

    /**
     * @brief 设置日志格式器
     */
    void setFormatter(LogFormatter::ptr val) { 
        Mutex::Lock lock(m_mutex);
        m_formatter = val;
        for (auto& i: m_appenders){
            Mutex::Lock l1(i->m_mutex);
            if (!i->m_hasFormatter){
                i->m_formatter = m_formatter;
            }
        }
    }

    /**
     * @brief 设置日志格式模板
     */
    void setFormatter(const std::string& val) { 

        sylar::LogFormatter::ptr new_val(new sylar::LogFormatter(val));
        if (new_val->isError()){
            std::cout << "Logger setFormatter name = " << m_name
                        << " value = " << val << " invalid formatter"
                        << std::endl;
            return;
        }
        //m_formatter = new_val;
        setFormatter(new_val);
    }
    LogFormatter::ptr getFormatter() { 
        Mutex::Lock lock(m_mutex);
        return m_formatter; 
    }
    /**
     * @brief 将日志器的配置转成YAML String
     */
    std::string toYamlString();
private:
    std::string m_name;                     //日志名称
    LogLevel::Level m_level;                //日志级别
    std::list<LogAppender::ptr> m_appenders;//日志目标集合
    LogFormatter::ptr m_formatter;          //日志格式器

    Logger::ptr m_root;                     //主日志器
    Mutex m_mutex;                          //锁
};

///输出到控制台的Appender
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void Log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;
};

///输出到文件的Appender
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void Log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;

    /**
     * @brief 重新打开日志文件
     * @return 成功返回true
     */
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;

    uint64_t m_lastTime;
};

/**
 * @brief 日志器管理类
 */
class LoggerManager{
public:
    LoggerManager();

    /**
     * @brief 获取日志器
     * @param[in] name 日志器名称
     */
    Logger::ptr getLogger(const std::string& name);
    /**
     * @brief 初始化
    */
    void init();

    /**
     * @brief 返回主日志器
     */
    Logger::ptr getRoot() const { return m_root;}

    /**
     * @brief 将所有的日志器配置转成YAML String
     */
    std::string toYamlString();
private:
    /// Mutex
    Mutex m_mutex;
    /// 日志器容器
    std::map<std::string, Logger::ptr> m_loggers;
    /// 主日志器
    Logger::ptr m_root;
};

/// 日志器管理类单例模式
typedef sylar::Singleton<LoggerManager> LoggerMgr;

}

#endif