#include "../sylar/env.h"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>

struct A
{
    A(){
        std::ifstream ifs("/proc/" + std::to_string(getpid()) + "/cmdline", std::ios::binary);
        std::string content;
        content.resize(4096);
        ifs.read(&content[0], content.size());
        content.resize(ifs.gcount());

        for (size_t i = 0; i < content.size(); ++i) {
            std::cout << i << " - " << content[i] << " - " << (int)content[i] << std::endl;
        }

        std::cout << content << std::endl;
    }
    

};

A a;

int main(int argc, char** argv){

    sylar::EnvMgr::Getinstance()->addHelp("s", "start with the terminal");
    sylar::EnvMgr::Getinstance()->addHelp("d", "run as daemon");
    sylar::EnvMgr::Getinstance()->addHelp("p", "print help");

    if (!sylar::EnvMgr::Getinstance()->init(argc, argv)) {
        sylar::EnvMgr::Getinstance()->printHelp();
        return 0;
    }

    std::cout << "exe=" << sylar::EnvMgr::Getinstance()->getExe() << std::endl;
    std::cout << "cwd=" << sylar::EnvMgr::Getinstance()->getCwd() << std::endl;
    std::cout << "path=" << sylar::EnvMgr::Getinstance()->getEnv("PATH", "xxx") << std::endl;
    std::cout << "test=" << sylar::EnvMgr::Getinstance()->getEnv("TEST", "") << std::endl;
    std::cout << "set env" << sylar::EnvMgr::Getinstance()->setEnv("TEST", "yy") << std::endl;
    std::cout << "test=" << sylar::EnvMgr::Getinstance()->getEnv("TEST", "") << std::endl;


    if (sylar::EnvMgr::Getinstance()->has("d")) {
        sylar::EnvMgr::Getinstance()->printHelp();
        
    }
    return 0;
}