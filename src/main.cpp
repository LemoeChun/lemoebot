#include <future>
#include <string>
#include <nlohmann/json.hpp>
#include <thread>
#include <toml++/toml.hpp>
#include <HXLibs/net/client/HttpClient.hpp>
#include <HXLibs/log/Log.hpp>

#include "config.hpp"
#include "process_msg.hpp"

using json = nlohmann::json;
auto conf = toml::parse_file("config.toml") ;
const toml::table &CONFIG = conf;
const std::string &WORK_DIR = CONFIG["Bot"]["Work_Dir"].value_or<std::string>("bot");
const std::string &API = CONFIG["Bot"]["Millky_API_URL"].value_or<std::string>("http://127.0.0.1:3100/api");
const uint &BOT_ID = CONFIG["Bot"]["QQ_ID"].value_or<uint>(114514);
const std::string &BOT_NAME = CONFIG["Bot"]["QQ_NAME"].value_or<std::string>("喵呜");
std::string url = CONFIG["Bot"]["Websockets_Push_URL"].value_or<std::string>("ws://127.0.0.1:3100/event");
HX::coroutine::Task<> coMain(){
    HX::net::HttpClient cli{};
        try {
            co_await cli.coWsLoop(url,
                [](HX::net::WebSocketClient ws) -> HX::coroutine::Task<> {
                while (true){
                    auto msg = json::parse(co_await ws.recvText());
                    if (!((msg["post_type"] == "meta_event") && (msg["meta_event_type"] == "heartbeat"))){
                        HX::log::hxLog.info("收到：\n",msg.dump(4));
                    }
                    if (msg["event_type"] == "message_receive"){
                       std::thread(ProcessMsg,msg).detach();
                    }
                }
            }
        );
    } catch (std::runtime_error const& err){
        HX::log::hxLog.error("ws err:", err.what());
    }
}


int main() {
    coMain().runSync();
}

