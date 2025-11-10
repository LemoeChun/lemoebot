#include <cpr/api.h>
#include <cpr/cpr.h>
#include <cpr/cprtypes.h>
#include <cpr/response.h>
#include <string>

#include "milky_api.hpp"
#include "config.hpp"

auto json_header = cpr::Header{{"Content-Type", "application/json"}};
std::string api_url = config["Bot"]["Millky_API_URL"].value_or<std::string>("/send_group_message");

cpr::Response CallApi(std::string &api,nlohmann::json &msg){
    api_url.append(api);
    return cpr::Post(cpr::Url{api_url},json_header,cpr::Body{msg.dump()});
}

cpr::Response SendMsg(nlohmann::json &msg){
    if (msg.contains("group_id")) api_url.append("/send_group_message");
    if (msg.contains("user_id")) api_url.append("/send_private_message");
    return cpr::Post(cpr::Url{api_url},json_header,cpr::Body{msg.dump()});
}

cpr::Response UpLoadFile(nlohmann::json &msg){
    if (msg.contains("group_id")) api_url.append("/send_group_message");
    if (msg.contains("user_id")) api_url.append("/send_private_message");
    return cpr::Post(cpr::Url{api_url},json_header,cpr::Body{msg.dump()});
}
