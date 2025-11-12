#include <nlohmann/json.hpp>
#include <string>

void about(const nlohmann::json &msg, std::string &arg);
void help(const nlohmann::json &msg, std::string &arg);
void DownloadVideo(const nlohmann::json &msg, std::string &arg);
void DownloadAudio(const nlohmann::json &msg, std::string &arg);
void jm(const nlohmann::json &msg, std::string &arg);
