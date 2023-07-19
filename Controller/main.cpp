#include <Controller/Nodes.hpp>
#include <Controller/User.hpp>

int main() {
    Nodes nodes;
    User user;

    user.RegisterFunction("Authorize", "Set the token for authorization", [&]() {
        nodes.Authorize();
    });

    user.RegisterFunction("Authorize unique", "Set the token for a single node", [&]() {
        nodes.AuthorizeSingle();
    });

    user.RegisterFunction("Add node", "Add a node to the node list", [&]() {
        nodes.Add();
    });

    user.RegisterFunction("Remove node", "Remove a node from the node list", [&]() {
        nodes.Remove();
    });

    user.RegisterFunction("Clear nodes", "Remove ALL nodes from the node list", [&]() {
        nodes.RemoveAll();
    });

    user.RegisterFunction("Import nodes", "Import nodes from a list in a file", [&]() {
        nodes.Import();
    });

    user.RegisterFunction("Export nodes", "Export nodes to a file and their respective tokens", [&]() {
        nodes.Export();
    });

    user.RegisterFunction("Node list", "Prints the nodes url's", [&]() {
        nodes.PrintList();
    });

    user.RegisterFunction("Check nodes", "Check all nodes, their uptime, etc.", [&]() {
        nodes.Check();
    });

    user.RegisterFunction("Reload config", "Reloads the config of all nodes", [&]() {
        nodes.Reload();
    });

    user.RegisterFunction("Single reload config", "Reloads the config of just one node", [&]() {
        nodes.SingleReload();
    });

    user.RegisterFunction("GET DDoS", "Sets up massive GET requester", [&]() {
        nodes.MassGet();
    });

    user.RegisterFunction("POST DDoS", "Sets up massive POST requester", [&]() {
        nodes.MassPost();
    });

    user.RegisterFunction("GET DDoS Unique", "Sets up massive GET requester on a single node", [&]() {
        nodes.SingleMassGet();
    });

    user.RegisterFunction("POST DDoS Unique", "Sets up massive POST requester on a single node", [&]() {
        nodes.SingleMassPost();
    });

    user.RegisterFunction("Restart", "Stops all the requesters and threads", [&]() {
        nodes.Restart();
    });

    user.RegisterFunction("Kill", "Kill the main process on all nodes", [&]() {
        nodes.Kill();
    });

    user.Welcome("the Node Controller");
}