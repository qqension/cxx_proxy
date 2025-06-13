# C++ Proxy Server

A C++ implementation of a proxy server with web interface and logging capabilities.

## Overview

This project implements a functional proxy server that can handle HTTP/HTTPS requests, filter content, and provide a web-based management interface. It includes features for request filtering, logging, and real-time monitoring through a web dashboard.

## Features

- **Proxy Functionality:**
  - HTTP/HTTPS request handling
  - Request/Response filtering
  - Content modification capabilities
  - Connection management

- **Web Interface:**
  - Real-time monitoring dashboard
  - Blacklist management
  - Log viewer
  - System status display

- **Logging System:**
  - Detailed request/response logging
  - Error tracking
  - Performance monitoring
  - Configurable log levels (DEBUG, INFO, WARNING, ERROR)

- **Filter Management:**
  - URL-based filtering
  - Content-based filtering
  - Blacklist/Whitelist support
  - Real-time filter updates

## Dependencies

To build and run this project, you will need:

- g++ (with C++17 support or newer)
- make
- pthread library
- doxygen (optional, for generating documentation)

## Building the Project

The project uses Make for building. Follow these steps:

1. Navigate to the proxy_server directory:
   ```bash
   cd proxy_server
   ```

2. Build the project:
   ```bash
   make
   ```

   This will create the following:
   - `proxy_server` executable
   - `logs` directory for log files

3. Clean build files (optional):
   ```bash
   make clean
   ```

4. Generate documentation (optional):
   ```bash
   make docs
   ```

## Running the Server

To start the proxy server:

```bash
./proxy_server [server_port] [web_page_port]
```

Where `[port]` is an optional port number (default is 8080).

## Web Interface

The web interface is accessible at:
```
http://localhost:8080
```

Features available through the web interface:
- View real-time logs
- Manage blacklist entries
- Monitor system status
- View connection statistics

## Project Structure

```
.
├── proxy_server/           # Main project directory
│   ├── Doxyfile           # Doxygen configuration
│   ├── Makefile           # Build configuration
│   ├── include/           # Header files
│   │   ├── logger.hpp     # Logging system
│   │   ├── web_ui.hpp     # Web interface
│   │   └── filter_manager.hpp
│   ├── src/               # Source files
│   │   ├── logger.cpp
│   │   ├── web_ui.cpp
│   │   └── filter_manager.cpp
│   ├── third_party/       # External dependencies
│   │   └── httplib.h      # HTTP library
│   └── logs/              # Log files directory
├── README.md              # This file
└── LICENSE                # Project License
```

## Documentation

The project documentation can be generated using Doxygen:

```bash
cd proxy_server
make docs
```

This will create HTML documentation in the `doc` directory. Open `doc/html/index.html` in your web browser to view the documentation.

## License

This project is licensed under the terms of the LICENSE file included in the repository.
