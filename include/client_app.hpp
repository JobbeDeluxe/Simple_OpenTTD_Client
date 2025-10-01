#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include "network/coordinator_client.hpp"

namespace sotc {

struct LaunchOptions {
    std::string server_host;
    std::uint16_t server_port{network::NETWORK_DEFAULT_GAME_PORT};
    std::string player_name;
    bool headless{false};
    std::string coordinator_host{"coordinator.openttd.org"};
    std::uint16_t coordinator_port{network::NETWORK_COORDINATOR_SERVER_PORT};
    network::ServerGameType server_game_type{network::ServerGameType::Public};
    std::string invite_code{};
    bool listed_publicly{true};
    bool allow_direct{true};
    bool allow_stun{true};
    bool allow_turn{true};
    std::chrono::seconds heartbeat_interval{std::chrono::seconds{30}};
    std::vector<std::string> advertised_grfs{};
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
    void render_gui_preview() const;
};

std::vector<std::string> discover_local_servers();

} // namespace sotc
