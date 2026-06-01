#include "server.hpp"

#include <iostream>
#include <thread>
#include <chrono>

#include <httplib.h>

#include "config.hpp"

Server::Server(Agent& agent) : agent_ {agent} {}

void Server::Run() {
    httplib::Server server;

    server.Post("/input", [this](const httplib::Request& req, httplib::Response& res) {
        std::string input {req.body};
        agent_.ProcessInput(input);
        res.set_content("Recieved", "application/json");
    });

    server.Post("/kill", [this](const httplib::Request& req, httplib::Response& res) {
        agent_.KillDrone();
        res.set_content("Recieved", "application/json");
    });

    server.Get("/output", [this](const httplib::Request& req, httplib::Response& res) {
        std::string output {agent_.GetOutput()};
        res.set_content(output, "application/json");
    });

    server.Get("/telemetry", [this](const httplib::Request& req, httplib::Response& res) {
        std::string telemetry {agent_.GetDroneTelemetry()};
        res.set_content(telemetry, "application/json");
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
