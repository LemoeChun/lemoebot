#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

json ProcessCommand(json &msg);
