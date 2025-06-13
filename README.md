# C++ Proxy Server (cxx_proxy)

HSE MIEM student project for a high-performance HTTP/HTTPS proxy server implementation in C++.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Dependencies](#dependencies)
- [Building the Project](#building-the-project)
  - [Using CMake](#using-cmake)
  - [Using Make](#using-make)
- [Running the Server](#running-the-server)
- [Configuration](#configuration)
- [Running Tests](#running-tests)
- [Project Structure](#project-structure)

---

## Overview

This project implements a robust HTTP/HTTPS proxy server with advanced features including URL filtering, connection pooling, and a modern web interface for monitoring and management.

---

## Features

### HTTP/HTTPS Support

- Full HTTP/1.1 protocol implementation
- HTTPS tunneling with SSL/TLS support
- Connection pooling for improved performance

### Web Interface

- Add/remove blacklist
- Logs

---

## Dependencies

To build and run this project, you will need:

- C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 2019+)
- CMake 3.10 or higher
- OpenSSL development libraries
- Google Test (for testing)
- Doxygen (optional, for documentation)

### Installing Dependencies

#### Arch Linux
```bash
sudo pacman -S base-devel cmake gtest doxygen openssl
```

#### macOS
```bash
brew install cmake googletest doxygen openssl
```

#### Windows
1. Install [Visual Studio](https://visualstudio.microsoft.com/) with C++ development tools
2. Install [CMake](https://cmake.org/download/)
3. Install [vcpkg](https://github.com/Microsoft/vcpkg) and run:
```bash
vcpkg install gtest:x64-windows
vcpkg install openssl:x64-windows
```

---

## Building the Project

### Using CMake

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

### Using Make

```bash
make -j$(nproc)
```

---

## Running the Server

Or specify a custom port:
```bash
./proxy_server/proxy_server [server port] [web port]
```

---

## Configuration

The proxy server can be configured through:
1. Command line arguments
2. Web interface
3. Configuration file

Available options:
- Port number
- Blacklist/whitelist mode
- Logging level
- Maximum connections
- Cache size
- Rate limiting
- SSL certificate paths

---

## Running Tests

Run the test suite:
```bash
# Run all tests
make test

# Run specific test
./test_runner --gtest_filter=FilterManagerTest.*

# Run with coverage (requires gcov/lcov)
make coverage
```

---

## Project Structure

```
proxy_server/
├── include/           # Header files
│   ├── proxy_server.hpp
│   ├── filter_manager.hpp
│   ├── web_ui.hpp
│   └── logger.hpp
├── src/              # Source files
│   ├── proxy_server.cpp
│   ├── filter_manager.cpp
│   ├── web_ui.cpp
│   └── logger.cpp
├── tests/            # Test files
│   ├── test_main.cpp
│   ├── test_filter_manager.cpp
│   ├── test_web_ui.cpp
│   └── test_logger.cpp
├── third_party/      # Third-party dependencies
│   └── httplib.h
└── CMakeLists.txt    # CMake build configuration
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [cpp-httplib](https://github.com/yhirose/cpp-httplib) for the HTTP server implementation
- [Google Test](https://github.com/google/googletest) for the testing framework
- [OpenSSL](https://www.openssl.org/) for SSL/TLS support
