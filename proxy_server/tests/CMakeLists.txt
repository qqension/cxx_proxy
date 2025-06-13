find_package(GTest REQUIRED)

add_executable(test_runner
    test_main.cpp
    test_logger.cpp
    test_web_ui.cpp
    test_filter_manager.cpp
    test_proxy_server.cpp
)

target_link_libraries(test_runner
    GTest::GTest
    GTest::Main
    proxy_lib
)

target_include_directories(test_runner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_test(NAME test_runner COMMAND test_runner) 