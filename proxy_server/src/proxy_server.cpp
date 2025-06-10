#include "proxy_server.hpp"
#include "filter_manager.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <regex>
#include <netdb.h>
#include <errno.h>
#include <ctime>

ProxyServer::ProxyServer(const std::string& host, uint16_t port)
    : host_(host), port_(port), server_socket_(-1), running_(false) {
    if (!initialize_socket()) {
        throw std::runtime_error("Failed to initialize socket");
    }
}

ProxyServer::~ProxyServer() {
    stop();
    if (server_socket_ != -1) {
        close(server_socket_);
    }
}

bool ProxyServer::initialize_socket() {
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        return false;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Setsockopt failed: " << strerror(errno) << std::endl;
        close(server_socket_);
        return false;
    }

    // Set non-blocking mode
    int flags = fcntl(server_socket_, F_GETFL, 0);
    if (flags < 0) {
        std::cerr << "Fcntl get failed: " << strerror(errno) << std::endl;
        close(server_socket_);
        return false;
    }
    if (fcntl(server_socket_, F_SETFL, flags | O_NONBLOCK) < 0) {
        std::cerr << "Fcntl set failed: " << strerror(errno) << std::endl;
        close(server_socket_);
        return false;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host_.c_str());
    address.sin_port = htons(port_);

    if (bind(server_socket_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(server_socket_);
        return false;
    }

    if (listen(server_socket_, MAX_CONNECTIONS) < 0) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(server_socket_);
        return false;
    }

    return true;
}

void ProxyServer::start() {
    running_ = true;
    accept_connections();
}

void ProxyServer::stop() {
    running_ = false;
    
    // Wait for all worker threads to finish
    for (auto& thread : worker_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    worker_threads_.clear();
}

bool ProxyServer::is_running() const {
    return running_;
}

void ProxyServer::accept_connections() {
    fd_set read_fds;
    struct timeval tv;
    
    while (running_) {
        FD_ZERO(&read_fds);
        FD_SET(server_socket_, &read_fds);
        
        // Set timeout to 1 second
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        
        int ret = select(server_socket_ + 1, &read_fds, nullptr, nullptr, &tv);
        if (ret < 0) {
            if (running_) {
                std::cerr << "Select failed: " << strerror(errno) << std::endl;
            }
            continue;
        }
        
        if (ret == 0) {
            // Timeout, continue the loop
            continue;
        }
        
        if (FD_ISSET(server_socket_, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            
            int client_socket = accept(server_socket_, (struct sockaddr*)&client_addr, &client_len);
            if (client_socket < 0) {
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    std::cerr << "Accept failed: " << strerror(errno) << std::endl;
                }
                continue;
            }

            // Create a new thread to handle the connection
            worker_threads_.emplace_back(&ProxyServer::handle_connection, this, client_socket);
            
            // Clean up finished threads
            worker_threads_.erase(
                std::remove_if(worker_threads_.begin(), worker_threads_.end(),
                    [](std::thread& t) { return !t.joinable(); }),
                worker_threads_.end()
            );
        }
    }
}

void ProxyServer::handle_connection(int client_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        std::string request(buffer);
        
        // Log request
        std::time_t now = std::time(nullptr);
        std::cout << "\n[" << std::ctime(&now) << "] Request:\n" << request << std::endl;
        
        // Parse the request
        std::istringstream request_stream(request);
        std::string method, path, version;
        request_stream >> method >> path >> version;
        
        // Handle CONNECT method (HTTPS)
        if (method == "CONNECT") {
            std::string host_port = path;
            size_t colon_pos = host_port.find(':');
            if (colon_pos == std::string::npos) {
                std::cerr << "Invalid CONNECT request" << std::endl;
                close(client_socket);
                return;
            }
            
            std::string host = host_port.substr(0, colon_pos);
            int port = std::stoi(host_port.substr(colon_pos + 1));
            
            std::cout << "HTTPS request to " << host << ":" << port << std::endl;
            
            // Create connection to target server
            int target_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (target_socket < 0) {
                std::cerr << "Failed to create target socket: " << strerror(errno) << std::endl;
                close(client_socket);
                return;
            }
            
            // Resolve hostname
            struct hostent* host_info = gethostbyname(host.c_str());
            if (!host_info) {
                std::cerr << "Failed to resolve hostname: " << host << " - " << hstrerror(h_errno) << std::endl;
                close(target_socket);
                close(client_socket);
                return;
            }
            
            struct sockaddr_in target_addr;
            memset(&target_addr, 0, sizeof(target_addr));
            target_addr.sin_family = AF_INET;
            target_addr.sin_port = htons(port);
            memcpy(&target_addr.sin_addr, host_info->h_addr, host_info->h_length);
            
            if (connect(target_socket, (struct sockaddr*)&target_addr, sizeof(target_addr)) < 0) {
                std::cerr << "Failed to connect to target server: " << host << ":" << port 
                         << " - " << strerror(errno) << std::endl;
                close(target_socket);
                close(client_socket);
                return;
            }
            
            // Send 200 Connection Established
            const char* response = "HTTP/1.1 200 Connection Established\r\n\r\n";
            send(client_socket, response, strlen(response), 0);
            
            std::cout << "HTTPS tunnel established" << std::endl;
            
            // Forward data in both directions
            fd_set read_fds;
            while (true) {
                FD_ZERO(&read_fds);
                FD_SET(client_socket, &read_fds);
                FD_SET(target_socket, &read_fds);
                
                int max_fd = std::max(client_socket, target_socket) + 1;
                if (select(max_fd, &read_fds, nullptr, nullptr, nullptr) < 0) {
                    if (errno != EINTR) {
                        std::cerr << "Select failed: " << strerror(errno) << std::endl;
                        break;
                    }
                    continue;
                }
                
                if (FD_ISSET(client_socket, &read_fds)) {
                    ssize_t bytes = recv(client_socket, buffer, BUFFER_SIZE, 0);
                    if (bytes <= 0) break;
                    if (send(target_socket, buffer, bytes, 0) < 0) {
                        std::cerr << "Failed to send to target: " << strerror(errno) << std::endl;
                        break;
                    }
                }
                
                if (FD_ISSET(target_socket, &read_fds)) {
                    ssize_t bytes = recv(target_socket, buffer, BUFFER_SIZE, 0);
                    if (bytes <= 0) break;
                    if (send(client_socket, buffer, bytes, 0) < 0) {
                        std::cerr << "Failed to send to client: " << strerror(errno) << std::endl;
                        break;
                    }
                }
            }
            
            std::cout << "HTTPS tunnel closed" << std::endl;
            close(target_socket);
            close(client_socket);
            return;
        }
        
        // Handle HTTP request
        std::string host;
        std::string line;
        while (std::getline(request_stream, line) && !line.empty()) {
            if (line.find("Host:") == 0) {
                host = line.substr(6);
                // Remove any whitespace
                host.erase(0, host.find_first_not_of(" \t\r\n"));
                host.erase(host.find_last_not_of(" \t\r\n") + 1);
                break;
            }
        }
        
        if (host.empty()) {
            std::cerr << "No Host header found" << std::endl;
            close(client_socket);
            return;
        }
        
        // Extract port from host if present
        int port = 80;
        size_t colon_pos = host.find(':');
        if (colon_pos != std::string::npos) {
            port = std::stoi(host.substr(colon_pos + 1));
            host = host.substr(0, colon_pos);
        }
        
        std::cout << "HTTP request to " << host << ":" << port << std::endl;
        
        // Create connection to target server
        int target_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (target_socket < 0) {
            std::cerr << "Failed to create target socket: " << strerror(errno) << std::endl;
            close(client_socket);
            return;
        }
        
        // Resolve hostname
        struct hostent* host_info = gethostbyname(host.c_str());
        if (!host_info) {
            std::cerr << "Failed to resolve hostname: " << host << " - " << hstrerror(h_errno) << std::endl;
            close(target_socket);
            close(client_socket);
            return;
        }
        
        struct sockaddr_in target_addr;
        memset(&target_addr, 0, sizeof(target_addr));
        target_addr.sin_family = AF_INET;
        target_addr.sin_port = htons(port);
        memcpy(&target_addr.sin_addr, host_info->h_addr, host_info->h_length);
        
        if (connect(target_socket, (struct sockaddr*)&target_addr, sizeof(target_addr)) < 0) {
            std::cerr << "Failed to connect to target server: " << host << ":" << port 
                     << " - " << strerror(errno) << std::endl;
            close(target_socket);
            close(client_socket);
            return;
        }
        
        // Forward the request
        if (send(target_socket, buffer, bytes_received, 0) < 0) {
            std::cerr << "Failed to send request: " << strerror(errno) << std::endl;
            close(target_socket);
            close(client_socket);
            return;
        }
        
        // Forward the response
        char response_buffer[BUFFER_SIZE];
        ssize_t response_bytes;
        std::string response_data;
        while ((response_bytes = recv(target_socket, response_buffer, BUFFER_SIZE, 0)) > 0) {
            if (send(client_socket, response_buffer, response_bytes, 0) < 0) {
                std::cerr << "Failed to send response: " << strerror(errno) << std::endl;
                break;
            }
            response_data.append(response_buffer, response_bytes);
        }
        
        if (response_bytes < 0) {
            std::cerr << "Failed to receive response: " << strerror(errno) << std::endl;
        }
        
        // Log response
        std::cout << "Response:\n" << response_data << std::endl;
        
        close(target_socket);
    }
    
    close(client_socket);
} 