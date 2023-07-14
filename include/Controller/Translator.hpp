#pragma once
#include <json.hpp>
#include <string>

class Translator {
public:
    static bool checkStatus(std::string json) {
        try {
            return (nlohmann::json::parse(json)["status"] == true);
        }
        catch (...) {
            return false;
        }
    }

    static size_t getUptime(std::string json) {
        try {
            return std::stoull(nlohmann::json::parse(json)["desc"].template get<std::string>());
        }
        catch (...) {
            return 0;
        }
    }

    static std::string getIP(std::string json) {
        try {
            return nlohmann::json::parse(json)["desc"];
        }
        catch (...) {
            return "";
        }
    }
};