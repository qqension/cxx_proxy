#include <gtest/gtest.h>
#include "web_ui.hpp"
#include "filter_manager.hpp"
#include <httplib.h>
#include <thread>
#include <chrono>

class WebUITest : public ::testing::Test {
protected:
    void SetUp() override {
        filter_manager = std::make_unique<FilterManager>();
        web_ui = std::make_unique<WebUI>(8080, *filter_manager);
        web_ui->start();
        // Give the server time to start
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void TearDown() override {
        web_ui->start(); // This will stop the server since it's already running
        // Give the server time to stop
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::unique_ptr<WebUI> web_ui;
    std::unique_ptr<FilterManager> filter_manager;
};

TEST_F(WebUITest, ServerStartup) {
    httplib::Client cli("localhost", 8080);
    auto res = cli.Get("/");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
}

TEST_F(WebUITest, GetFilters) {
    httplib::Client cli("localhost", 8080);
    auto res = cli.Get("/api/filters");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_TRUE(res->body.find("filters") != std::string::npos);
}

TEST_F(WebUITest, AddFilter) {
    httplib::Client cli("localhost", 8080);
    httplib::Params params;
    params.emplace("pattern", "test.com");
    params.emplace("action", "block");
    
    auto res = cli.Post("/api/filters", params);
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    
    // Verify filter was added
    auto get_res = cli.Get("/api/filters");
    ASSERT_TRUE(get_res);
    EXPECT_TRUE(get_res->body.find("test.com") != std::string::npos);
}

TEST_F(WebUITest, RemoveFilter) {
    httplib::Client cli("localhost", 8080);
    
    // First add a filter
    httplib::Params add_params;
    add_params.emplace("pattern", "test.com");
    add_params.emplace("action", "block");
    cli.Post("/api/filters", add_params);
    
    // Then remove it
    httplib::Params remove_params;
    remove_params.emplace("pattern", "test.com");
    auto res = cli.Delete("/api/filters?pattern=test.com");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    
    // Verify filter was removed
    auto get_res = cli.Get("/api/filters");
    ASSERT_TRUE(get_res);
    EXPECT_TRUE(get_res->body.find("test.com") == std::string::npos);
}

TEST_F(WebUITest, InvalidPort) {
    filter_manager = std::make_unique<FilterManager>();
    EXPECT_THROW(std::make_unique<WebUI>(0, *filter_manager), std::runtime_error);
    EXPECT_THROW(std::make_unique<WebUI>(70000, *filter_manager), std::runtime_error);
} 