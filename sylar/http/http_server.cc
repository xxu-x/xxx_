#include "http_server.h"
#include "../log.h"

namespace sylar {
namespace http{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

HttpServer::HttpServer(bool keepalive
               ,sylar::IOManager* worker
               ,sylar::IOManager* io_worker
               ,sylar::IOManager* accept_worker)
    :sylar::TcpServer(worker, io_worker, accept_worker)
    ,m_isKeepalive(keepalive) {
   m_dispatch.reset(new ServletDispatch);
}


// void HttpServer::setName(const std::string& v) {
//     TcpServer::setName(v);
//     m_dispatch->setDefault(std::make_shared<NotFoundServlet>(v));
// }

void HttpServer::handleClient(sylar::Socket::ptr client) {
    SYLAR_LOG_DEBUG(g_logger) << "handleClient " << *client;
    sylar::http::HttpSession::ptr session(new HttpSession(client));
    do {
        auto req = session->recvRequest();
        if(!req) {
            SYLAR_LOG_DEBUG(g_logger) << "recv http request fail, errno="
                << errno << " errstr=" << strerror(errno)
                << " cliet:" << *client << " keep_alive=" << m_isKeepalive;
            break;
        }

        HttpResponse::ptr rsp(new HttpResponse(req->getVersion()
                            ,req->isClose() || !m_isKeepalive));
        rsp->setHeader("Server", getName());
        //rsp->setBody("hello, world!");
        m_dispatch->handle(req, rsp, session);
        session->sendResponse(rsp);
        //SYLAR_LOG_INFO(g_logger) << "request: " << *rsp;
        if(!m_isKeepalive || req->isClose()) {
            break;
        }
    } while(true);
    session->close();
}

}
}