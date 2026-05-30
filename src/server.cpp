#include "server.hpp"

#include <iostream>

#include <asio.hpp>

Server::Server(Agent& agent) : agent_(agent) {}

void Server::Run() {
    try {
        asio::io_context io_context;
        asio::ip::tcp::acceptor acceptor {io_context, asio::ip::tcp::endpoint {asio::ip::tcp::v4(), 8888}};
        std::cout << "Server::Run: Server started on port 8888." << std::endl;

        while (true) {
            asio::ip::tcp::socket client_socket {io_context};
            acceptor.accept(client_socket);
            asio::streambuf buffer;
            asio::error_code error;
            asio::read_until(client_socket, buffer, '\n', error);
            if (!error) {
                std::string command;
                std::istream is {&buffer};

                std::getline(is, command);
                if (!command.empty() && command.back() == '\r') {
                    command.pop_back();
                }

                std::string reply = ProcessCommand(command);
                asio::write(client_socket, asio::buffer(reply));
            } else {
                std::cout << "Server::Run: Error while receiving data: " << error.message() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Server::Run: Error: " << e.what() << std::endl;
    }
}

std::string Server::ProcessCommand(const std::string& command) {
    std::cout << "Server::ProcessCommand: Recieved - " << command << std::endl;
    return "Recieved";
}
