#include <algorithm>
#include <cctype>
#include <charconv>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>
#include <print>
#include <ranges>
#include <string>
#include <system_error>
#include <vector>

#include "util.hpp"
#include "milky_api.hpp"
#include "config.hpp"

using nlohmann::json;
constexpr std::string_view fwd_msg_tmpl = R"(
{{
    "user_id":{0},
    "sender_name":"{1}",
    "segments" : [
        {{"type": "text",
            "data" : {{
                "text":"{2}"
            }}
        }},
        {{"type": "image",
            "data" : {{
                "uri":"{3}",
                "summary":"[塔罗牌]",
                "sub_type": "normal"
            }}
        }}
    ]
}}
                )";
constexpr std::string_view card_tmpl = R"({0}({1})({2})\n「{3}」\n\n“{4}”)";
void batarot(json &resp_msg, const json &msg, std::string &arg){
    std::string work_dir = WORK_DIR + "/batarot";
    const json batarots = json::parse(std::ifstream(work_dir + "/batarot.json"));
    const json spread = json::parse(std::ifstream(work_dir + "/batarot_spread.json"));
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
    } else {
        resp_msg["message"][0]["type"] = "forward";
        json forward_msgs;
        auto GetTarots = [&](const int &n){
            std::vector<json> cards;
            for([[maybe_unused]] int i : std::ranges::views::iota(0,n)){
                json card = batarots["cards"][random(21)];
                cards.push_back(card);
           }
            return cards;
        };
        if(std::ranges::all_of(arg,[](char c) { return std::isdigit(c);})) {
            int number;
            { auto [ptr,ec] = std::from_chars(arg.data(),arg.data() + arg.size(),number);
              if (ec == std::errc::result_out_of_range) return;
              if (number > 15) return;
            }
            std::vector<json> cards = GetTarots(number);
            for (auto card : cards){
                std::string pos = (random(0,1)) ? "up" : "down";
                int descrip_size = card["description"].size();
                std::string card_descrip = std::format(
card_tmpl,
card["name_cn"].get<std::string>(),
card["name_en"].get<std::string>(),
(pos == "up") ? "顺位" : "逆位",
card["meaning"][pos].get<std::string>(),
card["description"][random(descrip_size-1)].get<std::string>()
                );
                std::string img_uri = "file://" + work_dir + "/image/" + card["pic"].get<std::string>() + ".png";
                forward_msgs.push_back(json::parse(
std::format(fwd_msg_tmpl,
            BOT_ID,
            BOT_NAME,
            card_descrip,
            img_uri)));
            }
        }
        else {
            return;
        }
        resp_msg["message"][0]["data"]["messages"] = forward_msgs;
        SendMsg(resp_msg);
    }

}

