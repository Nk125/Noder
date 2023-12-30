//	Definition automatically added by CMake
//#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <Include.pch>
#include <Server/RequestHandler.hpp>
#include <Server/Threading.hpp>


// To compile:

// clear && clang++ -O1 -pthread -lssl -lcrypto -std=c++17 -o ./main main.cpp &&
// ./main

void initTimestamp() {
	Time::timer = Time::now();
}

void start(int port) {
	httplib::Server sv;

	RequestHandler::setupServer(sv);

	Threading::threader = std::make_shared<Threading::TP>();

	sv.new_task_queue = [&]() {
		Threading* thr = new Threading();

		if (thr->threader == nullptr) {
			std::cout << "Failed to create threader\n";
			std::exit(1);
		}

		return thr;
	};

	try {
		initTimestamp();
		std::cout << "Obtained current time\n";
		std::cout << "Listening.\n";

		std::cout << "Designed Port: " << port << "\n";
		sv.listen("0.0.0.0", port);
	}
	catch (...) {
		std::cerr << "Failed at some point of the binding/listening\n";
		return;
	}
}

int main() {
	Configuration::loadConfig();

	while (true) {
		try {
			start(Configuration::config["port"]);
		}
		catch (std::exception &e) {
			std::cerr << e.what() << "\n";
		}
		catch (...) {
			std::cerr << "Unhandled error in start()\n";
		}
	}
}