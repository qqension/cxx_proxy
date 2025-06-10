#include "proxy_server.hpp"
#include "filter_manager.hpp"
#include <iostream>
#include <csignal>

std::unique_ptr<ProxyServer> server;

void signal_handler(int signal) {
    if (server) {
        std::cout << "\nShutting down proxy server..." << std::endl;
        server->stop();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        return 1;
    }

    std::string host = argv[1];
    uint16_t port = static_cast<uint16_t>(std::stoi(argv[2]));

    // Set up signal handling
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    try {
        server = std::make_unique<ProxyServer>(host, port);
        std::cout << "Starting proxy server on " << host << ":" << port << std::endl;
        server->start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 