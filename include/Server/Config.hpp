#pragma once
#include <fstream>
#include <json.hpp>

namespace Configuration {
    nlohmann::json config;

    void loadConfig() {
        nlohmann::json defaultConfig {
            {"token", "default"}
        };

	    try {
			std::ifstream f(CONFIG_FILE, std::ios::binary);

			if (f.is_open()) {
				config = nlohmann::json::parse(f);
				f.close();
			}
			else {
				throw std::exception();
			}
	    }
	    catch (...) {
		    std::cerr << "Failed loading config, using default values\n";
            config = defaultConfig;
		    return;
	    }
    }
}