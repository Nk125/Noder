#pragma once
#include <Include.pch>

namespace Configuration {
    nlohmann::json config;

    void loadConfig() {
        nlohmann::json defaultConfig {
            {"token", "default"}
        };

	    try {
			std::ifstream f(CONFIG_FILE);

			if (f.is_open()) {
				config = nlohmann::json::parse(f);
				f.close();
				return;
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