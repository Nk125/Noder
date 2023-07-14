#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <Controller/rang.hpp>
#include <string>
#include <vector>

class User {
private:
	struct Function {
		std::function<void()> Executor;
		std::string Name, Descriptor;
	};

	std::vector<Function> functions;

	void Selector() {
		while (true) {
			int opt = 0;

			std::cout << "Select: ";
			std::cin >> opt;
			std::cout << "\n\n";

			try {
				if (std::cin.fail()) {
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					
					throw std::exception();

					continue;
				}

				std::cin.ignore();

				functions.at(opt).Executor();
			}
			catch (...) {
				std::cout << "Invalid option\n";
			}
		}
	}

	void Help() {
		std::cout << "Help (enter the number to select):\n";

		for (size_t i = 0; i < functions.size(); i++) {
			const Function& func = functions[i];
			std::cout << rang::fgB::cyan << (i) << rang::fg::reset << ".- " 
				<< func.Name << ":\n  " << func.Descriptor << "\n\n";
		}
	}

public:
	User() {
		Function help, exit;

		help.Descriptor = "This help";
		help.Name = "Help";
		help.Executor = [&]() {
			this->Help();
		};

		exit.Descriptor = "Close the program";
		exit.Name = "Exit";
		exit.Executor = []() {
			std::cout << rang::fg::red << ("Closing") << rang::fg::reset << ("...\n");
			std::exit(0);
		};

		functions.push_back(help);
		functions.push_back(exit);
	}

	void RegisterFunction(std::string name, std::string desc, std::function<void()> f) {
		Function newFunc;
		
		newFunc.Descriptor = desc;
		newFunc.Name = name;
		newFunc.Executor = f;
		
		functions.push_back(newFunc);
	}

	void Welcome(std::string name) {
		std::cout << "Welcome to " << rang::fg::cyan << (name) << rang::fg::reset << "!\n\n";

		Help();

		Selector();
	}

	static void Notify(std::string content, std::string prefix = "Unknown") {
		std::cout << "\n[" << rang::fgB::yellow << (prefix) << rang::fg::reset 
			<< "]" << ": " << content << "\n";
	}

	static void Warn(std::string content) {
		Notify(content, "Warning");
	}

	static void Error(std::string description, std::string mod = "Unknown", bool fatal = false) {
		std::cerr << "\n[" << rang::fgB::red << ("Error") << rang::fg::reset << " (" 
			<< rang::fgB::yellow << mod << rang::fg::reset << ")]" 
			<< ": " << description << "\n";

		if (fatal) std::exit(1);
	}

	static bool Request(std::string& user_input, std::string msg) {
		std::cout << msg << ": ";
		std::getline(std::cin, user_input);
	
		std::cout << "\n";
		
		return (user_input != "cancel");
	}
};