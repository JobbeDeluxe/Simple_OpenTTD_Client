#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include "client_app.hpp"
#include "gui/configuration_preview.hpp"

namespace sotc::ui {

struct CoordinatorSettingsState {
    std::string player_name;
    std::string advertised_server_name;
    std::string server_host;
    std::uint16_t server_port{};
    bool headless{false};
    std::string coordinator_host;
    std::uint16_t coordinator_port{};
    network::ServerGameType server_game_type{network::ServerGameType::Public};
    std::string invite_code;
    bool listed_publicly{true};
    bool allow_direct{true};
    bool allow_stun{true};
    bool allow_turn{true};
    std::chrono::seconds heartbeat_interval{std::chrono::seconds{30}};
    std::vector<std::string> advertised_grfs;
};

[[nodiscard]] CoordinatorSettingsState build_state_from_launch_options(const LaunchOptions &options);

class CoordinatorSettingsWindow {
public:
    explicit CoordinatorSettingsWindow(CoordinatorSettingsState state);

    [[nodiscard]] const CoordinatorSettingsState &state() const noexcept { return state_; }

    void set_headless(bool headless) noexcept { state_.headless = headless; }
    void set_listed_publicly(bool listed) noexcept { state_.listed_publicly = listed; }
    void set_server_game_type(network::ServerGameType type) noexcept { state_.server_game_type = type; }
    void update_nat_capabilities(bool allow_direct, bool allow_stun, bool allow_turn) noexcept;
    void set_advertised_grfs(std::vector<std::string> grfs) noexcept;

    [[nodiscard]] std::vector<Section> build_sections() const;

private:
    CoordinatorSettingsState state_{};
};

} // namespace sotc::ui

