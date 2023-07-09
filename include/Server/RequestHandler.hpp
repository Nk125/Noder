#pragma once
#include <httplib.h>
#include <json.hpp>
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
		InvalidMethod
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

			Threading::threader->push_task(RequestSender::getHTTPRequest, url, config);

			genericResponse(res, "Ok, initialized GET requester");
			return;
		}
		break;
		case POST:
		{
			std::string body, contenttype;

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

			config[std::to_string(RequestSender::ContentType)] = contenttype;
			config[std::to_string(RequestSender::Body)] = body;

			Threading::id++;

			std::cout << "New requester: POST to " << url << " id: " << Threading::id << " body: " << body << "\n";

			RequestSender::stopThreads = false;

			Threading::threader->push_task(RequestSender::postHTTPRequest, url, config);

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
		sv.Get("/post", [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			genericError(res, "Invalid Method", InvalidMethod);
			});

		sv.Get("/get", [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			genericError(res, "Invalid Method", InvalidMethod);
			});

		sv.Post("/get", [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			requestResponse(req, res, GET);
			});

		sv.Post("/post", [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			requestResponse(req, res, POST);
			});

		sv.Get("/server/check", [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			genericResponse(res, "Online");
			});

		sv.Get("/server/ip", [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			genericResponse(res, getIP());
			});

		sv.Get("/server/uptime", [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			Time::timePoint moment = Time::now();

			genericResponse(res, std::to_string(Time::diff(moment, Time::timer).count()));
			});

		sv.Get("/server/config/reload", [](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			genericResponse(res, "Reloading config");

			Configuration::loadConfig();
			});

		sv.Get("/server/restart", [&](const httplib::Request& req, httplib::Response& res) {
			if (!checkAuthorization(req, res)) return;

			genericResponse(res, "Restarting...");

			RequestSender::stopThreads = true;
			Threading::threader->purge();
			sv.stop();
			});

		sv.Get("/", [](const httplib::Request& req, httplib::Response& res) {
			res.status = 200;
			res.set_content("Hi!", "text/plain");
			});
	}
};
