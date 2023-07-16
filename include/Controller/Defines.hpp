#pragma once
#include <string>

struct MasserData {
    std::string url = "", uag = "", body = "", ctype = "";
    bool useregex = false;

    MasserData(std::string u, std::string ua, std::string b = "", std::string c = "", bool rgx = false) {
        url = u;
        uag = ua;
        body = b;
        ctype = c;
        useregex = rgx;
    }
};