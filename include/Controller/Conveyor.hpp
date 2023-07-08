#pragma once
#include <httplib.h>

class Conveyor {
private:
    std::shared_ptr<httplib::Client> http;

public:
    Conveyor(std::string url) {

    }
}