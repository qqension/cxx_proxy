#pragma once

#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <functional>
#include "filter_manager.hpp"

class ProxyServer {
public:
    static constexpr int BUFFER_SIZE = 8192;
    static constexpr int MAX_CONNECTIONS = 100;

    ProxyServer(uint16_t port, FilterManager& filter_manager);
    ~ProxyServer();

    void start();
    void stop();
    bool is_running() const;

private:
    void handle_connection(int client_socket);
    void accept_connections();
    bool initialize_socket();
    int create_target_connection(const std::string& host, int port);
    void tunnel_connection(int client_socket, int target_socket);
    void send_error_response(int socket, const std::string& status);
    std::string extract_host_from_request(const std::string& request);

    uint16_t port_;
    int server_socket_;
    std::atomic<bool> running_;
    std::vector<std::thread> worker_threads_;
    std::mutex mutex_;
    FilterManager& filter_manager_;
}; 