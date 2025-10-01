#include "client_app.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <thread>
#include <utility>

#include "gui/configuration_preview.hpp"
#include "gui/coordinator_settings_window.hpp"
#include "gui/session_formatting.hpp"
#include "network/coordinator_client.hpp"

namespace sotc {

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
    registration.server_name = ui::build_server_name(options_.player_name);
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
    std::cout << "  Server: " << ui::format_endpoint(options_.server_host, options_.server_port) << '\n';
    std::cout << "  Coordinator: " << ui::format_endpoint(options_.coordinator_host, options_.coordinator_port)
              << '\n';
    std::cout << "  Player: " << (options_.player_name.empty() ? "<anonymous>" : options_.player_name) << '\n';
    std::cout << "  Advertised name: " << ui::build_server_name(options_.player_name) << '\n';
    std::cout << "  Headless: " << (options_.headless ? "yes" : "no") << '\n';
    std::cout << "  Game type: " << ui::to_string(options_.server_game_type) << '\n';
    std::cout << "  Public listing: " << (options_.listed_publicly ? "yes" : "no") << '\n';
    std::cout << "  Invite code: " << (options_.invite_code.empty() ? "<not set>" : options_.invite_code) << '\n';
    std::cout << "  NAT: " << ui::describe_nat_policy(options_.allow_direct, options_.allow_stun, options_.allow_turn) << '\n';
    std::cout << "  Heartbeat: every " << options_.heartbeat_interval.count() << "s" << std::endl;
}

void ClientApp::render_gui_preview() const {
    auto state = ui::build_state_from_launch_options(options_);
    ui::CoordinatorSettingsWindow window{std::move(state)};
    std::cout << '\n' << ui::render_sections(window.build_sections()) << std::endl;
}

std::vector<std::string> discover_local_servers() {
    std::cout << "Server discovery is not implemented yet." << std::endl;
    return {};
}

} // namespace sotc
