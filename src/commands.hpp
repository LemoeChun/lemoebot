#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>

void about(nlohmann::json &resp_msg, const nlohmann::json &msg, std::string &arg);
void help(nlohmann::json &resp_msg, const nlohmann::json &msg, std::string &arg);
void DownloadVideo(nlohmann::json &resp_msg, const nlohmann::json &msg, std::string &arg);
void DownloadAudio(nlohmann::json &resp_msg, const nlohmann::json &msg, std::string &arg);
void jm(nlohmann::json &resp_msg, const nlohmann::json &msg, std::string &arg);
