#pragma once

namespace Configuration {
	static nlohmann::json config ({
	    {"token", "default"},
	    {"port", 80}
    });

    static void loadConfig() {
		nlohmann::json backup = config;

		std::cout << "Loading config\n";

	    try {
			std::ifstream f(CONFIG_FILE);

			if (f.is_open()) {
				config.merge_patch(nlohmann::json::parse(f));
				f.close();
			}
			else {
				throw std::exception();
			}
	    }
	    catch (...) {
		    std::cerr << "Failed loading config, using default values\n";
			config = backup;
		    return;
	    }

		// The try block is to prevent faulty default values
		try {
			// Test values
			if (!config["token"].is_string()) {
				std::cerr << "Token isn't a string, reverting to default value\n";
				config["token"] = backup["token"].template get<std::string>();
			}

			// Test if port is a number and convertible to unsigned short (0-65535)
			try {
				unsigned short test = config["port"].template get<unsigned short>();
			}
			catch (...) {
				std::cerr << "Port isn't a valid number, reverting to default value\n";
				config["port"] = backup["port"].template get<unsigned short>();
			}
		}
		catch (...) {
			std::cerr << "Failed loading config, closing\n";
		    std::exit(1);
			return;
		}

		std::cout << "Succesfully loaded config: " << config.dump(2) << "\n";
    }
}