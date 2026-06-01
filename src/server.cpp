#include "server.hpp"

#include <iostream>
#include <thread>
#include <chrono>

#include <httplib.h>

#include "config.hpp"

Server::Server(Agent& agent) : agent_ {agent} {}

void Server::Run() {
    httplib::Server server;

    server.Post("/command", [this](const httplib::Request& req, httplib::Response& res) {
        std::string command {req.body};

        if (!command.empty() && command[0] == '#') {
            res.set_content("Recieved", "text/plain");
        } else {
            std::string reply {agent_.HandleUserInput(command)};
            res.set_content(reply, "text/plain");
        }
    });

    std::thread monitor {[&server]() {
        while (global_running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        server.stop();
    }};

    std::cout << "Server::Run: HTTP server started on port " << config.http_server_port << "." << std::endl;

    server.listen("0.0.0.0", config.http_server_port);

    monitor.join();
    std::cout << "Server::Run: HTTP server stopped." << std::endl;
}
