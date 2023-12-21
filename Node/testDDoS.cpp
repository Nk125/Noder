#include <atomic>
#include <httplib.h>
#include <iostream>
#include <map>
#include <mutex>

void usage() {
    std::cout << R"""(
        Usage:
            1st arg: Listen port
            2nd arg: Host (Default: 127.0.0.1)
    )""";
}

int main(int argc, char* argv[]) {
    std::string host;
    int port;
    
    if (argc <= 1) {
        usage();
        return 0;
    }

    host.assign((argc <= 2 ? std::string("127.0.0.1") : std::string(argv[2])));

    port = std::stoi(argv[1]);

    httplib::Server sv;

    std::map<std::string, std::atomic<unsigned long long>> requests = {};
    std::mutex m;

    sv.set_logger([&](const httplib::Request& req, const httplib::Response& res) {
        m.lock();
        std::cout << "\rRequest #" << (++requests.try_emplace({req.method, 0}).first->second);
        m.unlock();
    });

    sv.Get("/", [&](const httplib::Request& req, httplib::Response& res) {
        res.status = 200;
        res.set_content("Your request is the #" + std::to_string(requests["GET"]) +
        " (Global: " + std::to_string(requests["GET"] + requests["POST"]) + ")", "text/plain");
    });

    sv.Post("/", [&](const httplib::Request& req, httplib::Response& res) {
        res.status = 200;
        res.set_content("Your request is the #" + std::to_string(requests["POST"]) +
        " (Global: " + std::to_string(requests["GET"] + requests["POST"]) + ")", "text/plain");
    });

    std::cout << "Listening at: " << host << ":" << port << "\n";

    if (!sv.listen(host, port)) {
        std::cout << "Failed at listen\n";
    }
}