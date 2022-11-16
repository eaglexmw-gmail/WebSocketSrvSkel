#ifndef _WSPP_FILE_SERVER_H_
#define _WSPP_FILE_SERVER_H_

#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <functional>

#include "zip_fileserver.h"

template<class Base>
class Request
{
private:
    const Base* request_;

public:
    explicit Request(const Base* req):request_(req) {};

    const std::string & get_method () const;
    const std::string & get_uri () const;
    const std::string & get_header (const std::string &key) const;
    const std::string & get_body () const;
    const std::string & get_version () const;
};

template<class Base>
class Response
{
private:
    Base* response_;

public:
    explicit Response(Base* rsp):response_(rsp) {};

    void set_status (uint32_t status_);
    void set_body (const std::string &);
    void set_body (const uint8_t* data, uint32_t len);
    void set_header(const std::string& key, const std::string& val);
    void append_header(const std::string& key, const std::string& val);
};

template<class Base>
class WebSocketMsg
{
private:
    const Base* message_;

public:
    explicit WebSocketMsg(const Base* msg):message_(msg) {};
    
    uint32_t get_opcode() const;
    const std::string& get_header ()const;
    const std::string& get_payload ()const;
};

template<class ReqBase, class RspBase, class MsgBase>
class WsFileServer : public ZipFileServer
{
public:
    using WsHandler = std::function<void(void* con, const Request<ReqBase>&, const WebSocketMsg<MsgBase>&)>;
    using GetHandler = std::function<bool (void* con, const Request<ReqBase>&, Response<RspBase>&)>;

private:
    using WsHandlers = std::vector<std::pair<std::regex, WsHandler>>;
    WsHandlers message_handlers_;

    using GetHandlers = std::vector<std::pair<std::regex, GetHandler>>;
    GetHandlers get_handlers_;

    template<typename ConnectionType>
    static void _on_http(WsFileServer<ReqBase, RspBase, MsgBase>* svr, ConnectionType hdl){
        if (nullptr != svr) {
            svr->on_http<ConnectionType>(&hdl);
        }
    }
    template<typename ConnectionType>
    static void _on_message(WsFileServer<ReqBase, RspBase, MsgBase>* svr, ConnectionType hdl, MsgBase msg)
    {
        if (nullptr != svr) {
            const WebSocketMsg<MsgBase> msg_(&msg);
            svr->on_message<ConnectionType>(&hdl, msg_);
        }
    }

    template<typename ConnectionType>
    void on_http(ConnectionType* conn_) {
        try {
            RspBase rsp;
            const Request<ReqBase> req_(&get_request(conn_));

            const std::string& strUrl = req_.get_uri();
            const std::string& strMethod = req_.get_method();
            if (strMethod.compare("GET") == 0) {
                Response<RspBase> rsp_(&rsp);
                if (handle_get_file(strUrl, conn_, req_, rsp_)){
                    send_http_msg(conn_, false, rsp);
                } else {
                    if (dispatch_http_get(strUrl, conn_, req_, rsp_)){
                        send_http_msg(conn_, false, rsp);
                    } else {
                        send_http_msg(conn_, true, rsp);
                    }
                }
            }
        } catch(std::exception &e) {
            std::cout << "exception: " << e.what() << std::endl;
        } catch(...) {
            std::cout << "exception! " << std::endl;
        }
    }
    template<typename ConnectionType>
    void on_message(ConnectionType* conn_, const WebSocketMsg<MsgBase>& msg) {
        try {
            const Request<ReqBase> req_(&get_request(conn_));
            const std::string& strUrl = req_.get_uri();
            // dispatch websocket msg
            for (const auto &x : message_handlers_) {
                const auto &pattern = x.first;
                const auto &handler = x.second;
                if (std::regex_match(strUrl, pattern)) {
                    handler(conn_, req_, msg);
                    return;
                }
            }
        } catch(std::exception &e) {
            std::cout << "exception: " << e.what() << std::endl;
        } catch(...) {
            std::cout << "exception! " << std::endl;
        }
    }

protected:
    void*       m_endpoint;

    bool dispatch_http_get(const std::string& path, void* con, const Request<ReqBase>& req, Response<RspBase>& rsp) {
        for (const auto &x : get_handlers_) {
            const auto &pattern = x.first;
            const auto &handler = x.second;
            if (std::regex_match(path, pattern)) {
                if (handler(con, req, rsp)) {
                    return true;
                }
            }
        }
        return false;
    };
    bool handle_get_file(const std::string& path, void* con, const Request<ReqBase>& req, Response<RspBase>& rsp);

public:
    WsFileServer() : ZipFileServer(), m_endpoint(nullptr) {};

    void* get_endpoint() {return m_endpoint;};
    const ReqBase& get_request(void* conn_);

    bool init();
    void run(uint32_t port);

    void send_bin_msg(void* con, const uint8_t* data, uint32_t len) const;
    void send_text_msg(void* con, const std::string& msg) const;
    void send_http_msg(void* con, bool not_found, const RspBase& rsp) const;

    inline WsFileServer& Get(const std::string &pattern, GetHandler handler) {
        get_handlers_.push_back(std::make_pair(std::regex(pattern), std::move(handler)));
        return *this;
    }

    inline WsFileServer& WsMsg(const std::string &pattern, WsHandler handler) {
        message_handlers_.push_back(std::make_pair(std::regex(pattern), std::move(handler)));
        return *this;
    }

};

#endif
