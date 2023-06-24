#include "../sylar/http/http.h"
#include "../sylar/log.h"

void test_request() {
    sylar::http::HttpRequest::ptr req(new sylar::http::HttpRequest);
    req->setHeader("host", "www.baidu.com");
    req->setBody("hello world!");

    req->dump(std::cout) << std::endl;
}

void test_response(){
    sylar::http::HttpResponse::ptr rsq(new sylar::http::HttpResponse);
    rsq->setHeader("X-X", "baidu");
    rsq->setBody("hello world!");

    rsq->setStatus(sylar::http::HttpStatus(200));
    rsq->setClose(false);

    rsq->dump(std::cout) << std::endl;
}

int main(int argc, char** argv){

    test_request();
    test_response();
    std::cout << "hello world" << std::endl;
    return 0;
}