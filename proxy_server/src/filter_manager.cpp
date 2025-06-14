#include "filter_manager.hpp"
#include "logger.hpp"
#include <regex>

FilterManager::FilterManager() : blacklist_mode_(false) {}

void FilterManager::set_blacklist_mode(bool enabled) {
    blacklist_mode_ = enabled;
    Logger::get_instance().info("Blacklist mode " + std::string(enabled ? "enabled" : "disabled"));
}

void FilterManager::add_blacklist_entry(const std::string& entry) {
    blacklist_.insert(entry);
    Logger::get_instance().info("Added blacklist entry: " + entry);
}

void FilterManager::remove_blacklist_entry(const std::string& entry) {
    blacklist_.erase(entry);
    Logger::get_instance().info("Removed blacklist entry: " + entry);
}

bool FilterManager::is_blocked(const std::string& url) const {
    if (!blacklist_mode_) {
        return false;
    }
    
    std::string domain = url;
    
    size_t protocol_end = domain.find("://");
    if (protocol_end != std::string::npos) {
        domain = domain.substr(protocol_end + 3);
    }
    
    size_t path_start = domain.find('/');
    if (path_start != std::string::npos) {
        domain = domain.substr(0, path_start);
    }
    
    size_t port_start = domain.find(':');
    if (port_start != std::string::npos) {
        domain = domain.substr(0, port_start);
    }
    
    if (domain.substr(0, 4) == "www.") {
        domain = domain.substr(4);
    }
    
    Logger::get_instance().debug("Checking domain: " + domain);
    
    for (const auto& entry : blacklist_) {
        if (domain == entry) {
            Logger::get_instance().info("Domain blocked: " + domain + " (matches blacklist entry: " + entry + ")");
            return true;
        }
    }
    
    return false;
} 