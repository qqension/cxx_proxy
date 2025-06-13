# C++ Proxy Server

A modern, high-performance HTTP proxy server written in C++ with filtering capabilities and a web-based management interface. This proxy server is designed to be fast, secure, and easy to configure.

## Features

- **HTTP/HTTPS Proxy**
  - Full HTTP/1.1 support
  - HTTPS tunneling
  - Connection pooling
  - Request/response caching

- **URL Filtering**
  - Blacklist/whitelist support
  - Case-insensitive domain matching
  - Subdomain support
  - Pattern-based filtering
  - Real-time filter updates

- **Web Interface**
  - Modern, responsive dashboard
  - Real-time connection monitoring
  - Filter management
  - System statistics
  - Configuration management

- **Security**
  - Thread-safe implementation
  - Input validation
  - Rate limiting
  - Access control

- **Logging & Monitoring**
  - Configurable log levels
  - File-based logging
  - Real-time log viewing
  - Performance metrics

## Prerequisites

- C++17 compatible compiler:
  - GCC 7+ or Clang 5+ (Linux/macOS)
  - MSVC 2019+ (Windows)
- CMake 3.10 or higher
- GTest (for running tests)
- Doxygen (optional, for documentation)
- OpenSSL (for HTTPS support)

### Installing Dependencies

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential cmake libgtest-dev doxygen libssl-dev
```

#### Fedora
```bash
sudo dnf install gcc-c++ cmake gtest-devel doxygen openssl-devel
```

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

## Building

1. Clone the repository:
```bash
git clone https://github.com/yourusername/cxx_proxy.git
cd cxx_proxy
```

2. Create and enter build directory:
```bash
mkdir build && cd build
```

3. Configure with CMake:
```bash
# For Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# For Release build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

4. Build the project:
```bash
# Using make (Linux/macOS)
make -j$(nproc)

# Using Visual Studio (Windows)
cmake --build . --config Release
```

## Running

Start the proxy server:
```bash
# Default port (8080)
./proxy_server/proxy_server

# Custom port
./proxy_server/proxy_server 9090
```

The server will start and you can access the web interface at `http://localhost:8080` (or your custom port).

### Configuration

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

## Testing

Run the test suite:
```bash
# Run all tests
make test

# Run specific test
./test_runner --gtest_filter=FilterManagerTest.*

# Run with coverage (requires gcov/lcov)
make coverage
```

## Documentation

Generate documentation:
```bash
make docs
```

The documentation will be available in the `build/html` directory. Open `index.html` in your browser to view it.

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

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development Guidelines

- Follow the existing code style
- Write unit tests for new features
- Update documentation as needed
- Use meaningful commit messages
- Keep commits focused and atomic

## Troubleshooting

Common issues and solutions:

1. **Build fails with OpenSSL errors**
   - Ensure OpenSSL development libraries are installed
   - Check OpenSSL paths in CMake configuration

2. **Tests fail on Windows**
   - Ensure GTest is properly installed via vcpkg
   - Check PATH includes GTest DLL location

3. **Proxy not accepting connections**
   - Check if port is already in use
   - Verify firewall settings
   - Check server logs for errors

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [cpp-httplib](https://github.com/yhirose/cpp-httplib) for the HTTP server implementation
- [Google Test](https://github.com/google/googletest) for the testing framework
- [OpenSSL](https://www.openssl.org/) for SSL/TLS support
