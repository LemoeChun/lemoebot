#pragma once

#include <cpr/response.h>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

cpr::Response CallApi(std::string &api,nlohmann::json &msg);
cpr::Response SnedMsg(nlohmann::json &msg);
cpr::Response UpLoadFile(nlohmann::json &msg);
