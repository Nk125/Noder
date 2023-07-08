#pragma once
#include <Server/CONFIG.h>
#include <fstream>
#include <json.hpp>

namespace Configuration {
    nlohmann::json config;

    void loadConfig() {
        nlohmann::json defaultConfig {
            {"token", "default"}
        };

	    try {
		    std::ifstream f(CONFIG_FILE);
		    config = nlohmann::json::parse(f);
		    f.close();
	    }
	    catch (...) {
		    std::cerr << "Failed loading config, using default values\n";
            config = defaultConfig;
		    return;
	    }
    }
}