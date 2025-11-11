#include <cpr/api.h>
#include <cpr/cpr.h>
#include <cpr/cprtypes.h>
#include <cpr/response.h>
#include <HXLibs/log/Log.hpp>

#include <string>

#include "milky_api.hpp"
#include "config.hpp"

auto json_header = cpr::Header{{"Content-Type", "application/json"}};
const std::string api_base_url = config["Bot"]["Milky_API_URL"].value_or<std::string>("http://127.0.0.1:3100/api");

cpr::Response CallApi(std::string &api,nlohmann::json &msg){
    std::string api_url(api_base_url + api);
    HX::log::hxLog.info("将向 ",api_url, " 发送：\n",msg.dump(4));
    return cpr::Post(cpr::Url{api_url},json_header,cpr::Body{msg.dump()});
}

cpr::Response SendMsg(nlohmann::json &msg){
    std::string api_url = api_base_url;
    if (msg.contains("group_id")) api_url.append("/send_group_message");
    if (msg.contains("user_id")) api_url.append("/send_private_message");
    HX::log::hxLog.info("将向 ",api_url, " 发送：\n",msg.dump(4));
    return cpr::Post(cpr::Url{api_url},json_header,cpr::Body{msg.dump()});
}

cpr::Response UpLoadFile(nlohmann::json &msg){
    std::string api_url = api_base_url;
    if (msg.contains("group_id")) api_url.append("/upload_group_file");
    if (msg.contains("user_id")) api_url.append("/upload_private_file");
    HX::log::hxLog.info("将向 ",api_url, " 发送：\n",msg.dump(4));
    return cpr::Post(cpr::Url{api_url},json_header,cpr::Body{msg.dump()});
}
