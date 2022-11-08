#include <stdio.h>
#include <web_assets_basews.h>

#include <iostream>

#include "wspp_fileserver.h"

#define ASIO_STANDALONE
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::http::parser::request   ws_request;
typedef websocketpp::http::parser::response  ws_response;
typedef websocketpp::server<websocketpp::config::asio>::message_ptr  ws_msg;

int main(int argc, char* argv[])
{
    ZipFileSystem zfs;
    // HTTP
    WsFileServer<ws_request, ws_response, ws_msg> svr;

    uint32_t assets_len = 0;
    const uint8_t* assets = NULL;
    get_web_assets_basews(&assets, &assets_len);

    if (!zfs.init(assets, assets_len)) {
        printf("init zip file system fail.\n");
        return -1;
    }
    svr.set_fs_handle(&zfs);
    svr.set_fs_base_dir("/static");
    // 
    svr.Get("/hello", [svr](void* con, const Request<ws_request>& req, Response<ws_response>& rsp) -> bool {
        //
        std::cout << "url: " << req.get_uri() << std::endl;
        std::cout << "method: " << req.get_method() << std::endl;
        std::string result = "{\"id\": 234}";
        rsp.set_status(200);
        rsp.set_body(result);
        return true;
    });
    // 
    svr.WsMsg("/websocket", [svr](void* con, const Request<ws_request>& req, const WebSocketMsg<ws_msg>& msg) {
        //
        std::cout << "url: " << req.get_uri() << std::endl;
        std::cout << "method: " << req.get_method() << std::endl;
        std::cout << "opcode: " << msg.get_opcode() << std::endl;
        std::cout << "header: " << msg.get_header() << std::endl;
        std::cout << "payload: " << msg.get_payload() << std::endl;
        std::string result = "{\"id\": 123}";
        svr.send_text_msg(con, result);
    });

    if (svr.init()) {
        svr.run(9002);
    }
}