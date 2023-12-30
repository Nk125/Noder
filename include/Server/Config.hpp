#pragma once

namespace Configuration {
	static nlohmann::json config ({
	    {"token", "default"},
	    {"port", 80}
    });

    static void loadConfig() {
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
		    return;
	    }
    }
}