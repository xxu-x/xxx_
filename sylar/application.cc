#include "application.h"

#include <unistd.h>
#include <signal.h>

#include "tcp_server.h"
#include "daemon.h"
#include "config.h"
#include "env.h"
#include "log.h"
//#include "worker.h"

namespace sylar{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static sylar::ConfigVar<std::string>::ptr g_server_work_path =
    sylar::Config::Lookup("server.work_path"
            ,std::string("/mnt/e/sylar_Project/sylar/sylar")
            , "server work path");

static sylar::ConfigVar<std::string>::ptr g_server_pid_file =
    sylar::Config::Lookup("server.pid_file"
            ,std::string("sylar.pid")
            , "server pid file");

// static sylar::ConfigVar<std::string>::ptr g_service_discovery_zk =
//     sylar::Config::Lookup("service_discovery.zk"
//             ,std::string("")
//             , "service discovery zookeeper");


struct HttpServerConf{
    std::vector<std::string> address;
    int keepalive = 0;
    int timeout = 1000 * 2 * 60;
    std::string name;

    bool isValid() const{
        return !address.empty();
    }

    bool operator==(const HttpServerConf& rhs) const{
        return address == rhs.address
            && keepalive == rhs.keepalive
            && timeout == rhs.timeout
            && name == rhs.name;
    }
};

template<>
class LexicalCast<std::string, HttpServerConf> {
public:
    HttpServerConf operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        HttpServerConf conf;
        conf.keepalive = node["keepalive"].as<int>();
        conf.timeout = node["timeout"].as<int>();
        conf.name = node["name"].as<std::string>();
        if (node["address"].IsDefined()) {
            for (size_t i = 0; i < node["address"].size(); ++i) {
                conf.address.push_back((node["address"][i].as<std::string>()));
            }
        }
        return conf;
    }
};

template<>
class LexicalCast<HttpServerConf, std::string> {
public:
    std::string operator()(const HttpServerConf& conf) {
        YAML::Node node;
        node["name"] = conf.name;
        node["keepalive"] = conf.keepalive;
        node["timeout"] = conf.timeout;
        for (auto& i : conf.address) {
            node["address"].push_back(i);
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

static sylar::ConfigVar<std::vector<HttpServerConf> >::ptr g_http_servers_conf
     = sylar::Config::Lookup("http_servers", std::vector<HttpServerConf>(), "http server config");

Application* Application::s_instance = nullptr;

Application::Application() {
    s_instance = this;
}

bool Application::init(int argc, char** argv) {
    m_argc = argc;
    m_argv = argv;

    sylar::EnvMgr::Getinstance()->addHelp("s", "start with the terminal");
    sylar::EnvMgr::Getinstance()->addHelp("d", "run as daemon");
    sylar::EnvMgr::Getinstance()->addHelp("c", "conf path default: ./conf");
    sylar::EnvMgr::Getinstance()->addHelp("p", "print help");

    //bool is_print_help = false;
    if(!sylar::EnvMgr::Getinstance()->init(argc, argv)) {
        sylar::EnvMgr::Getinstance()->printHelp();
        return false;
    }

    if(sylar::EnvMgr::Getinstance()->has("p")) {
        sylar::EnvMgr::Getinstance()->printHelp();
        return false;
    }

    // std::string conf_path = sylar::EnvMgr::Getinstance()->getConfigPath();
    // SYLAR_LOG_INFO(g_logger) << "load conf path:" << conf_path;
    // sylar::Config::LoadFromConfDir(conf_path);

    // ModuleMgr::Getinstance()->init();
    // std::vector<Module::ptr> modules;
    // ModuleMgr::Getinstance()->listAll(modules);

    // for(auto i : modules) {
    //     i->onBeforeArgsParse(argc, argv);
    // }

    // if(is_print_help) {
    //     sylar::EnvMgr::Getinstance()->printHelp();
    //     return false;
    // }

    // for(auto i : modules) {
    //     i->onAfterArgsParse(argc, argv);
    // }
    // modules.clear();

    int run_type = 0;
    if(sylar::EnvMgr::Getinstance()->has("s")) {
        run_type = 1;
    }
    if(sylar::EnvMgr::Getinstance()->has("d")) {
        run_type = 2;
    }

    if(run_type == 0) {
        sylar::EnvMgr::Getinstance()->printHelp();
        return false;
    }

    std::string pidfile = g_server_work_path->getValue()
                                + "/" + g_server_pid_file->getValue();
    if(sylar::FSUtil::IsRunningPidfile(pidfile)) {
        SYLAR_LOG_ERROR(g_logger) << "server is running:" << pidfile;
        return false;
    }

    std::string conf_path = sylar::EnvMgr::Getinstance()->getAbsolutePath(
                sylar::EnvMgr::Getinstance()->get("c", "conf")
                );
    SYLAR_LOG_INFO(g_logger) << "load conf path:" << conf_path;
    sylar::Config::LoadFromConfDir(conf_path);

    if(!sylar::FSUtil::Mkdir(g_server_work_path->getValue())) {
        SYLAR_LOG_FATAL(g_logger) << "create work path [" << g_server_work_path->getValue()
            << " errno=" << errno << " errstr=" << strerror(errno);
        return false;
    }
    return true;
}

bool Application::run() {
    bool is_daemon = sylar::EnvMgr::Getinstance()->has("d");
    return start_daemon(m_argc, m_argv,
            std::bind(&Application::main, this, std::placeholders::_1,
                std::placeholders::_2), is_daemon);
    //return true;
}

int Application::main(int argc, char** argv) {
    // signal(SIGPIPE, SIG_IGN);
    SYLAR_LOG_INFO(g_logger) << "main";
    // std::string conf_path = sylar::EnvMgr::Getinstance()->getConfigPath();
    // sylar::Config::LoadFromConfDir(conf_path);
    
        std::string pidfile = g_server_work_path->getValue()
                                    + "/" + g_server_pid_file->getValue();
        std::ofstream ofs(pidfile);
        if(!ofs) {
            SYLAR_LOG_ERROR(g_logger) << "open pidfile " << pidfile << " failed";
            return false;
        }
        ofs << getpid();

        auto http_confs = g_http_servers_conf->getValue();
        for (auto& i : http_confs) {
            SYLAR_LOG_INFO(g_logger) << LexicalCast<HttpServerConf, std::string>()(i);
        }
    

    m_mainIOManager.reset(new sylar::IOManager(1, true, "main"));
    m_mainIOManager->schedule(std::bind(&Application::run_fiber, this));
    m_mainIOManager->addTimer(2000, [](){
            //SYLAR_LOG_INFO(g_logger) << "hello";
    }, true);
    m_mainIOManager->stop();
    return 0;
}

int Application::run_fiber(){
    auto http_confs = g_http_servers_conf->getValue();
    for (auto& i : http_confs) {
        SYLAR_LOG_INFO(g_logger) << LexicalCast<HttpServerConf, std::string>()(i);

        std::vector<Address::ptr> address;
        for (auto& a : i.address) {
            size_t pos = a.find(":");
            if (pos == std::string::npos) {
                SYLAR_LOG_ERROR(g_logger) << "invalid address: " << a;
                continue;
            }
            auto addr = sylar::Address::LookupAny(a);
            if (addr) {
                address.push_back(addr);
                continue;
            }
            std::vector<std::pair<Address::ptr, uint32_t> > result;
            if (sylar::Address::GetInterfaceAddresses(result, a.substr(0, pos))) {
                SYLAR_LOG_ERROR(g_logger) << "invalid address: " << a;
                continue;
            }
            for (auto& x : result) {
                auto ipaddr = std::dynamic_pointer_cast<IPAddress>(x.first);
                if (ipaddr) {
                    ipaddr->setPort(atoi(a.substr(pos + 1).c_str()));
                }
                address.push_back(ipaddr);
            }
        }
        sylar::http::HttpServer::ptr server(new sylar::http::HttpServer(i.keepalive));
        std::vector<Address::ptr> fails;

        if (!server->bind(address, fails)) {
            for (auto& x : fails) {
                SYLAR_LOG_ERROR(g_logger) << "bind address fail" << *x;
            }
            _exit(0);
        }
        if (!i.name.empty())
            server->setName(i.name);
        server->start();
        m_httpservers.push_back(server);
    }
    return 0;
}

}