#include "client_app.hpp"

#include <chrono>
#include <iostream>
#include <thread>

namespace sotc {

ClientApp::ClientApp() = default;

void ClientApp::configure(LaunchOptions options) {
    options_ = std::move(options);
}

void ClientApp::run() {
    log_startup_info();
    std::cout << "Simple OpenTTD Client scaffold running." << std::endl;
    std::cout << "Networking and rendering subsystems are not yet implemented." << std::endl;

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
    std::cout << "  Headless: " << (options_.headless ? "yes" : "no") << std::endl;
}

std::vector<std::string> discover_local_servers() {
    std::cout << "Server discovery is not implemented yet." << std::endl;
    return {};
}

} // namespace sotc
