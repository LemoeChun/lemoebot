#include "milky_api.hpp"

#include <cpr/api.h>
#include <cpr/cpr.h>

void PostMsg(std::string &url, std::string &api, nlohmann::json &msg){
    cpr::Post(url);
}
