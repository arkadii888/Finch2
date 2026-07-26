#include "api_server.hpp"

#include <chrono>
#include <thread>

#include <httplib.h>
#include <spdlog/spdlog.h>

import globals;
import lifecycle;

ApiServer::ApiServer(Agent& agent) : agent_ {agent} {}

void ApiServer::Run() {
    httplib::Server server;

    server.Post("/input", [this](const httplib::Request& req, httplib::Response& res) {
        if (req.body.empty()) {
            res.status = 400;
            res.set_content(R"({"error":"Prompt cannot be empty"})", "application/json");
        } else if (agent_.ProcessInput(req.body)) {
            res.status = 202;
            res.set_content(R"({"status":"accepted"})", "application/json");
        } else {
            res.status = 409;
            res.set_content(R"({"error":"Another prompt is processing"})", "application/json");
        }
    });

    server.Post("/kill", [this](const httplib::Request& req, httplib::Response& res) {
        agent_.KillVehicle();
        res.set_content("Recieved", "application/json");
    });

    server.Post("/arm", [this](const httplib::Request& req, httplib::Response& res) {
        agent_.ArmVehicle();
        res.set_content("Recieved", "application/json");
    });

    server.Post("/disarm", [this](const httplib::Request& req, httplib::Response& res) {
        agent_.DisarmVehicle();
        res.set_content("Recieved", "application/json");
    });

    server.Post("/return", [this](const httplib::Request& req, httplib::Response& res) {
        agent_.ReturnVehicle();
        res.set_content("Recieved", "application/json");
    });

    server.Post("/land", [this](const httplib::Request& req, httplib::Response& res) {
        agent_.LandVehicle();
        res.set_content("Recieved", "application/json");
    });

    server.Get("/output", [this](const httplib::Request& req, httplib::Response& res) {
        std::string output {agent_.GetOutput()};
        res.set_content(output, "application/json");
    });

    server.Get("/telemetry", [this](const httplib::Request& req, httplib::Response& res) {
        std::string telemetry {agent_.GetVehicleTelemetry()};
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
