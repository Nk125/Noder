#pragma once
#include <Controller/Defines.hpp>
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

    static std::string buildGETData(MasserData mdata) {
        nlohmann::json j = {
            {"url", mdata.url},
            {"useragent", mdata.uag}
        };

        return j.dump();
    }

    static std::string buildPOSTData(MasserData mdata) {
        nlohmann::json j = {
            {"url", mdata.url},
            {"useragent", mdata.uag},
            {"body", mdata.body},
            {"contenttype", mdata.ctype},
            {"useregex", mdata.useregex}
        };

        return j.dump();
    }
};