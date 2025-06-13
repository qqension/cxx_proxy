#pragma once

#include "filter_manager.hpp"
#include <httplib.h>
#include <string>

class WebUI {
public:
    WebUI(uint16_t port, FilterManager& filter_manager);
    void start();

private:
    std::string generate_dashboard();
    uint16_t port_;
    FilterManager& filter_manager_;
    httplib::Server server_;
}; 