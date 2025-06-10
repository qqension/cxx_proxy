#pragma once

#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <functional>

class ProxyServer {
public:
    ProxyServer(const std::string& host, uint16_t port);
    ~ProxyServer();

    void start();
    void stop();
    bool is_running() const;

private:
    void handle_connection(int client_socket);
    void accept_connections();
    bool initialize_socket();

    std::string host_;
    uint16_t port_;
    int server_socket_;
    std::atomic<bool> running_;
    std::vector<std::thread> worker_threads_;
    std::mutex mutex_;
    
    static constexpr int MAX_CONNECTIONS = 100;
    static constexpr int BUFFER_SIZE = 8192;
}; 