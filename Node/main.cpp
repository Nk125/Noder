//	Automatically added by CMake
//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <Include.pch>
#include <Server/Config.hpp>
#include <Server/RequestHandler.hpp>
#include <Server/Threading.hpp>
#include <Server/Time.hpp>

// clear && clang++ -O1 -pthread -lssl -lcrypto -std=c++17 -o ./main main.cpp &&
// ./main

void initTimestamp() { Time::timer = Time::now(); }

void start() {
	httplib::Server sv;

	RequestHandler::setupServer(sv);

	Threading::threader = std::make_shared<Threading::TP>();

	try {
		initTimestamp();
		std::cout << "Obtained current time\n";
		std::cout << "Listening.\n";

		int port = 80;

		std::cout << "Designed Port: " << port << "\n";
		sv.listen("0.0.0.0", port);
	}
	catch (...) {
		std::cerr << "Failed at some point of the binding/listening\n";
		return;
	}
}

int main() {
	RequestSender::stopThreads = false;
	Configuration::loadConfig();

	while (true) {
		try {
			start();
		}
		catch (std::exception &e) {
			std::cerr << e.what() << "\n";
		}
		catch (...) {
			std::cerr << "Unhandled error in start()\n";
		}
	}
}