#include "client_app.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <thread>

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
    registration.server_name = options_.player_name.empty()
                                  ? std::string{"Simple OpenTTD Client"}
                                  : options_.player_name + "'s game";
    registration.listen_port = static_cast<std::uint16_t>(options_.server_port);
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
    std::cout << "  Player: " << (options_.player_name.empty() ? "<anonymous>" : options_.player_name) << '\n';
    std::cout << "  Headless: " << (options_.headless ? "yes" : "no") << '\n';
    std::cout << "  Game type: " << to_string(options_.server_game_type) << '\n';
    std::cout << "  Public listing: " << (options_.listed_publicly ? "yes" : "no") << '\n';
    std::cout << "  Invite code: " << (options_.invite_code.empty() ? "<not set>" : options_.invite_code) << '\n';
    std::cout << "  NAT: " << describe_nat_policy(options_) << '\n';
    std::cout << "  Heartbeat: every " << options_.heartbeat_interval.count() << "s" << std::endl;
}

void ClientApp::render_gui_preview() const {
    std::cout << "\n=== Session Configuration ===\n";
    std::cout << "Player identity: "
              << (options_.player_name.empty() ? "<anonymous>" : options_.player_name) << '\n';
    std::cout << "Preferred server: "
              << (options_.server_host.empty() ? "<not set>" : options_.server_host)
              << ':' << options_.server_port << "\n\n";

    std::cout << "[Server Visibility]\n";
    std::cout << "  Game type: " << to_string(options_.server_game_type) << '\n';
    std::cout << "  Listed on coordinator: " << (options_.listed_publicly ? "yes" : "no") << '\n';
    if (!options_.invite_code.empty()) {
        std::cout << "  Invite code: " << options_.invite_code << '\n';
    } else {
        std::cout << "  Invite code: <not set>\n";
    }

    std::cout << "\n[Connectivity]\n";
    std::cout << "  Allowed pathways: " << describe_nat_policy(options_) << '\n';
    std::cout << "  Coordinator heartbeat: every " << options_.heartbeat_interval.count() << "s\n";

    std::cout << "\n[Advertised Content]\n";
    if (options_.advertised_grfs.empty()) {
        std::cout << "  No NewGRFs configured for advertising.\n";
    } else {
        for (const auto &grf_id : options_.advertised_grfs) {
            std::cout << "  - " << grf_id << '\n';
        }
    }

    std::cout << '\n';
}

std::vector<std::string> discover_local_servers() {
    std::cout << "Server discovery is not implemented yet." << std::endl;
    return {};
}

} // namespace sotc
