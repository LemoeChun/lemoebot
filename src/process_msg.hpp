#pragma once

#include <nlohmann/json.hpp>
#include <toml++/toml.hpp>

using json = nlohmann::json;

json ProcessMsg(toml::table &config,json &msg);
