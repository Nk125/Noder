#include <Server/RequestSender.hpp>
#include <Server/Threading.hpp>
#include <Server/RequestHandler.hpp>

void RequestHandler::silentError(httplib::Response &res) {
    res.set_header("Location", "/");
    res.status = 200;
    res.set_content("", "text/plain");
}

void RequestHandler::genericResponse(httplib::Response &res, std::string descriptor, int status_code = 200, bool error = false, int error_num = 0) {
    nlohmann::json r = {
        {"status", !error}
    };

    if (error) {
        r["error"] = descriptor;
        r["error_code"] = error_num;
    }
    else {
        r["desc"] = descriptor;
    }

    res.status = status_code;
    res.set_content(r.dump(), "application/json");
    return;
}

bool RequestHandler::checkAuthorization(const httplib::Request &req, httplib::Response &res) {
    if (req.get_header_value("Token") == Configuration::config["token"].template get<std::string>()) {
        return true;
    }

    silentError(res);
    return false;
}

void RequestHandler::genericError(httplib::Response &res, std::string descriptor, int error_num, int status_code = 400) {
    genericResponse(res, descriptor, status_code, true, error_num);
}

void RequestHandler::requestResponse(const httplib::Request &req, httplib::Response &res, int type) {
    nlohmann::json r;
    if (req.get_header_value("Content-Type") != "application/json") {
        genericError(res, "Invalid content type", Error::InvContentType);
        return;
    }

    if (req.body.empty()) {
        genericError(res, "Body is empty", Error::EmptyBody);
        return;
    }

#if ENABLE_REGEX_IN_POST_REQUESTS
        bool useRegex;
#endif

    std::string url, useragent, path, body, contenttype;
    nlohmann::json j;

    try {
        j = nlohmann::json::parse(req.body);

        if (!j.at("url").is_string()) {
            genericError(res, "Invalid body", Error::InvalidBody);
            return;
        }
        else
            url = j.at("url");

        if (!j.at("useragent").is_string()) {
            genericError(res, "Invalid body", Error::InvalidBody);
            return;
        }
        else
            useragent = j.at("useragent");
    }
    catch (...) {
        genericError(res, "Invalid body", Error::InvalidBody);
        return;
    }

    nlohmann::json config = {
        {std::to_string(RequestSender::UserAgent), useragent}
    };

    switch (type) {
    case GET:
    {
        Threading::id++;

        std::cout << "New requester: GET to " << url << " id: " << Threading::id << "\n";

        RequestSender::stopThreads = false;
    }
    break;
    case POST:
    {
        try {
            if (!j.at("body").is_string()) {
                genericError(res, "Body param doesn't have a valid value", Error::InvalidBody);
                return;
            }
            else
                body = j.at("body");

            if (!j.at("contenttype").is_string()) {
                genericError(res, "Content Type param doesn't have a valid value", Error::InvalidBody);
                return;
            }
            else
                contenttype = j.at("contenttype");

#if ENABLE_REGEX_IN_POST_REQUESTS
            if (!j.at("useregex").is_boolean()) {
                genericError(res, "Use regex doesn't have a valid value", Error::InvalidBody);
                return;
            }
            else
                useRegex = j.at("useregex");
#endif
        }
        catch (...) {
            genericError(res, "Invalid body", Error::InvalidBody);
            return;
        }

        config[std::to_string(RequestSender::ContentType)] = contenttype;
        config[std::to_string(RequestSender::Body)] = body;

#if ENABLE_REGEX_IN_POST_REQUESTS
        config[std::to_string(RequestSender::UseRegex)] = useRegex;
#endif

        Threading::id++;

        std::cout << "New requester: POST to " << url << " id: " << Threading::id << " body: " << body << "\n";

        RequestSender::stopThreads = false;
    }
    break;
    default:
        genericError(res, "Not implemented yet", Error::NotImplemented, 501);
        return;
        break;
    }

    for (size_t i = 0; i < CONNECTIONS; i++) {
#if USE_THREAD_POOL_FOR_INIT
        Threading::threader->detach_task([&]() {
            switch (type) {
            case GET:
            {
                RequestSender::getHTTPRequest(url, config);
            }
            case POST:
            {
                RequestSender::postHTTPRequest(url, config);
            }
            }
        });
#else
        try {
            switch (type) {
            case GET:
            {
                std::thread(RequestSender::getHTTPRequest, url, config).detach();
            }
            case POST:
            {
                std::thread(RequestSender::postHTTPRequest, url, config).detach();
            }
            }
        }
        catch (...) {
            genericError(res, "Failed to detach thread", Error::Thread, 500);
            return;
        }
#endif
        }

        genericResponse(res, "Ok, initialized requester");
        return;
}

std::string RequestHandler::getIP() {
    httplib::Client c("https://api.ipify.org");
    httplib::Result r = c.Get("/");
    return (r ? r->body : "0.0.0.0");
}

void RequestHandler::setupServer(httplib::Server &sv) {
    sv.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        if (req.path != "/") {
            if (!checkAuthorization(req, res)) return httplib::Server::HandlerResponse::Handled;
        }

        return httplib::Server::HandlerResponse::Unhandled;
    });

    sv.Get(URL::POST, [](const httplib::Request &req, httplib::Response &res) {
			genericError(res, "Invalid Method", InvalidMethod);
        });

    sv.Get(URL::GET, [](const httplib::Request &req, httplib::Response &res) {
			genericError(res, "Invalid Method", InvalidMethod);
        });

    sv.Post(URL::GET, [](const httplib::Request &req, httplib::Response &res) {
			requestResponse(req, res, GET);
        });

    sv.Post(URL::POST, [](const httplib::Request &req, httplib::Response &res) {
			requestResponse(req, res, POST);
        });

    sv.Get(URL::SERVER::CHECK, [](const httplib::Request &req, httplib::Response &res) {
			genericResponse(res, "Online");
        });

    sv.Get(URL::SERVER::IP, [](const httplib::Request &req, httplib::Response &res) {
			genericResponse(res, getIP());
        });

    sv.Get(URL::SERVER::UPTIME, [](const httplib::Request &req, httplib::Response &res) {
			Time::timePoint moment = Time::now();

			genericResponse(res, std::to_string(Time::diff(moment, Time::timer).count()));
        });

    sv.Get(URL::SERVER::CONFIG::RELOAD, [](const httplib::Request &req, httplib::Response &res) {
			genericResponse(res, "Reloading config");

			Configuration::loadConfig();
        });

    sv.Get(URL::SERVER::RESTART, [&](const httplib::Request &req, httplib::Response &res) {
			genericResponse(res, "Restarting...");

			RequestSender::stopThreads = true;
			Threading::threader->purge();
			sv.stop();
        });

    sv.Get(URL::SERVER::KILL, [&](const httplib::Request &req, httplib::Response &res) {
			genericResponse(res, "Killing...");

			Threading::threader->purge();
			sv.stop();
			std::exit(0);
        });

    sv.Get("/", [](const httplib::Request &req, httplib::Response &res) {
			res.status = 200;
			res.set_content("Hi!", "text/plain");
        });

    /*
    While the server is robust, any thread exception or things like that will be safely caught here
    Also, the controller has these kind of exceptions acknowledged so you're going to see an error,
    not so verbose but an advice that something isn't good at least.
    */
    sv.set_exception_handler([](const httplib::Request &req, httplib::Response &res, std::exception_ptr ep) {
			res.status = 200;
			res.set_content("Hi!", "text/plain");
        });
}
