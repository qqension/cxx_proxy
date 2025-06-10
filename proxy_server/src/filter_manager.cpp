#include "filter_manager.hpp"
#include <algorithm>

FilterManager::FilterManager() = default;

void FilterManager::add_filter(const std::string& name, FilterCallback filter) {
    filters_.push_back({name, std::move(filter)});
}

void FilterManager::remove_filter(const std::string& name) {
    filters_.erase(
        std::remove_if(filters_.begin(), filters_.end(),
            [&name](const Filter& f) { return f.name == name; }),
        filters_.end()
    );
}

bool FilterManager::apply_filters(const std::string& data) const {
    for (const auto& filter : filters_) {
        if (!filter.callback(data)) {
            return false;
        }
    }
    return true;
}

void FilterManager::clear_filters() {
    filters_.clear();
} 