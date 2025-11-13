#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <yaml-cpp/yaml.h>

#include <filesystem>

#include "config.hpp"
#include "milky_api.hpp"


using json = nlohmann::json; 
namespace fs = std::filesystem; 

void about(json &resp_msg,const json &msg,std::string &arg){
    resp_msg["message"][0]["type"] = "text";
    resp_msg["message"][0]["data"]["text"] = "你好喵，这里是帕酱～";
    SendMsg(resp_msg);
}
void help(json &resp_msg,const json &msg,std::string &arg){
    resp_msg["message"][0]["type"] = "text";
    resp_msg["message"][0]["data"]["text"] =
        "命令提示符可以用`>`,`#`，`$`\n"
        "   `命令列表` \n"
        ">下载视频 [视频链接] \n"
        ">下载音频 [视频链接] \n"
        ">jm [神秘小数字] \n"
        ">ai [你的prompt] ";
    SendMsg(resp_msg);
}
void DownloadAudio(json &resp_msg,const json &msg,std::string &arg){
    std::string AudioDir = config["Bot"]["Work_Dir"].value_or<std::string>("bot") + "/audio/";
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

void DownloadVideo(json &resp_msg,const json &msg,std::string &arg){
    std::string VideoDir = config["Bot"]["Work_Dir"].value_or<std::string>("bot") + "/video/";
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
//        std::cout << thumb << "\n" << filename << "\n" <<fs::file_size(VideoDir + filename) << "\n" ;
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

void jm(json &resp_msg,const json &msg,std::string &arg){
    std::string JMDir = config["Bot"]["Work_Dir"].value_or<std::string>("bot") + "/pdf/" + arg;
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
void get_file(json &resp_msg,const json &msg,std::string &arg){
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

