#pragma once

#include <string>
#include <set>
#include <chrono>

class FilterManager {
public:
    FilterManager();
    ~FilterManager() = default;

    // Blacklist management
    void add_blacklist_entry(const std::string& entry);
    void remove_blacklist_entry(const std::string& entry);
    const std::set<std::string>& get_blacklist() const { return blacklist_; }
    void set_blacklist_mode(bool enabled);
    bool is_blacklist_mode() const { return blacklist_mode_; }

    // Check if a URL is blocked
    bool is_blocked(const std::string& url) const;

private:
    std::set<std::string> blacklist_;
    bool blacklist_mode_;
}; 