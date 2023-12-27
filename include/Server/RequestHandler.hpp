#pragma once
#include <Include.pch>
#include <Server/Config.hpp>
#include <Server/RequestSender.hpp>
#include <Server/Threading.hpp>
#include <Server/Time.hpp>

class RequestHandler {
private:
	enum Error {
		InvContentType = 1,
		NotImplemented,
		EmptyBody,
		InvalidBody,
		InvalidMethod,
		Thread
	};

	enum {
		GET = 0,
		POST
	};

	static void silentError(httplib::Response& res) {
		res.set_header("Location", "/");
		res.status = 200;
		res.set_content("", "text/plain");
	}

	static void genericResponse(httplib::Response& res, std::string descriptor, int status_code = 200, bool error = false, int error_num = 0) {
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

	static bool checkAuthorization(const httplib::Request& req, httplib::Response& res) {
		if (req.get_header_value("Token") == Configuration::config["token"]) {
			return true;
		}

		silentError(res);
		return false;
	}

	static void genericError(httplib::Response& res, std::string descriptor, int error_num, int status_code = 400) {
		genericResponse(res, descriptor, status_code, true, error_num);
	}

	static void requestResponse(const httplib::Request& req, httplib::Response& res, int type) {
		nlohmann::json r;
		if (req.get_header_value("Content-Type") != "application/json") {
			genericError(res, "Invalid content type", Error::InvContentType);
			return;
		}

		if (req.body.empty()) {
			genericError(res, "Body is empty", Error::EmptyBody);
			return;
		}

		std::string url, useragent, path;
		nlohmann::json j;

		try {
			j = nlohmann::json::parse(req.body);

			if (!j.at("url").is_string()) {
				genericError(res, "Invalid body", Error::InvalidBody);
				return;
			}
			else url = j.at("url");

			if (!j.at("useragent").is_string()) {
				genericError(res, "Invalid body", Error::InvalidBody);
				return;
			}
			else useragent = j.at("useragent");
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

			for (size_t i = 0; i < REQUESTER_LOOPS; i++) {
#if USE_THREAD_POOL_FOR_INIT
				Threading::threader->detach_task(RequestSender::getHTTPRequest, url, config);
#else
				try {
					std::thread(RequestSender::getHTTPRequest, url, config).detach();
				}
				catch (...) {
					genericError(res, "Failed to detach thread", Error::Thread, 500);
					return;
				}
#endif
			}

			genericResponse(res, "Ok, initialized GET requester");
			return;
		}
		break;
		case POST:
		{
			std::string body, contenttype;
			bool useRegex;

			if (!j.at("body").is_string()) {
				genericError(res, "Body param isn't a valid value", Error::InvalidBody);
				return;
			}
			else body = j.at("body");

			if (!j.at("contenttype").is_string()) {
				genericError(res, "Content Type param isn't a valid value", Error::InvalidBody);
				return;
			}
			else contenttype = j.at("contenttype");

			if (!j.at("useregex").is_boolean()) {
				genericError(res, "Content Type param isn't a valid value", Error::InvalidBody);
				return;
			}
			else useRegex = j.at("useregex");

			config[std::to_string(RequestSender::ContentType)] = contenttype;
			config[std::to_string(RequestSender::Body)] = body;
			config[std::to_string(RequestSender::UseRegex)] = useRegex;

			Threading::id++;

			std::cout << "New requester: POST to " << url << " id: " << Threading::id << " body: " << body << "\n";

			RequestSender::stopThreads = false;

			for (size_t i = 0; i < REQUESTER_LOOPS; i++) {
#if USE_THREAD_POOL_FOR_INIT
				Threading::threader->detach_task(RequestSender::postHTTPRequest, url, config);
#else
				try {
					std::thread(RequestSender::postHTTPRequest, url, config).detach();
				}
				catch (...) {
					genericError(res, "Failed to detach thread", Error::Thread, 500);
					return;
				}
#endif
			}

			genericResponse(res, "Ok, initialized POST requester");
			return;
		}
		break;
		default:
			genericError(res, "Not implemented yet", Error::NotImplemented, 501);
			return;
			break;
		}
	}

	static std::string getIP() {
		httplib::Client c("https://api.ipify.org");
		auto r = c.Get("/");
		return (r ? r->body : "0.0.0.0");
	}

public:
	static void setupServer(httplib::Server& sv) {
		sv.Get(URL::POST, [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			genericError(res, "Invalid Method", InvalidMethod);
			});

		sv.Get(URL::GET, [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			genericError(res, "Invalid Method", InvalidMethod);
			});

		sv.Post(URL::GET, [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			requestResponse(req, res, GET);
			});

		sv.Post(URL::POST, [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			requestResponse(req, res, POST);
			});

		sv.Get(URL::SERVER::CHECK, [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			genericResponse(res, "Online");
			});

		sv.Get(URL::SERVER::IP, [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			genericResponse(res, getIP());
			});

		sv.Get(URL::SERVER::UPTIME, [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			Time::timePoint moment = Time::now();

			genericResponse(res, std::to_string(Time::diff(moment, Time::timer).count()));
			});

		sv.Get(URL::SERVER::CONFIG::RELOAD, [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			genericResponse(res, "Reloading config");

			Configuration::loadConfig();
			});

		sv.Get(URL::SERVER::RESTART, [&](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			genericResponse(res, "Restarting...");

			RequestSender::stopThreads = true;
			Threading::threader->purge();
			sv.stop();
			});

		sv.Get(URL::SERVER::KILL, [&](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			genericResponse(res, "Killing...");

			Threading::threader->purge();
			sv.stop();
			std::exit(0);
			});

		sv.Get("/", [](const httplib::Request& req, httplib::Response& res) {
			res.status = 200;
			res.set_content("Hi!", "text/plain");
			});
	}
};
