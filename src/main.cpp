#include <string>
#include <nlohmann/json.hpp>
#include <toml++/toml.hpp>
#include <HXLibs/net/client/HttpClient.hpp>
#include <HXLibs/log/Log.hpp>

#include "config.hpp"
#include "process_msg.hpp"

using json = nlohmann::json;
auto conf = toml::parse_file("config.toml") ;
const toml::table &config = conf;
std::string url = config["Bot"]["Websockets_Push_URL"].value_or<std::string>("ws://127.0.0.1:3100/event");
HX::coroutine::Task<> coMain(){
    HX::net::HttpClient cli{};
        try {
            co_await cli.coWsLoop(url,
                [](HX::net::WebSocketClient ws) -> HX::coroutine::Task<> {
                auto msg = json::parse(co_await ws.recvText());
                if (!((msg["post_type"] == "meta_event") && (msg["meta_event_type"] == "heartbeat"))){
                    HX::log::hxLog.info("收到：\n",msg.dump(4));
                }
                if (msg["event_type"] == "message_receive"){
                   ProcessMsg(msg);
                }
            }
        );
    } catch (std::runtime_error const& err){
        HX::log::hxLog.error("ws err:", err.what());
    }
}


int main() {
    while (true){
        coMain().runSync();
    }
}

