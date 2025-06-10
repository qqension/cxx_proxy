cmake_minimum_required(VERSION 3.10)
project(cxx_proxy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    src/main.cpp
    src/proxy_server.cpp
    src/filter_manager.cpp
)

# Add header files
set(HEADERS
    include/proxy_server.hpp
    include/filter_manager.hpp
)

# Create executable
add_executable(proxy_server ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(proxy_server PRIVATE include)

# Find required packages
find_package(Threads REQUIRED)

# Link libraries
target_link_libraries(proxy_server PRIVATE Threads::Threads) 