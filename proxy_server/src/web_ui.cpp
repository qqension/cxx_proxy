#include "web_ui.hpp"
#include "logger.hpp"
#include <sstream>
#include <fstream>

WebUI::WebUI(uint16_t port, FilterManager& filter_manager)
    : port_(port), filter_manager_(filter_manager) {}

void WebUI::start() {
    server_.Get("/", [this](const httplib::Request&, httplib::Response& res) {
        res.set_content(generate_dashboard(), "text/html");
    });

    server_.Post("/add_blacklist", [this](const httplib::Request& req, httplib::Response& res) {
        std::string entry = req.get_param_value("entry");
        filter_manager_.add_blacklist_entry(entry);
        res.set_content("{\"success\":true}", "application/json");
    });

    server_.Post("/remove_blacklist", [this](const httplib::Request& req, httplib::Response& res) {
        std::string entry = req.get_param_value("entry");
        filter_manager_.remove_blacklist_entry(entry);
        res.set_content("{\"success\":true}", "application/json");
    });

    server_.Get("/logs", [this](const httplib::Request&, httplib::Response& res) {
        std::ifstream log_file("proxy.log");
        std::stringstream buffer;
        buffer << log_file.rdbuf();
        res.set_content(buffer.str(), "text/plain");
    });

    server_.listen("0.0.0.0", port_);
}

std::string WebUI::generate_dashboard() {
    std::stringstream ss;
    ss << R"DELIM(<!DOCTYPE html>
<html>
<head>
    <title>Blacklist Manager</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .container { max-width: 800px; margin: 0 auto; }
        .card { background: #f5f5f5; padding: 20px; margin: 10px 0; border-radius: 5px; }
        button { background: #4CAF50; color: white; border: none; padding: 10px 20px; border-radius: 3px; cursor: pointer; }
        button:hover { background: #45a049; }
        .blacklist-entry { display: flex; justify-content: space-between; align-items: center; margin: 5px 0; }
        .blacklist-entry button { padding: 5px 10px; }
        .logs { background: #1e1e1e; color: #fff; padding: 20px; border-radius: 5px; font-family: monospace; white-space: pre-wrap; max-height: 300px; overflow-y: auto; }
        .refresh-btn { margin: 10px 0; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Blacklist Manager</h1>
        <div class="card">
            <h2>Blacklist</h2>
            <form id="addBlacklistForm">
                <input type="text" name="entry" placeholder="Add to blacklist..." required>
                <button type="submit">Add</button>
            </form>
            <ul id="blacklistEntries">)DELIM";

    for (const auto& entry : filter_manager_.get_blacklist()) {
        ss << "<li class=\"blacklist-entry\">" << entry << " <button onclick=\"removeBlacklistEntry('" << entry << "')\">Remove</button></li>";
    }

    ss << R"DELIM(</ul>
        </div>

        <div class="card">
            <h2>Logs</h2>
            <button class="refresh-btn" onclick="refreshLogs()">Refresh Logs</button>
            <div id="logs" class="logs">Loading logs...</div>
        </div>
    </div>

    <script>
        function removeBlacklistEntry(entry) {
            fetch("/remove_blacklist", {
                method: "POST",
                headers: { "Content-Type": "application/x-www-form-urlencoded" },
                body: "entry=" + encodeURIComponent(entry)
            })
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    location.reload();
                }
            });
        }

        function refreshLogs() {
            fetch("/logs")
                .then(response => response.text())
                .then(logs => {
                    document.getElementById("logs").textContent = logs;
                });
        }

        document.getElementById("addBlacklistForm").onsubmit = function(e) {
            e.preventDefault();
            const entry = e.target.entry.value;
            fetch("/add_blacklist", {
                method: "POST",
                headers: { "Content-Type": "application/x-www-form-urlencoded" },
                body: "entry=" + encodeURIComponent(entry)
            })
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    location.reload();
                }
            });
        };

        // Load logs on page load
        refreshLogs();
        // Refresh logs every 5 seconds
        setInterval(refreshLogs, 5000);
    </script>
</body>
</html>)DELIM";

    return ss.str();
} 

