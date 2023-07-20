#pragma once
#include <Server/Time.hpp>
#include <httplib.h>
#include <parser.hpp>
#include <json.hpp>
#if ENABLE_REGEX_IN_POST_REQUESTS
#include <regex>
#endif
#if USE_THREAD_POOL_FOR_REQUESTER
#include <Server/Threading.hpp>
#else
#include <thread>
#endif

namespace Timer {
        Time::timePoint tp;
}

class RequestSender {
private:
        static std::atomic<size_t> reqs;

	static std::string genRanStr(const size_t len) {
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> uid(0, 127);

		std::string buf;
		std::generate_n(std::inserter(buf, buf.end()), len, [&]() -> char {return (char)(uid(mt)); });
		return buf;
	}

        static void logRequest() {
		reqs++;
		Time::timePoint now = Time::now();
		float secs = (Time::diff(Timer::tp, now).count()) / 1000;
		if (secs % 15 == 0) {
			std::cout << "\rAvg RPS: " << float(float(reqs) / float(secs));
		}
	}

        static void setupLogger() {
		reqs = 0;
		Timer::tp = Time::now();
	}

	#if ENABLE_REGEX_IN_POST_REQUESTS
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
	#endif

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
		Body,
		UseRegex
	};

	static void getHTTPRequest(std::string uri, nlohmann::json config) {
		setupLogger();
		urlparser u(uri);

		if (u.host_.empty()) return;

		httplib::Headers h{
			{"User-Agent", config[std::to_string(UserAgent)]}
		};

		std::string finalurl = (u.protocol_.empty() ? "http" : u.protocol_) + "://" + u.host_, finalpath = (u.path_.empty() ? "/" : u.path_) + (u.query_.empty() ? "" : "?" + u.query_);

		while (!stopThreads) {
			#if USE_THREADING_IN_REQUESTS
            try {
				#if USE_THREAD_POOL_FOR_REQUESTER
                Threading::threader->push_task
				#else
				std::thread t
				#endif

				(RequestSender::request, finalurl, finalpath, h, false, std::string(""), std::string(""));

				#if !USE_THREAD_POOL_FOR_REQUESTER
				t.detach();
				#endif
            }
            catch (...) {
                continue;
            }
			#else
			RequestSender::request(finalurl, finalpath, h);
			#endif

			logRequest();
		}
	}

	static void postHTTPRequest(std::string uri, nlohmann::json config) {
		setupLogger();
		urlparser u(uri);
		
		if (u.host_.empty()) return;

		u.ctype_ = config[std::to_string(ContentType)];
		u.body_	= config[std::to_string(Body)];

		std::string body, finalurl = (u.protocol_.empty() ? "http" : u.protocol_) + "://" + u.host_, finalpath = (u.path_.empty() ? "/" : u.path_) + (u.query_.empty() ? "" : "?" + u.query_);

		httplib::Headers h{
			{"User-Agent", config[std::to_string(UserAgent)]}
		};

		bool useRgx = config[std::to_string(UseRegex)];

		#if !ENABLE_REGEX_IN_POST_REQUESTS
		if (!useRgx) {
			body = u.body_;
		}
		#endif

		while (!stopThreads) {
			#if ENABLE_REGEX_IN_POST_REQUESTS
			if (useRgx) {
				regenBody(body, u.body_);
			}
			#endif

			#if USE_THREADING_IN_REQUESTS
            try {
				#if USE_THREAD_POOL_FOR_REQUESTER
                Threading::threader->push_task
				#else
				std::thread t
				#endif

				(RequestSender::request, finalurl, finalpath, h, true, body, u.ctype_);

				#if !USE_THREAD_POOL_FOR_REQUESTER
				t.detach();
				#endif
            }
            catch (...) {
                continue;
            }
			#else
			RequestSender::request(finalurl, finalpath, h, true, body, u.ctype_);
			#endif

			logRequest();
		}
	}
};

bool RequestSender::stopThreads = false;
std::atomic<size_t> RequestSender::reqs = 0;
