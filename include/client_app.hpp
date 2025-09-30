#pragma once

#include <string>
#include <vector>

namespace sotc {

struct LaunchOptions {
    std::string server_host;
    int server_port{3979};
    std::string player_name;
    bool headless{false};
};

class ClientApp {
public:
    ClientApp();

    void configure(LaunchOptions options);

    [[nodiscard]] const LaunchOptions &options() const noexcept { return options_; }

    void run();

private:
    LaunchOptions options_{};
    void log_startup_info() const;
};

std::vector<std::string> discover_local_servers();

} // namespace sotc
