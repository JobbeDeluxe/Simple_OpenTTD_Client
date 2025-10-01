#include "client_app.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <thread>
#include <utility>

#include "gui/configuration_preview.hpp"
#include "network/coordinator_client.hpp"

namespace sotc {

namespace {

[[nodiscard]] std::string_view to_string(network::ServerGameType type) {
    using network::ServerGameType;
    switch (type) {
    case ServerGameType::Public:
        return "Public";
    case ServerGameType::FriendsOnly:
        return "Friends only";
    case ServerGameType::InviteOnly:
        return "Invite only";
    }
    return "Unknown";
}

[[nodiscard]] std::string describe_nat_policy(const LaunchOptions &options) {
    std::string description;
    if (options.allow_direct) {
        description += "Direct";
    }
    if (options.allow_stun) {
        if (!description.empty()) description += ", ";
        description += "STUN";
    }
    if (options.allow_turn) {
        if (!description.empty()) description += ", ";
        description += "TURN";
    }
    if (description.empty()) {
        description = "None";
    }
    return description;
}

[[nodiscard]] std::string build_server_name(const LaunchOptions &options) {
    if (options.player_name.empty()) {
        return std::string{"Simple OpenTTD Client"};
    }
    return options.player_name + "'s game";
}

[[nodiscard]] std::string format_endpoint(const std::string &host, std::uint16_t port) {
    std::string endpoint = host.empty() ? std::string{"<not set>"} : host;
    endpoint += ':';
    endpoint += std::to_string(port);
    return endpoint;
}

} // namespace

ClientApp::ClientApp() = default;

void ClientApp::configure(LaunchOptions options) {
    options_ = std::move(options);
}

void ClientApp::run() {
    log_startup_info();
    render_gui_preview();
    std::cout << "Simple OpenTTD Client scaffold running." << std::endl;
    std::cout << "Networking and rendering subsystems are not yet implemented." << std::endl;

    sotc::network::CoordinatorClient coordinator{};
    sotc::network::RegistrationConfig registration{};
    registration.server_name = build_server_name(options_);
    registration.coordinator_host = options_.coordinator_host.empty()
                                      ? std::string{"coordinator.openttd.org"}
                                      : options_.coordinator_host;
    registration.coordinator_port = options_.coordinator_port == 0
                                        ? network::NETWORK_COORDINATOR_SERVER_PORT
                                        : options_.coordinator_port;
    registration.listen_port = options_.server_port;
    registration.listed_publicly = options_.listed_publicly && !options_.headless;
    registration.server_game_type = options_.server_game_type;
    registration.invite_code = options_.invite_code;
    registration.allow_direct = options_.allow_direct;
    registration.allow_stun = options_.allow_stun;
    registration.allow_turn = options_.allow_turn;
    registration.heartbeat_interval = options_.heartbeat_interval;
    registration.advertised_grfs = options_.advertised_grfs;

    auto frame = coordinator.build_registration_frame(registration);
    auto payload = frame.serialize();

    std::cout << "Prepared coordinator registration payload targeting " << registration.coordinator_host << ':'
              << registration.coordinator_port << " (" << payload.size() << " bytes)." << std::endl;
    std::cout << "  Server name: " << frame.server_name << std::endl;
    std::cout << "  NAT capabilities: " << network::describe_capabilities(frame.nat_capabilities) << std::endl;
    std::cout << "  Public listing: " << (frame.public_listing ? "enabled" : "disabled") << std::endl;

    std::cout << "  Payload preview:";
    std::cout << std::hex << std::setfill('0');
    for (std::size_t i = 0; i < payload.size() && i < 32; ++i) {
        if (i % 8 == 0) {
            std::cout << '\n' << "    ";
        }
        std::cout << " 0x" << std::setw(2)
                  << static_cast<int>(std::to_integer<std::uint8_t>(payload[i]));
    }
    if (payload.size() > 32) {
        std::cout << " ...";
    }
    std::cout << std::dec << std::setfill(' ') << '\n';

    if (options_.headless) {
        std::cout << "Headless mode enabled; exiting immediately." << std::endl;
        return;
    }

    using namespace std::chrono_literals;
    std::cout << "Simulating main loop (press Ctrl+C to exit)." << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(200ms);
        std::cout << "." << std::flush;
    }
    std::cout << std::endl;
}

void ClientApp::log_startup_info() const {
    std::cout << "Launching client with configuration:\n";
    std::cout << "  Server: " << (options_.server_host.empty() ? "<not set>" : options_.server_host)
              << ':' << options_.server_port << '\n';
    std::cout << "  Coordinator: " << format_endpoint(options_.coordinator_host, options_.coordinator_port)
              << '\n';
    std::cout << "  Player: " << (options_.player_name.empty() ? "<anonymous>" : options_.player_name) << '\n';
    std::cout << "  Advertised name: " << build_server_name(options_) << '\n';
    std::cout << "  Headless: " << (options_.headless ? "yes" : "no") << '\n';
    std::cout << "  Game type: " << to_string(options_.server_game_type) << '\n';
    std::cout << "  Public listing: " << (options_.listed_publicly ? "yes" : "no") << '\n';
    std::cout << "  Invite code: " << (options_.invite_code.empty() ? "<not set>" : options_.invite_code) << '\n';
    std::cout << "  NAT: " << describe_nat_policy(options_) << '\n';
    std::cout << "  Heartbeat: every " << options_.heartbeat_interval.count() << "s" << std::endl;
}

void ClientApp::render_gui_preview() const {
    using sotc::ui::FieldLine;
    using sotc::ui::Section;
    using sotc::ui::ToggleLine;

    const bool listing_enabled = options_.listed_publicly && !options_.headless;

    Section overview{};
    overview.title = "Session Overview";
    overview.fields.push_back(FieldLine{"Player identity", options_.player_name.empty() ? "<anonymous>" : options_.player_name});
    overview.fields.push_back(FieldLine{"Advertised server", build_server_name(options_)});
    overview.fields.push_back(FieldLine{"Preferred server", format_endpoint(options_.server_host, options_.server_port)});
    overview.fields.push_back(FieldLine{"Launch mode", options_.headless ? "Headless (no GUI)" : "Interactive"});

    Section registration{};
    registration.title = "Coordinator Registration";
    registration.fields.push_back(FieldLine{"Coordinator endpoint", format_endpoint(options_.coordinator_host, options_.coordinator_port)});
    registration.fields.push_back(FieldLine{"Listing visibility", listing_enabled ? "Public listing" : "Hidden"});
    registration.fields.push_back(FieldLine{"Game type", std::string{to_string(options_.server_game_type)}});
    registration.fields.push_back(FieldLine{"Invite code", options_.invite_code.empty() ? "<not set>" : options_.invite_code});
    registration.fields.push_back(FieldLine{"Heartbeat interval", std::to_string(options_.heartbeat_interval.count()) + "s"});

    if (!options_.listed_publicly) {
        registration.notes.emplace_back("Server will not appear in coordinator listings; share connection details manually.");
    }
    if (options_.headless && options_.listed_publicly) {
        registration.notes.emplace_back("Headless mode suppresses public listings for safety.");
    }
    switch (options_.server_game_type) {
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
    connectivity.toggles.push_back(ToggleLine{"Direct UDP", options_.allow_direct, "Attempt direct client connections when NAT allows."});
    connectivity.toggles.push_back(ToggleLine{"STUN assist", options_.allow_stun, "Use coordinator STUN services for UDP hole punching."});
    connectivity.toggles.push_back(ToggleLine{"TURN relay", options_.allow_turn, "Relay traffic through coordinator TURN servers when direct paths fail."});
    connectivity.notes.emplace_back("Effective NAT capabilities: " + describe_nat_policy(options_));
    if (!options_.allow_direct && !options_.allow_stun && !options_.allow_turn) {
        connectivity.notes.emplace_back("No connectivity pathways selected; clients will be unable to reach this server.");
    } else if (!options_.allow_turn) {
        connectivity.notes.emplace_back("TURN is disabled; players behind strict NATs may encounter connection issues.");
    }

    Section content{};
    content.title = "Advertised Content";
    if (options_.advertised_grfs.empty()) {
        content.notes.emplace_back("No NewGRFs configured for coordinator advertising.");
    } else {
        for (const auto &grf_id : options_.advertised_grfs) {
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

    std::cout << '\n' << ui::render_sections(sections) << std::endl;
}

std::vector<std::string> discover_local_servers() {
    std::cout << "Server discovery is not implemented yet." << std::endl;
    return {};
}

} // namespace sotc
