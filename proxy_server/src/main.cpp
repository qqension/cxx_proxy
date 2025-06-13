#include "proxy_server.hpp"
#include "filter_manager.hpp"
#include "web_ui.hpp"
#include <iostream>
#include <thread>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <proxy_port> <web_ui_port>" << std::endl;
        return 1;
    }

    int proxy_port = std::stoi(argv[1]);
    int web_ui_port = std::stoi(argv[2]);

    FilterManager filter_manager;
    ProxyServer server(proxy_port, filter_manager);
    WebUI web_ui(web_ui_port, filter_manager);

    // Start the web UI in a separate thread
    std::thread web_thread([&web_ui]() {
        web_ui.start();
    });

    // Start the proxy server in the main thread
    server.start();

    web_thread.join();
    return 0;
} 