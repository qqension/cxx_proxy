cmake_minimum_required(VERSION 3.10)
project(proxy_server VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

# Add source files
set(SOURCES
    src/main.cpp
    src/proxy_server.cpp
    src/filter_manager.cpp
    src/web_ui.cpp
    src/logger.cpp
)

# Add header files
set(HEADERS
    include/proxy_server.hpp
    include/filter_manager.hpp
    include/web_ui.hpp
    include/logger.hpp
)

# Create library target
add_library(proxy_lib STATIC ${SOURCES} ${HEADERS})
target_include_directories(proxy_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)
target_link_libraries(proxy_lib PRIVATE Threads::Threads)

# Create executable target
add_executable(proxy_server src/main.cpp)
target_link_libraries(proxy_server PRIVATE proxy_lib)
target_include_directories(proxy_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

# Create logs directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/logs)

# Configure Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

# Enable testing
enable_testing()

# Add test executable
add_executable(test_runner
    tests/test_main.cpp
    tests/test_logger.cpp
    tests/test_web_ui.cpp
    tests/test_filter_manager.cpp
)

# Link test executable with GTest and our library
target_link_libraries(test_runner
    PRIVATE
        GTest::GTest
        GTest::Main
        proxy_lib
)

# Include test directories
target_include_directories(test_runner
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

# Add test command
add_test(NAME test_runner COMMAND test_runner) 