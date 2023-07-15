#pragma once
#include <string>

struct MasserData {
    std::string url = "", uag = "", body = "", ctype = "";

    MasserData(std::string u, std::string ua, std::string b = "", std::string c = "") {
        url = u;
        uag = ua;
        body = b;
        ctype = c;
    }
};