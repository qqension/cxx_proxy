# C++ Proxy Server

A modern C++ proxy server with web-based management interface, built using CMake.

## Overview

This proxy server provides HTTP/HTTPS proxy functionality with a web-based management interface. It includes features like URL filtering, logging, and real-time monitoring.

## Features

- **Proxy Functionality**
  - HTTP/HTTPS proxy support
  - URL filtering and blacklisting
  - Connection pooling
  - Request/response caching

- **Web Interface**
  - Real-time monitoring dashboard
  - Filter management
  - Connection statistics
  - System status

- **Logging System**
  - Configurable log levels
  - File-based logging
  - Timestamp and log level tracking
  - Log rotation

- **Filter Management**
  - URL blacklisting
  - Pattern-based filtering
  - Real-time filter updates
  - Filter statistics

## Dependencies

- C++17 compatible compiler (g++ 8.0+ or clang++ 7.0+)
- CMake 3.10 or higher
- GTest (for unit tests)
- Doxygen (for documentation)

## Building

### Prerequisites

```bash
# Install build dependencies
sudo apt-get install build-essential cmake gtest libgtest-dev doxygen
```

### Build Steps

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run tests
ctest
```

## Running

```bash
# Start the proxy server (default port: 8080)
./proxy_server [port]
```

## Web Interface

Access the web interface at `http://localhost:8080` to:
- Monitor active connections
- Manage URL filters
- View system statistics
- Configure proxy settings

## Project Structure

```
.
├── CMakeLists.txt          # Main CMake configuration
├── proxy_server/
│   ├── include/           # Header files
│   ├── src/              # Source files
│   ├── third_party/      # Third-party dependencies
│   └── Doxyfile.in       # Documentation template
├── tests/                # Unit tests
│   ├── CMakeLists.txt    # Test configuration
│   └── test_*.cpp        # Test files
├── build/               # Build directory
└── logs/               # Log files
```

## Testing

The project uses Google Test framework for unit testing. Tests are automatically built with the main project.

```bash
# Run all tests
cd build
ctest

# Run specific test
./tests/test_runner --gtest_filter=LoggerTest.*
```

## Documentation

Generate documentation using Doxygen:

```bash
cd build
cmake --build . --target docs
```

The documentation will be available in `build/doc/html/index.html`.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
