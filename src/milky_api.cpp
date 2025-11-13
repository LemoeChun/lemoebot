#include <cpr/api.h>
#include <cpr/cpr.h>
#include <cpr/cprtypes.h>
#include <cpr/response.h>
#include <HXLibs/log/Log.hpp>

#include <string>

#include "milky_api.hpp"
#include "config.hpp"

using nlohmann::json;

auto json_header = cpr::Header{{"Content-Type", "application/json"}};
const std::string api_base_url = config["Bot"]["Milky_API_URL"].value_or<std::string>("http://127.0.0.1:3100/api");

cpr::Response CallApi(std::string &api,json &msg){
    std::string api_url(api_base_url + api);
    HX::log::hxLog.info("将向 ",api_url, " 发送：\n",msg.dump(4));
    return cpr::Post(cpr::Url{api_url},json_header,cpr::Body{msg.dump()});
}

cpr::Response SendMsg(json &msg){
    std::string api_url = api_base_url;
    if (msg.contains("group_id")) api_url.append("/send_group_message");
    if (msg.contains("user_id")) api_url.append("/send_private_message");
    if (msg["message"][0]["type"] == "text" 
            && msg["message"][0]["data"]["text"].get<std::string>().length() > 600 ) {
        msg["message"][0]["data"]["messages"][0]["segments"] = msg["message"];
        msg["message"][0]["type"] = "forward";
        msg["message"][0]["data"].erase("text");
        msg["message"][0]["data"]["messages"][0]["user_id"] = config["Bot"]["QQ_ID"].value_or<uint>(1);
        msg["message"][0]["data"]["messages"][0]["sender_name"] = config["Bot"]["QQ_Name"].value_or<std::string>("喵呜");
    }
    HX::log::hxLog.info("将向 ",api_url, " 发送：\n",msg.dump(4));
    return cpr::Post(cpr::Url{api_url},json_header,cpr::Body{msg.dump()});
}

cpr::Response UpLoadFile(json &msg){
    std::string api_url = api_base_url;
    if (msg.contains("group_id")) api_url.append("/upload_group_file");
    if (msg.contains("user_id")) api_url.append("/upload_private_file");
    HX::log::hxLog.info("将向 ",api_url, " 发送：\n",msg.dump(4));
    return cpr::Post(cpr::Url{api_url},json_header,cpr::Body{msg.dump()});
}
