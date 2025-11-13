#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>

using nlohmann::json;

void about(json &resp_msg, const nlohmann::json &msg, std::string &arg);
void help(json &resp_msg, const nlohmann::json &msg, std::string &arg);
void DownloadVideo(json &resp_msg, const nlohmann::json &msg, std::string &arg);
void DownloadAudio(json &resp_msg, const nlohmann::json &msg, std::string &arg);
void jm(json &resp_msg, const nlohmann::json &msg, std::string &arg);
void batarot(json &resp_msg, const json &msg, std::string &arg);
