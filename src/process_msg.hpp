#pragma once

#include <nlohmann/json.hpp>
#include <toml++/toml.hpp>

using json = nlohmann::json;

void ProcessMsg(json &msg);
