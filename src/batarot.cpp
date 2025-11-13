#include <fstream>
#include <nlohmann/json.hpp>
#include <print>
#include <string>

#include "util.hpp"
#include "milky_api.hpp"
#include "config.hpp"

using nlohmann::json;

std::string work_dir = config["Bot"]["Work_Dir"].value_or<std::string>("bot") + "/batarot";
const json batarots = json::parse(std::ifstream(work_dir + "/batarot.json"));
const json spread = json::parse(std::ifstream(work_dir + "/batarot_spread.json"));


void batarot(json &resp_msg, const json &msg, std::string &arg){
    std::println("work_dir: ",work_dir);
    if (arg == "") [[unlikely]] {
        std::string spreads{"牌阵列表:\n"};
        for (auto &[key, value] : spread.items()) {
            std::string represents;
            for(auto &represent : value["representations"]){
                for (auto &represent_ : represent){
                    represents.append("「" + represent_.get<std::string>() + "」");
                }
            }
            spreads.append(key + "\n" + represents + "\n\n");
        }
        resp_msg["message"][0]["type"] = "text";
        resp_msg["message"][0]["data"]["text"] =
            "使用: \n>塔罗牌 [数字(抽的塔罗牌数量)] \n"
             ">塔罗牌 [牌阵名字] \n" + spreads;
        SendMsg(resp_msg);
    }

}

