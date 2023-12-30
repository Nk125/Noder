#pragma once
#include <parser.hpp>

class RequestSender {
private:
	static std::string genRanStr(const size_t);

#if SHOW_RPS
	static std::atomic<size_t> reqs;

	static void logRequest();

	static void setupLogger();
#endif

#if ENABLE_REGEX_IN_POST_REQUESTS
	static void regenBody(std::string&, std::string);
#endif

	// Host, Path, Body and Content Type
	using Options = std::tuple<std::string, std::string, std::string, std::string>;

	// URL
	template <bool>
	static Options parseOptions(const std::string&, urlparser&, const nlohmann::json&);

	template <bool>
	static void request(std::shared_ptr<httplib::Client>, Options);

	static std::shared_ptr<httplib::Client> makeClient(const std::string&, const httplib::Headers&);

public:
	static std::atomic<bool> stopThreads;

	enum {
		UserAgent = 0,
		ContentType,
		Body,
		UseRegex
	};

	static void getHTTPRequest(const std::string&, const nlohmann::json&);

	static void postHTTPRequest(const std::string&, const nlohmann::json&);
};