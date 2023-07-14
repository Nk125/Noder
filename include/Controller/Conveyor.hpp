#pragma once
#include <Controller/Translator.hpp>
#include <httplib.h>

class Conveyor {
private:
    std::shared_ptr<httplib::Client> http;

public:
    Conveyor(std::string url) {
        http = std::make_shared<httplib::Client>(url);
        http->set_keep_alive(true);
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
        httplib::Result res = http->Get("/server/check");

        if (!res) return false;

        return (Translator::checkStatus(res->body) && !res->body.empty() && res->get_header_value("Content-Type") != "text/plain");
    }

    bool Check() {
        httplib::Result res = http->Get("/server/check");

        if (!res) return false;

        return (Translator::checkStatus(res->body));
    }

    bool ReloadConfig() {
        httplib::Result res = http->Get("/server/config/reload");

        if (!res) return false;

        return (Translator::checkStatus(res->body));
    }

    std::string Uptime(bool human = false) {
        httplib::Result res = http->Get("/server/uptime");

        if (!res) return 0;

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
            return 0;
        }
    }

    std::string IP() {
        httplib::Result res = http->Get("/server/ip");

        if (!res) return 0;

        std::string body = res->body;

        if (Translator::checkStatus(body)) {
            return Translator::getIP(body);
        }
        else {
            return "";
        }
    }
};