#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

class FilterManager {
public:
    using FilterCallback = std::function<bool(const std::string&)>;
    
    FilterManager();
    ~FilterManager() = default;

    // Add a new filter
    void add_filter(const std::string& name, FilterCallback filter);
    
    // Remove a filter by name
    void remove_filter(const std::string& name);
    
    // Apply all filters to the data
    bool apply_filters(const std::string& data) const;
    
    // Clear all filters
    void clear_filters();

private:
    struct Filter {
        std::string name;
        FilterCallback callback;
    };
    
    std::vector<Filter> filters_;
}; 