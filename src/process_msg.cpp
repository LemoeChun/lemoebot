#include <cpr/cprtypes.h>
#include <cpr/response.h>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>
#include <cstdlib>

#include <yaml-cpp/yaml.h>
#include <cpr/cpr.h>

#include "process_msg.hpp"

using Toml = toml::table;
void about(Toml& config,json &resp_msg,json &msg,std::string &arg){
    resp_msg["params"]["message"][0]["type"] = "text";
    resp_msg["params"]["message"][0]["data"]["text"] = "你好喵，这里是帕酱～";
}
void DownloadAudio(Toml &config,json &resp_msg,json &msg,std::string &arg){
    std::string AudioDir = config["Bot"]["download_dir"].value_or("bot");
    std::filesystem::remove_all("/home/lemoechun/bot/audio/");

    if ( (std::system(std::string("yt-dlp --cookies-from-browser firefox -t mp3 --force-overwrites -o /home/lemoechun/bot/audio/%\\(title\\)s.%\\(ext\\)s ").append(arg).c_str())) !=0 ){
        resp_msg["params"]["message"][0]["type"] = "text";
        resp_msg["params"]["message"][0]["data"]["text"] = "嗝儿～音频被吃掉了喵～";
        return;
    } else {
        if (msg["message_type"]=="group"){
            resp_msg["action"] = "upload_group_file";
        } else {
            resp_msg["action"] = "upload_private_file";
        }
        resp_msg["params"].erase("message");
        resp_msg["params"].erase("message_type");
        std::string filename = std::filesystem::path((*std::filesystem::directory_iterator("audio/"))).filename().string();
        resp_msg["params"]["name"] = filename;
        resp_msg["params"]["file"] = "/home/lemoechun/bot/audio/"+filename;
    };
    
}

void DownloadVideo(Toml &config,json &resp_msg,json &msg,std::string &arg){
    if ( (std::system(std::string("yt-dlp --cookies-from-browser firefox -t mp4 --force-overwrites -o /home/lemoechun/bot/video/video.mp4 ").append(arg).c_str())) !=0 ){
        resp_msg["params"]["message"][0]["type"] = "text";
        resp_msg["params"]["message"][0]["data"]["text"] = "嗝儿～视频被吃掉了喵～";
        return;
    } else {
        resp_msg["params"]["message"][0]["type"] = "video";
        resp_msg["params"]["message"][0]["data"]["file"] = "/home/lemoechun/bot/video/video.mp4";
        resp_msg["params"]["message"][0]["data"]["url"] = "file:///home/lemoechun/bot/video/video.mp4";
    };
    
}

void jm(Toml &config,json &resp_msg,json &msg,std::string &arg){
    std::string download_dir = "/home/lemoechun/bot/pdf/" + arg; 
    YAML::Node options;
    options["plugins"]["after_album"][0]["plugin"] = "img2pdf";
    options["plugins"]["after_album"][0]["kwargs"]["pdf_dir"] = download_dir;
    options["plugins"]["after_album"][0]["kwargs"]["filename_rule"] = "Aname";
    // options["plugins"]["after_album"][0]["kwargs"]["encrypt"]["password"] = "nyanyanya";
    std::ofstream jm_option_file("jm_option.yml");
    jm_option_file.clear();
    jm_option_file << options;
    jm_option_file.close();
    std::filesystem::create_directories(download_dir);
    std::string jm_command = "jmcomic --option jm_option.yml " + arg;
    if ( (std::system(jm_command.data())) !=0 ){
        resp_msg["params"]["message"][0]["type"] = "text";
        resp_msg["params"]["message"][0]["data"]["text"] = "hentai！一天到晚看本子真是没救了喵";
        return;
    };
    std::string filename = std::filesystem::path((*std::filesystem::directory_iterator("/home/lemoechun/bot/pdf/" + arg))).filename().string();
    if (msg["message_type"]=="group"){
        resp_msg["action"] = "upload_group_file";
    } else {
        resp_msg["action"] = "upload_private_file";
    }
    resp_msg["params"].erase("message");
    resp_msg["params"].erase("message_type");
    resp_msg["params"]["name"] = filename;
    resp_msg["params"]["file"] = download_dir + "/" + filename;
    
    
}
/**
void get_file(json &resp_msg,json &msg,std::string &arg){
    if (msg["message_type"]=="group"){
        resp_msg["action"] = "upload_group_file";
    } else {
        resp_msg["action"] = "upload_private_file";
    }
    resp_msg["params"].erase("message");
    resp_msg["params"].erase("message_type");
    
    std::string filename = std::filesystem::path(arg).filename().string();
    resp_msg["params"]["name"] = filename;
    resp_msg["params"]["file"] = arg;
}
**/
json ProcessMsg(Toml &config,json &msg){
    auto raw_message = std::string(msg["raw_message"]);
    std::string command,arg;
    std::unordered_map<std::string, std::function<void(Toml&,json&,json&,std::string&)>> commands;
    commands["about"] = about;
    commands["jm"] = jm;
    commands["下载视频"] = DownloadVideo;
    commands["下载音频"] = DownloadAudio;
//    commands["get_file"] = get_file;
    json resp_msg;
    resp_msg["action"] = "send_msg";
    resp_msg["params"]["message_type"] = msg["message_type"];
    if (msg["message_type"]=="group"){
        resp_msg["params"]["group_id"] = msg["group_id"];
    }
        resp_msg["params"]["user_id"] = msg["user_id"];
    if (raw_message[0] == '>'){
        size_t first_space = raw_message.find(" ");
        if (first_space != std::string::npos){
            command = raw_message.substr(1,first_space-1);
            arg = raw_message.substr(first_space+1);
        } else {
            command = raw_message.substr(1);
            arg = "";
        }
        commands[command](config,resp_msg,msg,arg);
        return resp_msg;
    };
    if (msg["message"][0]["type"] == "json"){
        auto json_msg_data = nlohmann::json::parse(std::string(msg["message"][0]["data"]["data"]));
        std::cout<< json_msg_data.dump(4) << "\n";
        std::string url = cpr::Get(cpr::Url{json_msg_data["meta"]["detail_1"]["qqdocurl"]}).url.str();
        std::cout << url << "\n";
        url = url.substr(0,url.find("?"));
        std::cout << url << "\n";
        resp_msg["params"]["message"][0]["type"] = "reply" ;
        resp_msg["params"]["message"][0]["data"]["id"] = std::to_string(int(msg["message_id"])) ;
        resp_msg["params"]["message"][1]["type"] = "text" ;
        resp_msg["params"]["message"][1]["data"]["text"] = "标题： " + std::string(json_msg_data["meta"]["detail_1"]["desc"]) + "\n\n" + "链接： " + url;
        return resp_msg;
    }
    
    return nlohmann::json();
}

