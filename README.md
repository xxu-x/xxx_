# C-

## 开发环境
Ubuntu-22.04
gcc 11.3.0
cmake

## 项目路径
bin -- 二进制
build -- 中间文件路径
cmake -- cmake函数文件夹
lib -- 库的输出路径
Makefile
sylar -- 源代码路径
tests -- 测试代码

## 日志系统
1)
    log4J
    Logger(定义日志类别)
        |
        |------ Formatter（日志格式）
        |
    Appender（日志输出地方）

## 配置系统
Config --> Yaml
yaml-cpp

配置系统的原则是， 约定优于配置：

```cpp
template<class T, class FromStr, class ToStr>
class ConfigVar;

template<T, F>
LexicalCast;
//容器偏特化
//list set vector map unordered_map unordered_set

```
自定义类型需要实现syar::LexicalCast 偏特化
实现后， 就可以支持Config解析自定义类型， 自定义类型可以和常规STL容器一起使用

配置的事件机制
当一个配置项发生修改的时候，可以反向通知代码回调

# 日志系统整和配置系统
```cpp
logs:
    -name: root
    -level: debug...
    -formatter: '%d%T%p%T%t%m%n'
    appender:
        - type: StdoutLogAppender, FileLogAppender
          level: debug...
          file: /logs/xxx.log
```

```cpp
static Logger::ptr g_log = SYLAR_LOG_NAME("system");
//m_system为空的时候就用的是m_root
//当logger的appenders为空， 使用root写logger
```

```cpp
//定义logDefine logAppenderDefine， 偏特化LexicalCast
//实现日志配置解析
```
## 线程库

Thread, Mutex

互斥量 mutex
信号量 semaphore

和Log整合
Logger appender formatter

SpinLock, 替换Mutex
写文件周期性reopen  防止文件被删掉,但是磁盘的内存没被释放掉

## 协程库封装
定义协程接口
ucontext_t.
自定义macro方便调试
              GetThis()
每个线程里有一个main_Fiber, main_fiber可以开启一个新的子协程
    也可以去调度子协程，一个结束让另一个去执行,子协程结束后会回到主协程

协程会出现无法销毁的问题，需要自己手动调度，而线程是操作系统调度的

```
协程调度模块scheduler
        1 - N       1-M
scheduler --> thread --> fiber
1、线程池， 分配一组线程
2、协程调度器， 将协程指定到相应的线程上去执行

N : M  (N个线程，M个协程，其中M个协程在N个线程上面切换)

包括一个线程池 和一个协程队列（包括回调函数和协程）
```
run()函数： 1、设置当前线程的scheduler
            2、设置当前线程的run、fiber
            3、协程调度循环while(true)
                -协程消息队列里面是否有任务
                -无任务执行，执行idle

```
IOManager(基于epoll) 继承 ---> Scheduler
                |
                |
                v
    idle(如果没有任务就会陷入epoll_wait)

一般的线程池是基于信号量
    信号量
PutMessage(msg,) 信号量+1
    Message_queue(消息队列)
        |
        |---Thread
        |---thread
            接收到就陷入wait() 信号量-1 RecvMessage

异步IO，线程等待数据返回的过程中，就会陷入epoll_wait()    
        这时候如果往句柄里面写，fd就会通知线程来处理，脱离epoll_wait()
        如果使用信号量来阻塞线程的话，异步IO的信号它是没办法接收的
```

```
Timer   --->addTimer()  添加定时任务
        --->cancel()  取消定时任务
获取当前的定时器触发离现在的时间差，
返回当前需要触发的定时器

```

```                 
                    [Fiber]              [Timer]
                        ^ N                 ^
                        |                   |
                        | 1                 |
                    [Thread]           [TimerManager]
                        ^  M                ^
                        |                   |
                        | 1                 |
                    [Scheduler] <---(继承) [IOManager(epoll)]
```
## HOOK
sleep,
usleep 

socket相关(socket, connect, accept)
io相关(read, write, send, recv)
fd相关(fcntl, iocnl)

## socket函数库

            UnixAddress
                |
            ---------                   |--IPV4
            |Address|------[IPAddress]--|
            ---------                   |--IPV6
                |
              socket  （不需要关注它是哪种address）

socket的api封装
    connect
    accept
    read/write/close

## 序列化bytearray

write(int, float, int64, ...)
read(int, float, int64, ...)

## http协议开发
HTTP/1.1 - API
URL解析

//实现两个结构体
HttpRequest;
HttpResponse;

GET / HTTP/1.1
Host: www.baidu.com

HTTP/1.1 200 OK
Accept-Ranges: bytes
Cache-Control: no-cache
Connection: keep-alive
Content-Length: 9508

uri: http://www.baidu.com:80/page/xxx?id=10&v=20#fr
    http , 协议
    www.baidu.com ,host
    80, 端口
    /page/xxx, path
    id=10&v=20 param
    fr fragment

ragel 

TcpServer封装，
基于TCPServer实现了一个EchoServer, 可以对其发消息

## Stream 针对文件、socket封装
read/write/readFixSize/writeFixSize(限定长度)

HttpSession/HttpConnection
Server.accept, socket->session
client connect socket -> connection

HttpServer 继承 TcpServer

            Servlet <--------FunctionServlet
                |
            ServletDispatch(管理Servlet)

## 分布协议

## 推荐系统
