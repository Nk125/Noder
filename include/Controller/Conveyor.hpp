#pragma once
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <Controller/Defines.hpp>
#include <Controller/Translator.hpp>
#include <httplib.h>
#include <PATHS.h>

class Conveyor {
private:
    std::shared_ptr<httplib::Client> http;

    bool genericChecker(std::string uri) {
        httplib::Result res = http->Get(uri);

        if (!res) return false;

        return (Translator::checkStatus(res->body));
    }

public:
    enum Request {
        GET = 1,
        POST
    };

    Conveyor(std::string url, std::string token = "") {
        http = std::make_shared<httplib::Client>(url);
        http->set_keep_alive(true);

        if (!token.empty()) AddToken(token);
    }

    ~Conveyor() {
        http->stop();
        http.reset();
    }

    void AddToken(std::string token) {
        http->set_default_headers({
            {std::string("Token"), token}
        });
    }

    bool AuthorizationCheck() {
        httplib::Result res = http->Get(URL::SERVER::CHECK);

        if (!res) return false;

        return (!res->body.empty() && res->get_header_value("Content-Type") != "text/plain" && Translator::checkStatus(res->body));
    }

    bool Check() {
        return genericChecker(URL::SERVER::CHECK);
    }

    bool ReloadConfig() {
        return genericChecker(URL::SERVER::CONFIG::RELOAD);
    }

    bool Restart() {
        return genericChecker(URL::SERVER::RESTART);
    }

    bool Kill() {
        return genericChecker(URL::SERVER::KILL);
    }

    std::string Uptime(bool human = false) {
        httplib::Result res = http->Get(URL::SERVER::UPTIME);

        if (!res) return "0";

        std::string body = res->body;

        if (Translator::checkStatus(body)) {
            size_t s = Translator::getUptime(body);

            if (human) {
                size_t days, hours, minutes, seconds;
                s /= 1000;

                days = s / (24 * 3600);
  
                s = s % (24 * 3600);
                hours = s / 3600;
  
                s %= 3600;
                minutes = s / 60;
  
                s %= 60;
                seconds = s;

                return std::to_string(days) + "d " +
                std::to_string(hours) + "h " +
                std::to_string(minutes) + "m " +
                std::to_string(seconds) + "s";
            }
            else {
                return std::to_string(s);
            }
        }
        else {
            return "0";
        }
    }

    std::string IP() {
        httplib::Result res = http->Get(URL::SERVER::IP);

        if (!res) return "";

        std::string body = res->body;

        if (Translator::checkStatus(body)) {
            return Translator::getIP(body);
        }
        else {
            return "";
        }
    }

    bool Mass(size_t type, MasserData mdata) {
        httplib::Result res;

        switch (type) {
        case Request::GET:
        {
            res = http->Post(URL::GET, Translator::buildGETData(mdata), "application/json");
        }
            break;
        case Request::POST:
        {
            res = http->Post(URL::POST, Translator::buildPOSTData(mdata), "application/json");
        }
            break;
        default:
            return false;
            break;
        }

        if (res) {
            return Translator::checkStatus(res->body);
        }
        else return false;
    }
};