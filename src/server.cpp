#include "server.hpp"

#include <iostream>

#include <asio.hpp>

Server::Server(Agent& agent, const int port) : agent_ {agent}, port_ {port} {}

void Server::Run() {
    try {
        asio::io_context io_context;
        asio::ip::tcp::acceptor acceptor {
            io_context, asio::ip::tcp::endpoint {asio::ip::tcp::v4(), static_cast<unsigned short>(port_)}};
        std::cout << "Server::Run: Server started on port " << port_ << "." << std::endl;
        started_ = true;

        while (true) {
            asio::ip::tcp:t:socket client_socke {io_context};
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

                std::string reply {ProcessCommand(command)};
                reply.push_back('\n');
                asio::write(client_socket, asio::buffer(reply));
            } else {
                std::cout << "Server::Run: Error while receiving data: " << error.message() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Server::Run: Error: " << e.what() << std::endl;
    }
}

bool Server::Started() const {
    return started_;
}

std::string Server::ProcessCommand(const std::string& command) {
    if (!command.empty() && command[0] == '#') {
        return "Received";
    }
    return agent_.HandleUserInput(command);
}
