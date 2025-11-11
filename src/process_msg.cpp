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

#include "config.hpp"
#include "process_msg.hpp"
#include "milky_api.hpp"

namespace fs = std::filesystem;
void about(json &resp_msg,json &msg,std::string &arg){
    resp_msg["message"][0]["type"] = "text";
    resp_msg["message"][0]["data"]["text"] = "你好喵，这里是帕酱～";
    SendMsg(resp_msg);
}
void DownloadAudio(json &resp_msg,json &msg,std::string &arg){
    std::string AudioDir = config["Bot"]["Download_Dir"].value_or<std::string>("bot") + "/audio/";
    fs::remove_all(AudioDir);

    if ( (std::system(std::string("yt-dlp --cookies-from-browser firefox -t mp3 --force-overwrites -o " + AudioDir +"%\\(title\\)s.%\\(ext\\)s " + arg).c_str())) !=0 ){
        resp_msg["message"][0]["type"] = "text";
        resp_msg["message"][0]["data"]["text"] = "嗝儿～音频被吃掉了喵～";
        SendMsg(resp_msg);
        return;
    } else {
        std::string filename = fs::path((*fs::directory_iterator(AudioDir))).filename().string();
        resp_msg["file_name"] = filename;
        resp_msg["file_uri"] = "file://" + AudioDir + filename;
        UpLoadFile(resp_msg);
    };
    
}

void DownloadVideo(json &resp_msg,json &msg,std::string &arg){
    std::string VideoDir = config["Bot"]["Download_Dir"].value_or<std::string>("bot") + "/video/";
    fs::remove_all(VideoDir);
    if ( (std::system(std::string("yt-dlp --cookies-from-browser firefox --write-thumbnail --convert-thumbnails jpg -t mp4 --force-overwrites -o " + VideoDir +"%\\(title\\)s.%\\(ext\\)s " + arg).c_str())) !=0 ){
        resp_msg["message"][0]["type"] = "text";
        resp_msg["message"][0]["data"]["text"] = "嗝儿～视频被吃掉了喵～";
        SendMsg(resp_msg);
        return;
    } else {
        auto files = fs::directory_iterator(VideoDir)
            | std::views::filter([](const fs::directory_entry& entry) {
              return entry.path().extension() == ".mp4";
          })
            | std::views::transform([](const fs::directory_entry& entry) {
        return entry.path().filename().string();
        });
        std::string filename = *std::ranges::begin(files);
        auto thumb = filename;
        thumb.replace(thumb.length()-3,3,"jpg");
        std::cout << thumb << "\n" << filename << "\n" <<fs::file_size(VideoDir + filename) << "\n" ;
/**
        if (fs::file_size(VideoDir + filename) < 104850000){
            resp_msg["message"][0]["type"] = "video";
            resp_msg["message"][0]["data"]["uri"] = "file://" + VideoDir +filename;
            resp_msg["message"][0]["data"]["thumb_uri"] = "file://" + VideoDir +thumb;
            SendMsg(resp_msg);
        } else { **/
        resp_msg["file_name"] = filename;
        resp_msg["file_uri"] = "file://" + VideoDir + filename;
        UpLoadFile(resp_msg);

//        }
    };
    
}

void jm(json &resp_msg,json &msg,std::string &arg){
    std::string JMDir = config["Bot"]["Download_Dir"].value_or<std::string>("bot") + "/pdf/" + arg;
    YAML::Node options;
    options["plugins"]["after_album"][0]["plugin"] = "img2pdf";
    options["plugins"]["after_album"][0]["kwargs"]["pdf_dir"] = JMDir;
    options["plugins"]["after_album"][0]["kwargs"]["filename_rule"] = "Aname";
    // options["plugins"]["after_album"][0]["kwargs"]["encrypt"]["password"] = "nyanyanya";
    std::ofstream jm_option_file("jm_option.yml");
    jm_option_file.clear();
    jm_option_file << options;
    jm_option_file.close();
    fs::create_directories(JMDir);
    std::string jm_command = "jmcomic --option jm_option.yml " + arg;
    if ( (std::system(jm_command.data())) !=0 ){
        resp_msg["message"][0]["type"] = "text";
        resp_msg["message"][0]["data"]["text"] = "hentai！一天到晚看本子真是没救了喵";
        return;
    };
    std::string filename = fs::path((*std::filesystem::directory_iterator(JMDir))).filename().string();
    resp_msg["file_name"] = filename;
    resp_msg["file_uri"] = "file://" + JMDir + "/" + filename;
    UpLoadFile(resp_msg);
    
    
    
}
/**
void get_file(json &resp_msg,json &msg,std::string &arg){
    if (msg["message_type"]=="group"){
        resp_msg["action"] = "upload_group_file";
    } else {
        resp_msg["action"] = "upload_private_file";
    }
    resp_msg.erase("message");
    resp_msg.erase("message_type");
    
    std::string filename = fs::path(arg).filename().string();
    resp_msg["file_name"] = filename;
    resp_msg["file_uri"] = arg;
}
**/
void ProcessMsg(json &msg){
    std::string command,arg;
    std::unordered_map<std::string, std::function<void(json&,json&,std::string&)>> commands;
    commands["about"] = about;
    commands["jm"] = jm;
    commands["下载视频"] = DownloadVideo;
    commands["下载音频"] = DownloadAudio;
//    commands["get_file"] = get_file;
    json resp_msg;
    if (msg["data"].contains("group")){
        resp_msg["group_id"] = msg["data"]["group"]["group_id"];
    } else if(msg["data"].contains("friend")) {
        resp_msg["user_id"] = msg["data"]["friend"]["user_id"];
    }
    if (msg["data"]["segments"][0]["type"] == "text"){
        auto raw_message = std::string(msg["data"]["segments"][0]["data"]["text"]);
        if (raw_message[0] == '>'){
            size_t first_space = raw_message.find(" ");
            if (first_space != std::string::npos){
                command = raw_message.substr(1,first_space-1);
                arg = raw_message.substr(first_space+1);
            } else {
                command = raw_message.substr(1);
                arg = "";
            }
            commands[command](resp_msg,msg,arg);
        }
    } else if (msg["data"]["segments"][0]["type"] == "light_app"){
        std::cout << "小程序！\n" ;
        auto json_msg_data = nlohmann::json::parse(std::string(msg["data"]["segments"][0]["data"]["json_payload"]));
        std::cout<< json_msg_data.dump(4) << "\n";
        std::string url = cpr::Get(cpr::Url{json_msg_data["meta"]["detail_1"]["qqdocurl"]}).url.str();
        std::cout << url << "\n";
        url = url.substr(0,url.find("?"));
        std::cout << url << "\n";
        resp_msg["message"][0]["type"] = "reply" ;
        resp_msg["message"][0]["data"]["message_seq"] = msg["data"]["message_seq"] ;
        resp_msg["message"][1]["type"] = "text" ;
        resp_msg["message"][1]["data"]["text"] = "标题： " + std::string(json_msg_data["meta"]["detail_1"]["desc"]) + "\n\n" + "链接： " + url;
        SendMsg(resp_msg);
    }
    
}

