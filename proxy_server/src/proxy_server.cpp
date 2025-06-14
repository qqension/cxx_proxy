#include "proxy_server.hpp"
#include "logger.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <netdb.h>
#include <arpa/inet.h>
#include <regex>

ProxyServer::ProxyServer(uint16_t port, FilterManager& filter_manager)
    : port_(port), server_socket_(-1), running_(false), filter_manager_(filter_manager) {
    Logger::get_instance().info("Proxy server initialized on port " + std::to_string(port));
    filter_manager_.set_blacklist_mode(true);  //  toggle blacklist mode
}

ProxyServer::~ProxyServer() {
    stop();
}

bool ProxyServer::initialize_socket() {
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        Logger::get_instance().error("Failed to create socket");
        return false;
    }

    int opt = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        Logger::get_instance().error("Failed to set socket options");
        return false;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(server_socket_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        Logger::get_instance().error("Failed to bind socket");
        return false;
    }

    if (listen(server_socket_, MAX_CONNECTIONS) < 0) {
        Logger::get_instance().error("Failed to listen on socket");
        return false;
    }

    Logger::get_instance().info("Socket initialized successfully");
    return true;
}

void ProxyServer::start() {
    if (!initialize_socket()) {
        throw std::runtime_error("Failed to initialize socket");
    }

    running_ = true;
    Logger::get_instance().info("Proxy server started");
    accept_connections();
}

void ProxyServer::stop() {
    running_ = false;
    if (server_socket_ >= 0) {
        close(server_socket_);
        server_socket_ = -1;
    }

    for (auto& thread : worker_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    worker_threads_.clear();
    Logger::get_instance().info("Proxy server stopped");
}

bool ProxyServer::is_running() const {
    return running_;
}

void ProxyServer::accept_connections() {
    while (running_) {
        int client_socket = accept(server_socket_, nullptr, nullptr);
        if (client_socket < 0) {
            if (running_) {
                Logger::get_instance().error("Failed to accept connection");
            }
            continue;
        }

        worker_threads_.erase(
            std::remove_if(worker_threads_.begin(), worker_threads_.end(),
                [](std::thread& t) { return !t.joinable(); }),
            worker_threads_.end()
        );

        worker_threads_.emplace_back(&ProxyServer::handle_connection, this, client_socket);
    }
}

void ProxyServer::handle_connection(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    
    if (bytes_read <= 0) {
        Logger::get_instance().error("Failed to read from client socket");
        close(client_socket);
        return;
    }

    buffer[bytes_read] = '\0';
    std::string request(buffer);
    Logger::get_instance().debug("Received request:\n" + request);

    std::istringstream request_stream(request);
    std::string method, target, version;
    request_stream >> method >> target >> version;

    if (method == "CONNECT") {
        // Handle HTTPS CONNECT request
        Logger::get_instance().info("Processing HTTPS CONNECT request to: " + target);
        
        size_t colon_pos = target.find(':');
        if (colon_pos == std::string::npos) {
            Logger::get_instance().error("Invalid CONNECT target format");
            send_error_response(client_socket, "400 Bad Request");
            return;
        }

        std::string host = target.substr(0, colon_pos);
        
        // Check blocked host
        if (filter_manager_.is_blocked(host)) {
            Logger::get_instance().info("HTTPS request blocked - host in blacklist: " + host);
            send_error_response(client_socket, "403 Forbidden");
            return;
        }

        int port = std::stoi(target.substr(colon_pos + 1));

        // Create connection to server
        int target_socket = create_target_connection(host, port);
        if (target_socket < 0) {
            Logger::get_instance().error("Failed to connect to target server: " + host + ":" + std::to_string(port));
            send_error_response(client_socket, "502 Bad Gateway");
            return;
        }

        // Send 200 Connection 
        std::string response = "HTTP/1.1 200 Connection Established\r\n\r\n";
        if (send(client_socket, response.c_str(), response.length(), 0) < 0) {
            Logger::get_instance().error("Failed to send CONNECT response");
            close(target_socket);
            close(client_socket);
            return;
        }

        tunnel_connection(client_socket, target_socket);
    } else {
        // Handle regular HTTP request
        std::string host = extract_host_from_request(request);
        if (host.empty()) {
            Logger::get_instance().error("No host found in request");
            send_error_response(client_socket, "400 Bad Request");
            return;
        }

        if (filter_manager_.is_blocked(host)) {
            Logger::get_instance().info("HTTP request blocked - host in blacklist: " + host);
            send_error_response(client_socket, "403 Forbidden");
            return;
        }

        int port = 80;
        size_t colon_pos = host.find(':');
        if (colon_pos != std::string::npos) {
            port = std::stoi(host.substr(colon_pos + 1));
            host = host.substr(0, colon_pos);
        }

        int target_socket = create_target_connection(host, port);
        if (target_socket < 0) {
            Logger::get_instance().error("Failed to connect to target server: " + host + ":" + std::to_string(port));
            send_error_response(client_socket, "502 Bad Gateway");
            return;
        }

        if (send(target_socket, request.c_str(), request.length(), 0) < 0) {
            Logger::get_instance().error("Failed to forward request to target server");
            close(target_socket);
            send_error_response(client_socket, "502 Bad Gateway");
            return;
        }

        // Forward response from target 
        char response_buffer[BUFFER_SIZE];
        int bytes_read;
        while ((bytes_read = recv(target_socket, response_buffer, sizeof(response_buffer) - 1, 0)) > 0) {
            response_buffer[bytes_read] = '\0';
            if (send(client_socket, response_buffer, bytes_read, 0) < 0) {
                Logger::get_instance().error("Failed to send response to client");
                break;
            }
        }

        close(target_socket);
    }

    close(client_socket);
}

int ProxyServer::create_target_connection(const std::string& host, int port) {
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    std::string port_str = std::to_string(port);
    if (getaddrinfo(host.c_str(), port_str.c_str(), &hints, &result) != 0) {
        Logger::get_instance().error("Failed to resolve host: " + host);
        return -1;
    }

    int sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock < 0) {
        freeaddrinfo(result);
        return -1;
    }

    if (connect(sock, result->ai_addr, result->ai_addrlen) < 0) {
        Logger::get_instance().error("Failed to connect to target server");
        close(sock);
        freeaddrinfo(result);
        return -1;
    }

    freeaddrinfo(result);
    return sock;
}

void ProxyServer::send_error_response(int socket, const std::string& status) {
    std::string response = "HTTP/1.1 " + status + "\r\n"
                          "Content-Type: text/plain\r\n"
                          "Content-Length: " + std::to_string(status.length()) + "\r\n\r\n" +
                          status;
    send(socket, response.c_str(), response.length(), 0);
}

void ProxyServer::tunnel_connection(int client_socket, int target_socket) {
    fd_set read_fds;
    char buffer[BUFFER_SIZE];
    
    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(client_socket, &read_fds);
        FD_SET(target_socket, &read_fds);
        
        int max_fd = std::max(client_socket, target_socket);
        if (select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr) < 0) {
            Logger::get_instance().error("Select error in tunnel");
            break;
        }

        // Client to target
        if (FD_ISSET(client_socket, &read_fds)) {
            int bytes = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes <= 0) break;
            if (send(target_socket, buffer, bytes, 0) <= 0) break;
        }

        // Target to client
        if (FD_ISSET(target_socket, &read_fds)) {
            int bytes = recv(target_socket, buffer, sizeof(buffer), 0);
            if (bytes <= 0) break;
            if (send(client_socket, buffer, bytes, 0) <= 0) break;
        }
    }

    close(target_socket);
}

std::string ProxyServer::extract_host_from_request(const std::string& request) {
    std::regex host_regex("Host:\\s*([^\\r\\n]+)");
    std::smatch match;
    if (std::regex_search(request, match, host_regex)) {
        std::string host = match[1];
        size_t colon_pos = host.find(':');
        if (colon_pos != std::string::npos) {
            host = host.substr(0, colon_pos);
        }
        return host;
    }
    return "";
} 