#pragma once

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

	static void silentError(httplib::Response&);

	static void genericResponse(httplib::Response&, std::string, int, bool, int);

	static bool checkAuthorization(const httplib::Request&, httplib::Response&);

	static void genericError(httplib::Response&, std::string, int, int);

	static void requestResponse(const httplib::Request&, httplib::Response&, int);

	static std::string getIP();

public:
	static void setupServer(httplib::Server&);
};
