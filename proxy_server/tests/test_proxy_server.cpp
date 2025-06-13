#include <gtest/gtest.h>
#include "proxy_server.hpp"
#include "filter_manager.hpp"
#include <memory>

class ProxyServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        filter_manager = std::make_unique<FilterManager>();
        proxy_server = std::make_unique<ProxyServer>(8080, *filter_manager);
    }

    void TearDown() override {
        proxy_server.reset();
        filter_manager.reset();
    }

    std::unique_ptr<FilterManager> filter_manager;
    std::unique_ptr<ProxyServer> proxy_server;
};

TEST_F(ProxyServerTest, InitialState) {
    EXPECT_FALSE(proxy_server->is_running());
}

TEST_F(ProxyServerTest, StartStop) {
    // Note: This test might need to be modified based on your actual implementation
    // as it involves network operations
    EXPECT_NO_THROW(proxy_server->start());
    EXPECT_TRUE(proxy_server->is_running());
    
    proxy_server->stop();
    EXPECT_FALSE(proxy_server->is_running());
}

TEST_F(ProxyServerTest, BlacklistMode) {
    // Test that blacklist mode is enabled by default
    EXPECT_TRUE(filter_manager->is_blacklist_mode());
    
    // Test adding and checking blocked hosts
    filter_manager->add_to_blacklist("example.com");
    EXPECT_TRUE(filter_manager->is_blocked("example.com"));
    EXPECT_FALSE(filter_manager->is_blocked("allowed.com"));
}

TEST_F(ProxyServerTest, SocketInitialization) {
    // Test socket initialization
    EXPECT_TRUE(proxy_server->initialize_socket());
    
    // Test that we can't initialize socket twice
    EXPECT_FALSE(proxy_server->initialize_socket());
} 