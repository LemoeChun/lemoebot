#pragma once

#include <cpr/response.h>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

cpr::Response CallApi(std::string &api,nlohmann::json &msg);
cpr::Response SendMsg(nlohmann::json &msg);
cpr::Response UpLoadFile(nlohmann::json &msg);
