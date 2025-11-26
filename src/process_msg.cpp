#include <cstddef>
#include <functional>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>
#include <cstdlib>

#include <cpr/cpr.h>

#include "config.hpp"
#include "commands.hpp"
#include "process_msg.hpp"
#include "milky_api.hpp"

void ProcessMsg(const json &msg){
    std::string command,arg;
    std::unordered_map<std::string, std::function<void(json&,const json&,std::string&)>> commands;
    commands["about"] = about;
    commands["help"] = help;
    commands["jm"] = jm;
    commands["下载视频"] = DownloadVideo;
    commands["下载音频"] = DownloadAudio;
    commands["塔罗牌"] = batarot;
//    commands["get_file"] = get_file;
    json resp_msg;
    if (msg["data"].contains("group")){
        resp_msg["group_id"] = msg["data"]["group"]["group_id"];
    } else if(msg["data"].contains("friend")) {
        resp_msg["user_id"] = msg["data"]["friend"]["user_id"];
    }
    if (msg["data"]["segments"][0]["type"] == "text"){
        auto raw_message = std::string(msg["data"]["segments"][0]["data"]["text"]);
        if (raw_message[0] == '>' || raw_message[0] == '#' || raw_message[0] == '$'){
            size_t first_space = raw_message.find(" ");
            if (first_space != std::string::npos){
                command = raw_message.substr(1,first_space-1);
                arg = raw_message.substr(first_space+1);
            } else {
                command = raw_message.substr(1);
                arg = "";
            }
        if (!commands.contains(command)) return;
            commands[command](resp_msg,msg,arg);
        }
    } else if (msg["data"]["segments"][0]["type"] == "light_app"){
        std::cout << "小程序！\n" ;
        auto light_app = nlohmann::json::parse(std::string(msg["data"]["segments"][0]["data"]["json_payload"]));
        std::cout<< light_app.dump(4) << "\n";
        std::string url = cpr::Get(cpr::Url{ 
                (light_app["view"] == "news") ? light_app["jumpUrl"] : light_app["meta"]["detail_1"]["qqdocurl"]}).url.str();
        std::cout << url << "\n";
        url = url.substr(0,url.find("?"));
        std::cout << url << "\n";
        resp_msg["message"][0]["type"] = "reply" ;
        resp_msg["message"][0]["data"]["message_seq"] = msg["data"]["message_seq"] ;
        resp_msg["message"][1]["type"] = "text" ;
        resp_msg["message"][1]["data"]["text"] = "标题： " + std::string(light_app["meta"]["detail_1"]["desc"]) + "\n\n" + "链接： " + url;
        SendMsg(resp_msg);
    }
    
}

