#include "api_server.hpp"

#include <thread>
#include <chrono>

#include <httplib.h>
#include <spdlog/spdlog.h

import globals;
import lifecycle;

ApiServer::ApiServer(Agent& agent) : agent_ {agent} {}

void ApiServer::Run() {
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
        while (lifecycle::is_alive_public) {
            std::this_thread::sleep_for(std::chrono::milliseconds {200});
        }
        server.stop();
    }};

    spdlog::info("ApiServer::Run: Started on port {}", globals::api_server_port);

    server.listen("0.0.0.0", globals::api_server_port);

    monitor.join();
    spdlog::info("ApiServer::Run: Stopped");
}
