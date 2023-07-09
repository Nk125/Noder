#pragma once
#include <httplib.h>
#include <parser.hpp>
#include <json.hpp>
#include <regex>
#include <Server/Threading.hpp>

class RequestSender {
private:
	static std::string genRanStr(const size_t len) {
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> uid(0, 127);

		std::string buf;
		std::generate_n(std::inserter(buf, buf.end()), len, [&]() -> char {return (char)(uid(mt)); });
		return buf;
	}

	static void regenBody(std::string& body, std::string originalBody) {
		{
			body.clear();

			std::regex regRule(R"(\{random([0-9]+)\})", std::regex_constants::icase);
			std::smatch rm;

			while (std::regex_search(originalBody, rm, regRule)) {
				body.append(rm.prefix());

				size_t size = std::stoi(rm[1]);

				body.append(genRanStr(size));

				originalBody = rm.suffix();
			}

			body.append(originalBody);
		}
	}

	static void request(std::string url, std::string path, httplib::Headers h, bool post = false, std::string body = "", std::string ctype = "") {
		httplib::Client c(url);

		c.set_keep_alive(true);
		c.set_follow_location(true);

		if (post) {
			c.Post(path, h, body, ctype);
		}
		else {
			c.Get(path, h);
		}
	}

public:
	static bool stopThreads;

	enum {
		UserAgent = 0,
		ContentType,
		Body
	};

	static void getHTTPRequest(std::string uri, nlohmann::json config) {
		urlparser u(uri);

		if (u.host_.empty()) return;

		httplib::Headers h{
			{"User-Agent", config[std::to_string(UserAgent)]}
		};

		std::string finalurl = (u.protocol_.empty() ? "http" : u.protocol_) + "://" + u.host_, finalpath = (u.path_.empty() ? "/" : u.path_) + (u.query_.empty() ? "" : "?" + u.query_);

		while (!stopThreads) {
			Threading::threader->push_task(RequestSender::request, finalurl, finalpath, h, false, "", "");
		}
	}

	static void postHTTPRequest(std::string uri, nlohmann::json config) {
		urlparser u(uri);
		
		if (u.host_.empty()) return;

		u.ctype_ = config[std::to_string(ContentType)];
		u.body_	= config[std::to_string(Body)];

		std::string body, finalurl = (u.protocol_.empty() ? "http" : u.protocol_) + "://" + u.host_, finalpath = (u.path_.empty() ? "/" : u.path_) + (u.query_.empty() ? "" : "?" + u.query_);

		httplib::Headers h{
			{"User-Agent", config[std::to_string(UserAgent)]}
		};

		while (!stopThreads) {
			regenBody(body, u.body_);
			Threading::threader->push_task(RequestSender::request, finalurl, finalpath, h, true, body, u.ctype_);
		}
	}
};

bool RequestSender::stopThreads = false;