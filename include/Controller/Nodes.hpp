#pragma once
#include <algorithm>
#include <Controller/Conveyor.hpp>
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
        Unauthorized = 0,
        EmptyURL
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
            return nlohmann::json{ {"url", n.url}, {"token", n.token}};
        }

        static Node from_json(const nlohmann::json& j) {
            return Node(j.at("url"), j.at("token"));
        }
    };

    std::vector<Node> nodes;

    bool easyNodeAuth(size_t id) {
        Node node;

            try {
                if (!getNodeFromID(node, nid)) {
                    return false;
                }

                internalAuthorize(node, token);

                return true;
            }
            catch (...) {
                return false;
            }

        return false;
    }

    void internalAuthorize(Node& node, std::string token) {
        std::string nurl = node.url, modname = "Internal Authorization";
        
        if (nurl.empty()) {
            User::Error("Node url is empty!", modname);
            throw std::runtime_error(std::to_string(EmptyURL));
            return;
        }

        Conveyor nodetalk(nurl);

        nodetalk.AddToken(token);

        if (!nodetalk.AuthorizationCheck()) {
            throw std::runtime_error(std::to_string(Unauthorized));
        }
        else {
            node.token = token;
        }
    }

    bool isNodeListEmpty() {
        bool nem = nodes.empty();

        if (nem) User::Error("There aren't any nodes registered yet!", "Node List");

        return nem;
    }

    bool getNodeFromID(Node& node, size_t nid) {
        try {
            node = nodes.at(nid);
            return true;
        }
        catch (...) {
            User::Error("ID out of range", "Node List");
            return false;
        }
    }

public:
    void Authorize() {
        if (isNodeListEmpty()) return;

        std::string token, modname = "Authorization";

        if (!User::Request(token, "Insert the token for all nodes")) return;
        
        for (size_t nid = 1; nid < nodes.size(); nid++) {
            if (!easyNodeAuth(nid - 1)) {
                User::Error("Node #" + std::to_string(nid) + " wasn't authorized", modname);
            }
            else {
                User::Notify("Node #" + std::to_string(nid) + " authorized!", modname);
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

        try {
            size_t nid = std::stoull(id) - 1;

            Node node;

            if (!getNodeFromID(node, nid)) {
                User::Error("Invalid node number", modname);
                return;
            }

            internalAuthorize(node, token);
        }
        catch (std::runtime_error& e) {
            switch (std::stoi(e.what())) {
            case Unauthorized:
                User::Error("Token wasn't authorized", modname);
                return;
                break;
            case EmptyURL:
                User::Error("Node url is empty", modname);
                return;
                break;
            default:
                break;
            }

            throw std::exception();
            return;
        }
        catch (...) {
            User::Error("Unknown error authenticating token", modname);
            return;
        }

        if (token == "default") {
            User::Warn("To have better security try to change the access token of your node and reload config");
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
                    User::Warn("Node list is empty");
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

        nlohmann::json j;
        j["nodes"] = nlohmann::json::array();

        std::transform(nodes.begin(), nodes.end(), std::inserter(j, j.end()), Node::to_json);

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
            nid++;

            try {
                internalAuthorize(node, node.token);
            }
            catch (std::runtime_error& e) {
                switch (std::stoi(e.what())) {
                case Unauthorized:
                    User::Error("Token wasn't authorized on node " + std::to_string(nid), modname);
                    return;
                    break;
                case EmptyURL:
                    User::Error("Url is empty on node " + std::to_string(nid), modname);
                    return;
                    break;
                default:
                    break;
                }

                throw std::exception();
                return;
            }
            catch (...) {
                User::Error("Unknown error checking token of node" + std::to_string(nid), modname);
                return;
            }

            Conveyor nodetalk(node.url);
            nodetalk.AddToken(node.token);

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
            Node node;
            
            try {
                if (!getNodeFromID(node, nid - 1)) {
                    throw std::exception();
                }

                internalAuthorize(node, node.token);
            }
            catch (std::runtime_error& e) {
                switch (std::stoi(e.what())) {
                case Unauthorized:
                    User::Error("Token wasn't authorized on node " + std::to_string(nid), modname);
                    return;
                    break;
                case EmptyURL:
                    User::Error("Node url is empty" + std::to_string(nid), modname);
                    return;
                    break;
                default:
                    break;
                }

                throw std::exception();
                return;
            }
            catch (...) {
                User::Error("Unknown error checking token", modname);
                return;
            }

            Conveyor nodetalk(node.url);
            nodetalk.AddToken(node.token);

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
                User::Error("Node " + std::to_string(nid) + " is offline", modname);
            }

            nid++;
        }
    }

    void SingleReload() {
        std::string modname = "Config reload";

        if (isNodeListEmpty()) return;

        std::string id;

        if (!User::Request(id, "Enter the node number")) return;

        bool online, reloaded;

        size_t nid = std::stoull(id) - 1;

        Node node;

        if (!getNodeFromID(node, nid)) {
            User::Error("Invalid node number", modname);
            return;
        }

        try {
            internalAuthorize(node, node.token);
        }
        catch (std::runtime_error& e) {
            if (std::string(e.what()) == std::to_string(Unauthorized)) {
                User::Error("Invalid token", modname);
                return;
            }
        }

        Conveyor nodetalk(node.url);
        nodetalk.AddToken(node.token);

        online = nodetalk.Check(), reloaded = nodetalk.ReloadConfig();

        if (!online || !reloaded) User::Error("Node is offline or failed to reload config", modname);

        User::Notify("Succesfully reloaded config of node", modname);
    }

    void MassGet() {
        if (isNodeListEmpty()) return;
    }

    void MassPost() {
        if (isNodeListEmpty()) return;
    }

    void SingleMassGet() {
        if (isNodeListEmpty()) return;
    }

    void SingleMassPost() {
        if (isNodeListEmpty()) return;
    }
};
