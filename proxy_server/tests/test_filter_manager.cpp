#include <gtest/gtest.h>
#include "filter_manager.hpp"

class FilterManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        filter_manager = std::make_unique<FilterManager>();
    }

    void TearDown() override {
        filter_manager.reset();
    }

    std::unique_ptr<FilterManager> filter_manager;
};

TEST_F(FilterManagerTest, AddBlacklistEntry) {
    filter_manager->add_blacklist_entry("example.com");
    filter_manager->add_blacklist_entry("test.org");
    
    auto blacklist = filter_manager->get_blacklist();
    EXPECT_TRUE(blacklist.find("example.com") != blacklist.end());
    EXPECT_TRUE(blacklist.find("test.org") != blacklist.end());
}

TEST_F(FilterManagerTest, RemoveBlacklistEntry) {
    filter_manager->add_blacklist_entry("example.com");
    filter_manager->remove_blacklist_entry("example.com");
    
    auto blacklist = filter_manager->get_blacklist();
    EXPECT_TRUE(blacklist.find("example.com") == blacklist.end());
}

TEST_F(FilterManagerTest, CheckUrl) {
    filter_manager->add_blacklist_entry("blocked.com");
    filter_manager->add_blacklist_entry("allowed.com");
    filter_manager->remove_blacklist_entry("allowed.com");
    
    EXPECT_TRUE(filter_manager->is_blocked("http://blocked.com/page"));
    EXPECT_FALSE(filter_manager->is_blocked("https://allowed.com/page"));
    EXPECT_FALSE(filter_manager->is_blocked("http://unknown.com/page"));
}

TEST_F(FilterManagerTest, DuplicateEntry) {
    EXPECT_NO_THROW(filter_manager->add_blacklist_entry("example.com"));
    EXPECT_NO_THROW(filter_manager->add_blacklist_entry("example.com"));
    
    auto blacklist = filter_manager->get_blacklist();
    EXPECT_EQ(blacklist.count("example.com"), 1);
}

TEST_F(FilterManagerTest, RemoveNonexistentEntry) {
    EXPECT_NO_THROW(filter_manager->remove_blacklist_entry("nonexistent.com"));
}

TEST_F(FilterManagerTest, CaseInsensitiveMatching) {
    filter_manager->add_blacklist_entry("Example.com");
    
    EXPECT_TRUE(filter_manager->is_blocked("http://example.com/page"));
    EXPECT_TRUE(filter_manager->is_blocked("http://EXAMPLE.com/page"));
    EXPECT_TRUE(filter_manager->is_blocked("http://ExAmPlE.cOm/page"));
}

TEST_F(FilterManagerTest, SubdomainMatching) {
    filter_manager->add_blacklist_entry("example.com");
    
    EXPECT_TRUE(filter_manager->is_blocked("http://sub.example.com/page"));
    EXPECT_TRUE(filter_manager->is_blocked("http://sub.sub.example.com/page"));
    EXPECT_FALSE(filter_manager->is_blocked("http://example.org/page"));
}

TEST_F(FilterManagerTest, BlacklistMode) {
    EXPECT_TRUE(filter_manager->is_blacklist_mode());
    
    filter_manager->set_blacklist_mode(false);
    EXPECT_FALSE(filter_manager->is_blacklist_mode());
    
    filter_manager->set_blacklist_mode(true);
    EXPECT_TRUE(filter_manager->is_blacklist_mode());
} 