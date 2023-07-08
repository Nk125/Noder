#pragma once
#include <httplib.h>
#include <parser.hpp>
#include <json.hpp>
#include <regex>

class RequestSender {
private:
	static std::string genRanStr(const size_t len) {
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> uid((unsigned char)(-1), (unsigned char)(0));

		std::string buf;
		std::generate_n(std::inserter(buf, buf.end()), len, [&]() -> char {return (char)(uid(mt)); });
		return buf;
	}

public:
	enum {
		UserAgent = 0,
		ContentType,
		Body
	};

	static void getHTTPRequest(std::string uri, nlohmann::json config) {
		urlparser u(uri);

		if (u.host_.empty()) return;

		httplib::Headers h{
			{"User-Agent", config[UserAgent]}
		};

		std::string finalurl = (u.protocol_.empty() ? "http" : u.protocol_) + "://" + u.host_, finalpath = (u.path_.empty() ? "/" : u.path_) + (u.query_.empty() ? "" : "?" + u.query_);

		while (true) {
			httplib::Client c(finalurl);

			c.set_keep_alive(true);
			c.set_follow_location(true);

			c.Get(finalpath, h);
		}
	}

	static void postHTTPRequest(std::string uri, nlohmann::json config) {
		urlparser u(uri);
		
		if (u.host_.empty()) return;

		u.ctype_ = config[ContentType];
		u.body_	= config[Body];

		std::string body, finalurl = (u.protocol_.empty() ? "http" : u.protocol_) + "://" + u.host_, finalpath = (u.path_.empty() ? "/" : u.path_) + (u.query_.empty() ? "" : "?" + u.query_);

		{
			std::regex regRule(R"(\{random([0-9]+)\})", std::regex_constants::icase);
			std::smatch rm;

			while (std::regex_search(u.body_, rm, regRule)) {
				body.append(rm.prefix());

				size_t size = std::stoi(rm[1]);

				body.append(genRanStr(size));

				u.body_ = rm.suffix();
			}

			body.append(u.body_);
		}

		httplib::Headers h{
			{"User-Agent", config[UserAgent]}
		};

		while (true) {
			httplib::Client c(finalurl);

			c.set_keep_alive(true);
			c.set_follow_location(true);

			c.Post(finalpath, h, body, u.ctype_);
		}
	}
};