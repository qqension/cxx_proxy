#include <gtest/gtest.h>
#include "logger.hpp"
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Get the build directory path
        build_dir = std::filesystem::current_path();
        logs_dir = build_dir / "logs";
        test_log_file = logs_dir / "proxy.log";
        
        // Create logs directory if it doesn't exist
        std::filesystem::create_directories(logs_dir);
        
        // Clear the log file if it exists
        if (std::filesystem::exists(test_log_file)) {
            std::filesystem::remove(test_log_file);
        }
    }

    void TearDown() override {
        // Clean up test log file
        if (std::filesystem::exists(test_log_file)) {
            std::filesystem::remove(test_log_file);
        }
    }

    std::filesystem::path build_dir;
    std::filesystem::path logs_dir;
    std::filesystem::path test_log_file;
};

TEST_F(LoggerTest, LogInitialization) {
    Logger& logger = Logger::get_instance();
    // Verify logger is working by logging a message
    logger.info("Test initialization");
    
    // Give the logger time to write to the file
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Verify log file exists
    std::ifstream log_file(test_log_file);
    ASSERT_TRUE(log_file.is_open());
}

TEST_F(LoggerTest, LogMessage) {
    Logger& logger = Logger::get_instance();
    logger.info("Test message");
    
    // Give the logger time to write to the file
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Verify log file exists and contains the message
    std::ifstream log_file(test_log_file);
    ASSERT_TRUE(log_file.is_open());
    
    std::string line;
    std::getline(log_file, line);
    EXPECT_TRUE(line.find("Test message") != std::string::npos);
}

TEST_F(LoggerTest, DifferentLogLevels) {
    Logger& logger = Logger::get_instance();
    
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warning("Warning message");
    logger.error("Error message");
    
    // Give the logger time to write to the file
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::ifstream log_file(test_log_file);
    ASSERT_TRUE(log_file.is_open());
    
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(log_file, line)) {
        lines.push_back(line);
    }
    
    EXPECT_GE(lines.size(), 4);
    EXPECT_TRUE(lines[0].find("Debug message") != std::string::npos);
    EXPECT_TRUE(lines[1].find("Info message") != std::string::npos);
    EXPECT_TRUE(lines[2].find("Warning message") != std::string::npos);
    EXPECT_TRUE(lines[3].find("Error message") != std::string::npos);
} 