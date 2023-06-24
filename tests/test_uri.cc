#include "../sylar/uri.h"
#include <iostream>

int main(int argc, char** argv) {
    //sylar::Uri::ptr uri = sylar::Uri::Create("http://www.baidu.com/src/uri?id=100&name=xxx#frg");
    sylar::Uri::ptr uri = sylar::Uri::Create("http://admin@www.baidu.com/src/中文/uri?id=100&name=xxx&vv=中文#frg中文");
    std::cout << uri-> toString() << std::endl;
    auto addr = uri->createAddress();
    std::cout << *addr << std::endl;

    return 0;
}
