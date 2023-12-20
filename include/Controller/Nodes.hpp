#pragma once
#include <algorithm>
#include <Controller/Conveyor.hpp>
#include <Controller/Defines.hpp>
#include <Controller/rang.hpp>
#include <Controller/User.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

class Nodes {
private:
	enum {
		Unknown = 0,
		Unauthorized,
		EmptyURL,
		OutOfIndex
	};

	struct Node {
		std::string url = "", token = "";

		Node(std::string u, std::string t) {
			url = u;
			token = t;
		}

		Node() {
			url = "";
			token = "";
		}

		static nlohmann::json to_json(const Node& n) {
			return nlohmann::json{{"url", n.url}, {"token", n.token}};
		}

		static Node from_json(const nlohmann::json& j) {
			return Node(j.at("url"), j.at("token"));
		}
	};

	std::vector<Node> nodes;

	void printError(size_t err) {
		std::string modname = "Authorization";

		switch (err) {
			default:
			case Unknown:
			{
				User::Error("Unknown error", modname);
			}
			break;
			case Unauthorized:
			{
				User::Error("Unauthorized token", modname);
			}
			break;
			case EmptyURL:
			{
				User::Error("Node URL is empty", modname);
			}
			break;
			case OutOfIndex:
			{
				User::Error("Node id isn't on node list", modname);
			}
			break;
		}
	}

	bool easyNodeAuth(size_t id, std::string token = "") {
		try {
			internalAuthorize(id, token);

			return true;
		}
		catch (std::runtime_error& e) {
			try {
				printError(std::stoull(std::string(e.what())));
			}
			catch (...) {
				printError(Unknown);
			}

			return false;
		}
		catch (...) {
			printError(Unknown);
				
			return false;
		}

		return false;
	}

	size_t safeConvert(std::string id) {
		try {
			return std::stoull(id) - 1;
		}
		catch (...) {
			return (-1);
		}
	}

	void internalAuthorize(size_t nid, std::string token) {
		if (nid == size_t(-1)) {
			throw std::runtime_error(std::to_string(OutOfIndex));
			return;
		}

		Node* node;
		
		try {
			node = &nodes.at(nid);
		}
		catch (...) {
			throw std::runtime_error(std::to_string(OutOfIndex));
			return;
		}
		
		std::string nurl = node->url;
		
		if (nurl.empty()) {
			throw std::runtime_error(std::to_string(EmptyURL));
			return;
		}

		bool onlyCheck = token.empty();

		Conveyor nodetalk(nurl, (onlyCheck ? node->token : token));

		if (!nodetalk.AuthorizationCheck()) {
			throw std::runtime_error(std::to_string(Unauthorized));
		}
		else {
			if (onlyCheck) return;

			node->token = token;
		}
	}

	bool isNodeListEmpty() {
		bool nem = nodes.empty();

		if (nem) User::Error("There aren't any nodes registered yet!", "Node List");

		return nem;
	}

public:
	void Authorize() {
		if (isNodeListEmpty()) return;

		std::string token, modname = "Authorization";

		bool tokenIsDefault = (token == "default");

		if (!User::Request(token, "Insert the token for all nodes")) return;
		
		for (size_t nid = 1; nid <= nodes.size(); nid++) {
			if (!easyNodeAuth(nid - 1, token)) {
				User::Error("Node #" + std::to_string(nid) + " wasn't authorized", modname);
			}
			else {
				User::Notify("Node #" + std::to_string(nid) + " authorized!", modname);

				if (tokenIsDefault) {
					User::Warn("To have better security, try to change the access token of your node and reload config");
				}
			}
		}

		return;
	}

	void AuthorizeSingle() {
		if (isNodeListEmpty()) return;

		std::string modname = "Authorization";

		PrintList();

		std::cout << "Select " << rang::fg::cyan << "one" << rang::fg::reset << " ";

		std::string token, id;

		if (!User::Request(id, "of the nodes (insert node number)")) return;

		if (!User::Request(token, "Insert the token")) return;

		size_t nid = safeConvert(id);

		if (!easyNodeAuth(nid, token)) {
			User::Error("Node wasn't authorized", modname);
		}
		else {
			if (token == "default") {
				User::Warn("To have better security, try to change the access token of your node and reload config");
			}

			User::Notify("Node authorized!", modname);
		}

		User::Notify("Token succesfully authorized", modname);
		return;
	}

	void Add() {
		std::string url, token, modname = "Add node";

		if (!User::Request(url, "Insert the node URL")) return;

		if (!User::Request(token, "Insert the node auth token")) return;

		try {
			nodes.emplace_back(Node(url, token));

			User::Notify("Node succesfully added!", modname);
		}
		catch (...) {
			User::Error("Something failed inserting the node", modname);
			return;
		}
	}

	void Remove() {
		if (isNodeListEmpty()) return;

		std::string id, modname = "Remove node";

		if (!User::Request(id, "Insert the node number")) return;

		try {
			nodes.erase(nodes.begin() + (std::stoull(id) - 1));

			User::Notify("Node succesfully removed!", modname);
		}
		catch (...) {
			User::Error("Something failed removing the node", modname);
			return;
		}
	}

	void RemoveAll() {
		if (isNodeListEmpty()) return;

		std::string confirm, modname = "Remove all nodes";

		if (!User::Request(confirm, "Press enter to clear all nodes or type \"cancel\" to cancel the clear")) {
			User::Notify("Node clearing was cancelled", modname);
			return;
		}

		try {
			nodes.clear();

			User::Notify("Nodes succesfully cleared!", modname);
		}
		catch (...) {
			User::Error("Something failed removing the nodes", modname);
			return;
		}
	}

	void Import() {
		std::string file, modname = "Import";

		if (!User::Request(file, "Enter the file name to import nodes")) return;

		std::ifstream i(file);

		if (i.is_open()) {
			try {
				nlohmann::json j = nlohmann::json::parse(i);
				j = j.at("nodes");

				if (j.size() <= 0) {
					User::Warn("Node list is empty, stopping import");
					return;
				}

				std::transform(j.begin(), j.end(), std::back_inserter(nodes), Node::from_json);
			}
			catch (...) {
				User::Error("Something failed parsing file data", modname);
				return;
			}
		}
		else {
			User::Error("Fail opening the file", modname);
			return;
		}
		
		User::Notify("Nodes succesfully imported!", modname);
	}

	void Export() {
		if (isNodeListEmpty()) return;

		std::string file, modname = "Export";

		if (!User::Request(file, "Enter the file name to export nodes")) return;

		nlohmann::json j {
			{"nodes", nlohmann::json::array()}
		};

		nlohmann::json& arr = j["nodes"];

		std::transform(nodes.begin(), nodes.end(), std::inserter(arr, arr.end()), Node::to_json);

		std::ofstream o(file, std::ios::binary);

		if (o.is_open()) {
			std::string jdump = j.dump();
			o.write(jdump.c_str(), jdump.size());
			o.close();
		}
		else {
			User::Error("Failed to open file", modname);
			return;
		}

		User::Notify("File succesfully wrote!", modname);
	}

	void PrintList() {
		if (isNodeListEmpty()) return;

		std::cout << "List of " << rang::fg::cyan << "nodes" << rang::fg::reset << ": \n";
		size_t n = 0;

		for (Node& node : nodes) {
			bool emptyURL = node.url.empty(), emptyToken = node.token.empty();

			std::cout << ++n << ":\n  URL: " << rang::fg::cyan << (emptyURL ? "(Empty URL)" : node.url) << rang::fg::reset
			<< "\n  Token: "
			<< rang::fg::cyan << (emptyToken ? "(Empty Token)" : node.token) << rang::fg::reset
			<< "\n";
		}
	}

	void Check() {
		std::string modname = "Checker";
		if (isNodeListEmpty()) return;

		std::cout << "Checking " << rang::fg::yellow << "all" << rang::fg::reset << " nodes...\n";

		size_t nid = 0;
		for (Node& node : nodes) {
			if (!easyNodeAuth(nid)) {
				User::Error("Node #" + std::to_string(++nid) + " has an invalid token", modname);
				continue;
			}

			nid++;

			Conveyor nodetalk(node.url, node.token);

			bool online = nodetalk.Check();

			std::cout << nid << ":\n  [" << (online ? rang::fgB::green : rang::fgB::red) << (online ? "Online" : "Offline") << rang::fg::reset << "]\n";

			if (online) {
				std::string uptime = nodetalk.Uptime(true);
				bool emptyUptime = uptime.empty();
				
				std::cout << "  Uptime: " << (!emptyUptime ? rang::fg::cyan : rang::fg::red)
				<< (!emptyUptime ? uptime : "Failed to get uptime") << rang::fg::reset;

				std::string ip = nodetalk.IP();
				bool emptyIP = ip.empty();

				std::cout << "\n  IP: " << (!emptyIP ? rang::fg::cyan : rang::fg::red)
				<< (!emptyIP ? ip : "Failed to get IP") << rang::fg::reset << "\n";
			}
		}
	}

	void Reload() {
		std::string modname = "Config reload";

		if (isNodeListEmpty()) return;

		std::cout << "Reloading config of " << rang::fg::yellow << "all" << rang::fg::reset << " nodes...\n";

		for (size_t nid = 1; nid <= nodes.size(); nid++) {
			if (!easyNodeAuth(nid - 1)) {
				User::Error("Node #" + std::to_string(nid) + " have an invalid token", modname);
				continue;
			}

			Node& node = nodes.at(nid - 1);

			Conveyor nodetalk(node.url, node.token);

			bool online = nodetalk.Check(), reloaded = nodetalk.ReloadConfig();

			if (online) {
				if (reloaded) {
					User::Notify("Succesfully reloaded config of node " + std::to_string(nid), modname);
				}
				else {
					User::Error("Failed reloading config of node " + std::to_string(nid), modname);
				}
			}
			else {
				User::Error("Node #" + std::to_string(nid) + " is offline", modname);
			}
		}
	}

	void SingleReload() {
		std::string modname = "Config reload";

		if (isNodeListEmpty()) return;

		std::string id;

		if (!User::Request(id, "Enter the node number")) return;

		bool online, reloaded;

		if (!easyNodeAuth(safeConvert(id))) {
			User::Error("Node token is invalid or id is incorrect", modname);
			return;
		}

		Node& node = nodes.at(safeConvert(id));

		Conveyor nodetalk(node.url, node.token);

		online = nodetalk.Check(), reloaded = nodetalk.ReloadConfig();

		if (!online || !reloaded) User::Error("Node is offline or failed to reload config", modname);

		User::Notify("Succesfully reloaded config of node", modname);
	}

	void MassGet() {
		if (isNodeListEmpty()) return;

		std::string url, uag, modname = "Mass GET";
		if (!User::Request(url, "Enter the url to send requests")) return;
		if (!User::Request(uag, "Enter the User Agent")) return;

		size_t id = 1;
		for (; id <= nodes.size(); id++) {
			if (!easyNodeAuth(id - 1)) {
				User::Error("Node #" + std::to_string(id) + " have an invalid token", modname);
				continue;
			}

			Node& node = nodes.at(id - 1);
			Conveyor nodetalk(node.url, node.token);

			if (!nodetalk.Mass(Conveyor::Request::GET, MasserData(url, uag))) {
				User::Error("Failed sending data to node #" + std::to_string(id), modname);
				continue;
			}
			else {
				User::Notify("Succesfully sent data to node #" + std::to_string(id), modname);
			}
		}
	}

	void MassPost() {
		if (isNodeListEmpty()) return;

		std::string url, uag, body, ctype, rgx, modname = "Mass POST";
		if (!User::Request(url, "Enter the url to send requests")) return;
		if (!User::Request(uag, "Enter the User Agent")) return;
		if (!User::Request(body, "Enter the Body")) return;
		if (!User::Request(ctype, "Enter the Content Type")) return;
		if (!User::Request(rgx, "The body uses regex? Type \"yes\" to confirm")) return;

		size_t id = 1;
		for (; id <= nodes.size(); id++) {
			if (!easyNodeAuth(id - 1)) {
				User::Error("Node #" + std::to_string(id) + " have an invalid token", modname);
				continue;
			}

			Node& node = nodes.at(id - 1);
			Conveyor nodetalk(node.url, node.token);

			std::transform(rgx.cbegin(), rgx.cend(), std::inserter(rgx, rgx.end()), tolower);

			if (!nodetalk.Mass(Conveyor::Request::POST, MasserData(url, uag, body, ctype, (rgx == "yes")))) {
				User::Error("Failed sending data to node #" + std::to_string(id), modname);
				continue;
			}
			else {
				User::Notify("Succesfully sent data to node #" + std::to_string(id), modname);
			}
		}
	}

	void SingleMassGet() {
		if (isNodeListEmpty()) return;

		std::string id, url, uag, modname = "Single Mass Get";

		if (!User::Request(id, "Enter the node number")) return;
		if (!User::Request(url, "Enter the url to send requests")) return;
		if (!User::Request(uag, "Enter the User Agent")) return;

		if (!easyNodeAuth(safeConvert(id))) {
			User::Error("Node token is invalid or id is incorrect", modname);
			return;
		}

		Node& node = nodes.at(safeConvert(id));

		Conveyor nodetalk(node.url, node.token);

		if (!nodetalk.Check()) User::Error("Node is offline", modname);

		if (!nodetalk.Mass(Conveyor::Request::GET, MasserData(url, uag))) {
			User::Error("Failed sending data to the node", modname);
			return;
		}

		User::Notify("Succesfully sent requester info", modname);
	}

	void SingleMassPost() {
		if (isNodeListEmpty()) return;

		std::string id, url, uag, body, ctype, rgx, modname = "Single Mass Post";

		if (!User::Request(id, "Enter the node number")) return;
		if (!User::Request(url, "Enter the url to send requests")) return;
		if (!User::Request(uag, "Enter the User Agent")) return;
		if (!User::Request(body, "Enter the Body")) return;
		if (!User::Request(ctype, "Enter the Content Type")) return;
		if (!User::Request(rgx, "The body uses regex? Type \"yes\" to confirm")) return;

		if (!easyNodeAuth(safeConvert(id))) {
			User::Error("Node token is invalid or id is incorrect", modname);
			return;
		}

		Node& node = nodes.at(safeConvert(id));

		Conveyor nodetalk(node.url, node.token);

		if (!nodetalk.Check()) User::Error("Node is offline", modname);

		std::transform(rgx.cbegin(), rgx.cend(), std::inserter(rgx, rgx.end()), tolower);

		if (!nodetalk.Mass(Conveyor::Request::POST, MasserData(url, uag, body, ctype, (rgx == "yes")))) {
			User::Error("Failed sending data to the node", modname);
			return;
		}

		User::Notify("Succesfully sent requester info", modname);
	}

	void Restart() {
		std::string modname = "Restart";
		if (isNodeListEmpty()) return;

		std::cout << "Restarting " << rang::fg::yellow << "all" << rang::fg::reset << " nodes...\n";

		size_t nid = 0;
		for (Node& node : nodes) {
			if (!easyNodeAuth(nid)) {
				User::Error("Node #" + std::to_string(++nid) + " has an invalid token", modname);
				continue;
			}

			nid++;

			Conveyor nodetalk(node.url, node.token);

			bool online = nodetalk.Check();

			if (online) {
				if (nodetalk.Restart()) {
					User::Notify("Node #" + std::to_string(nid) + " restarted!", modname);
				}
				else {
					User::Error("Node #" + std::to_string(nid) + " couldn't be restarted", modname);
				}
			}
			else {
				User::Error("Node #" + std::to_string(nid) + " isn't online", modname);
			}
		}
	}

	void Kill() {
		std::string modname = "Kill Nodes";
		if (isNodeListEmpty()) return;

		std::cout << "Killing " << rang::fg::yellow << "all" << rang::fg::reset << " nodes...\n";

		size_t nid = 0;
		for (Node& node : nodes) {
			if (!easyNodeAuth(nid)) {
				User::Error("Node #" + std::to_string(++nid) + " has an invalid token", modname);
				continue;
			}

			nid++;

			Conveyor nodetalk(node.url, node.token);

			bool online = nodetalk.Check();

			if (online) {
				nodetalk.Kill();
				
				User::Notify("Sent kill instruction to node #" + std::to_string(nid), modname);
			}
			else {
				User::Error("Node #" + std::to_string(nid) + " isn't online", modname);
			}
		}
	}
};
