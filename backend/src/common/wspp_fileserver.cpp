#include "wspp_fileserver.h"

#define ASIO_STANDALONE
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <functional>

typedef websocketpp::server<websocketpp::config::asio> asio_server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using websocketpp::lib::ref;

typedef websocketpp::http::parser::request   ws_request;
typedef websocketpp::http::parser::response  ws_response;
typedef asio_server::message_ptr             ws_msg;

template<>
const std::string & Request<ws_request>::get_method () const
{
    return request_->get_method();
}

template<>
const std::string & Request<ws_request>::get_uri () const
{
    return request_->get_uri();
}

template<>
const std::string & Request<ws_request>::get_header (const std::string &key) const
{
    return request_->get_header(key);
}

template<>
const std::string & Request<ws_request>::get_body () const
{
    return request_->get_body();
}

template<>
const std::string & Request<ws_request>::get_version () const
{
    return request_->get_version();
}

template<>
void Response<ws_response>::set_status (uint32_t status_)
{
    response_->set_status((websocketpp::http::status_code::value)(status_));
}

template<>
void Response<ws_response>::set_body (const std::string &body_)
{
    response_->set_body(body_);
}

template<>
void Response<ws_response>::set_body (const uint8_t* data, uint32_t len)
{
    std::string body_;
    body_.resize(len);
    memcpy(&body_[0], data, len);
    response_->set_body(body_);
}

template<>
void Response<ws_response>::set_header(const std::string& key, const std::string& val)
{
    response_->replace_header(key, val);
}

template<>
void Response<ws_response>::append_header(const std::string& key, const std::string& val)
{
    response_->append_header(key, val);
}

template<>
uint32_t WebSocketMsg<ws_msg>::get_opcode() const
{
    return (*message_)->get_opcode();
}

template<>
const std::string& WebSocketMsg<ws_msg>::get_header ()const
{
    return (*message_)->get_header();
}

template<>
const std::string& WebSocketMsg<ws_msg>::get_payload ()const
{
    return (*message_)->get_payload();
}

template<>
void WsFileServer<ws_request, ws_response, ws_msg>::send_text_msg(void* con, const std::string& msg) const
{
    auto con_ = (connection_hdl*)con;
    auto svr = ((asio_server*)m_endpoint);
    svr->send(*con_, msg, websocketpp::frame::opcode::text);
}

template<>
void WsFileServer<ws_request, ws_response, ws_msg>::send_bin_msg(void* con, const uint8_t* data, uint32_t len) const
{
    auto con_ = (connection_hdl*)con;
    auto svr = ((asio_server*)m_endpoint);
    svr->send(*con_, data, len, websocketpp::frame::opcode::binary);
}

template<>
void WsFileServer<ws_request, ws_response, ws_msg>::send_http_msg(void* conn_, bool not_found, const ws_response& rsp)  const
{
    asio_server::connection_ptr con = ((asio_server*)m_endpoint)->get_con_from_hdl(*(connection_hdl*)conn_);
    if (not_found)
        con->set_status(websocketpp::http::status_code::not_found);
    else{
        con->set_body(rsp.get_body());
        con->set_status(rsp.get_status_code());
    }
    //con->defer_http_response();
    //con->send_http_response();
}

template<>
const ws_request& WsFileServer<ws_request, ws_response, ws_msg>::get_request(void* conn_)
{
    asio_server::connection_ptr con = ((asio_server*)m_endpoint)->get_con_from_hdl(*(connection_hdl*)conn_);
    return con->get_request();
}

template<>
bool WsFileServer<ws_request, ws_response, ws_msg>::handle_get_file(const std::string& path, void* con, const Request<ws_request>& req, Response<ws_response>& rsp)
{
    std::string type_;
    std::string body_;
    // 读取到文件内容、及ContentType
    if (read_file_content(path, type_, body_)) {
        // 设置响应内容
        rsp.set_body(body_);
        // 设置ContentType
        if (type_.length() != 0) {
            rsp.set_header("Content-Type", type_);
        }
        // 206 or 200
        if (req.get_header("Range").length() == 0) {
            rsp.set_status(websocketpp::http::status_code::ok);
        } else {
            rsp.set_status(websocketpp::http::status_code::partial_content);
        }
        return true;
    }

    return false;
}

template<>
bool WsFileServer<ws_request, ws_response, ws_msg>::init()
{
    try{
        m_endpoint = (void*)(new asio_server);

        // Initialize Asio
        ((asio_server*)m_endpoint)->init_asio();

        ((asio_server*)m_endpoint)->set_error_channels(websocketpp::log::elevel::all);
        ((asio_server*)m_endpoint)->set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);

        ((asio_server*)m_endpoint)->set_message_handler(
            websocketpp::lib::bind(
                &WsFileServer<ws_request, ws_response, ws_msg>::_on_message<websocketpp::connection_hdl>, this, std::placeholders::_1, std::placeholders::_2));

        ((asio_server*)m_endpoint)->set_http_handler(
            websocketpp::lib::bind(
                &(WsFileServer<ws_request, ws_response, ws_msg>::_on_http<websocketpp::connection_hdl>), this, std::placeholders::_1));
            
    } catch(...) {
        return false;
    }
    return true;
}

template<>
void WsFileServer<ws_request, ws_response, ws_msg>::run(uint32_t port)
{
    ((asio_server*)m_endpoint)->set_reuse_addr(true);
    ((asio_server*)m_endpoint)->listen(port);
    ((asio_server*)m_endpoint)->start_accept();
    ((asio_server*)m_endpoint)->run();
}
