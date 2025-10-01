#include "gui/coordinator_settings_window.hpp"

#include <utility>

#include "gui/session_formatting.hpp"

namespace sotc::ui {

namespace {

[[nodiscard]] ToggleLine make_toggle(std::string label, bool enabled, std::string hint) {
    ToggleLine toggle{};
    toggle.label = std::move(label);
    toggle.enabled = enabled;
    toggle.hint = std::move(hint);
    return toggle;
}

[[nodiscard]] std::string invite_code_value(const CoordinatorSettingsState &state) {
    return state.invite_code.empty() ? std::string{"<not set>"} : state.invite_code;
}

[[nodiscard]] std::string player_identity(const CoordinatorSettingsState &state) {
    return state.player_name.empty() ? std::string{"<anonymous>"} : state.player_name;
}

} // namespace

CoordinatorSettingsState build_state_from_launch_options(const LaunchOptions &options) {
    CoordinatorSettingsState state{};
    state.player_name = options.player_name;
    state.advertised_server_name = build_server_name(options.player_name);
    state.server_host = options.server_host;
    state.server_port = options.server_port;
    state.headless = options.headless;
    state.coordinator_host = options.coordinator_host;
    state.coordinator_port = options.coordinator_port;
    state.server_game_type = options.server_game_type;
    state.invite_code = options.invite_code;
    state.listed_publicly = options.listed_publicly;
    state.allow_direct = options.allow_direct;
    state.allow_stun = options.allow_stun;
    state.allow_turn = options.allow_turn;
    state.heartbeat_interval = options.heartbeat_interval;
    state.advertised_grfs = options.advertised_grfs;
    return state;
}

CoordinatorSettingsWindow::CoordinatorSettingsWindow(CoordinatorSettingsState state)
    : state_(std::move(state)) {}

void CoordinatorSettingsWindow::update_nat_capabilities(bool allow_direct, bool allow_stun, bool allow_turn) noexcept {
    state_.allow_direct = allow_direct;
    state_.allow_stun = allow_stun;
    state_.allow_turn = allow_turn;
}

void CoordinatorSettingsWindow::set_advertised_grfs(std::vector<std::string> grfs) noexcept {
    state_.advertised_grfs = std::move(grfs);
}

std::vector<Section> CoordinatorSettingsWindow::build_sections() const {
    Section overview{};
    overview.title = "Session Overview";
    overview.fields.emplace_back(FieldLine{"Player identity", player_identity(state_)});
    overview.fields.emplace_back(FieldLine{"Advertised server", state_.advertised_server_name});
    overview.fields.emplace_back(FieldLine{"Preferred server", format_endpoint(state_.server_host, state_.server_port)});
    overview.fields.emplace_back(FieldLine{"Launch mode", state_.headless ? "Headless (no GUI)" : "Interactive"});

    const bool listing_enabled = state_.listed_publicly && !state_.headless;

    Section registration{};
    registration.title = "Coordinator Registration";
    registration.fields.emplace_back(FieldLine{"Coordinator endpoint", format_endpoint(state_.coordinator_host, state_.coordinator_port)});
    registration.fields.emplace_back(FieldLine{"Listing visibility", listing_enabled ? "Public listing" : "Hidden"});
    registration.fields.emplace_back(FieldLine{"Game type", to_string(state_.server_game_type)});
    registration.fields.emplace_back(FieldLine{"Invite code", invite_code_value(state_)});
    registration.fields.emplace_back(FieldLine{"Heartbeat interval", std::to_string(state_.heartbeat_interval.count()) + "s"});

    if (!state_.listed_publicly) {
        registration.notes.emplace_back("Server will not appear in coordinator listings; share connection details manually.");
    }
    if (state_.headless && state_.listed_publicly) {
        registration.notes.emplace_back("Headless mode suppresses public listings for safety.");
    }
    switch (state_.server_game_type) {
    case network::ServerGameType::FriendsOnly:
        registration.notes.emplace_back("Friends-only sessions remain discoverable through friend lists but are hidden from the public lobby.");
        break;
    case network::ServerGameType::InviteOnly:
        registration.notes.emplace_back("Invite-only sessions require players to join with the provided invite code.");
        break;
    case network::ServerGameType::Public:
        break;
    }

    Section connectivity{};
    connectivity.title = "Connectivity Options";
    connectivity.toggles.emplace_back(make_toggle("Direct UDP", state_.allow_direct, "Attempt direct client connections when NAT allows."));
    connectivity.toggles.emplace_back(make_toggle("STUN assist", state_.allow_stun, "Use coordinator STUN services for UDP hole punching."));
    connectivity.toggles.emplace_back(make_toggle("TURN relay", state_.allow_turn, "Relay traffic through coordinator TURN servers when direct paths fail."));
    connectivity.notes.emplace_back("Effective NAT capabilities: " + describe_nat_policy(state_.allow_direct, state_.allow_stun, state_.allow_turn));

    if (!state_.allow_direct && !state_.allow_stun && !state_.allow_turn) {
        connectivity.notes.emplace_back("No connectivity pathways selected; clients will be unable to reach this server.");
    } else if (!state_.allow_turn) {
        connectivity.notes.emplace_back("TURN is disabled; players behind strict NATs may encounter connection issues.");
    }

    Section content{};
    content.title = "Advertised Content";
    if (state_.advertised_grfs.empty()) {
        content.notes.emplace_back("No NewGRFs configured for coordinator advertising.");
    } else {
        for (const auto &grf_id : state_.advertised_grfs) {
            content.notes.emplace_back("NewGRF: " + grf_id);
        }
        content.notes.emplace_back("Ensure joining clients have the listed NewGRFs installed.");
    }

    std::vector<Section> sections;
    sections.reserve(4);
    sections.emplace_back(std::move(overview));
    sections.emplace_back(std::move(registration));
    sections.emplace_back(std::move(connectivity));
    sections.emplace_back(std::move(content));

    return sections;
}

} // namespace sotc::ui

