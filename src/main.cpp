#include "client_app.hpp"

#include "network/coordinator_client.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace {

[[nodiscard]] std::string trim_copy(std::string_view value) {
    auto begin = value.begin();
    auto end = value.end();
    while (begin != end && std::isspace(static_cast<unsigned char>(*begin))) {
        ++begin;
    }
    while (end != begin && std::isspace(static_cast<unsigned char>(*(end - 1)))) {
        --end;
    }
    return std::string{begin, end};
}

[[nodiscard]] std::string to_lower_copy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

[[nodiscard]] bool parse_bool(std::string_view value, bool &out) {
    const auto lowered = to_lower_copy(std::string{value});
    if (lowered == "true" || lowered == "yes" || lowered == "on" || lowered == "1") {
        out = true;
        return true;
    }
    if (lowered == "false" || lowered == "no" || lowered == "off" || lowered == "0") {
        out = false;
        return true;
    }
    return false;
}

[[nodiscard]] bool parse_uint16(std::string_view value, std::uint16_t &out) {
    try {
        const unsigned long parsed = std::stoul(std::string{value}, nullptr, 10);
        if (parsed > 0xFFFFUL) {
            return false;
        }
        out = static_cast<std::uint16_t>(parsed);
        return true;
    } catch (const std::exception &) {
        return false;
    }
}

[[nodiscard]] bool parse_seconds(std::string_view value, std::chrono::seconds &out) {
    try {
        const long long parsed = std::stoll(std::string{value}, nullptr, 10);
        if (parsed < 0) {
            return false;
        }
        out = std::chrono::seconds{parsed};
        return true;
    } catch (const std::exception &) {
        return false;
    }
}

[[nodiscard]] bool parse_server_game_type(std::string_view value, sotc::network::ServerGameType &out) {
    const auto lowered = to_lower_copy(std::string{value});
    if (lowered == "public") {
        out = sotc::network::ServerGameType::Public;
        return true;
    }
    if (lowered == "friends" || lowered == "friends_only" || lowered == "friends-only") {
        out = sotc::network::ServerGameType::FriendsOnly;
        return true;
    }
    if (lowered == "invite" || lowered == "invite_only" || lowered == "invite-only") {
        out = sotc::network::ServerGameType::InviteOnly;
        return true;
    }
    return false;
}

[[nodiscard]] bool parse_host_and_port(std::string_view value, std::string &host_out, std::uint16_t &port_out) {
    std::string host;
    std::string port_str;

    if (!value.empty() && value.front() == '[') {
        const auto closing = value.find(']');
        if (closing != std::string_view::npos) {
            host = std::string{value.substr(1, closing - 1)};
            if (closing + 1 < value.size() && value[closing + 1] == ':') {
                port_str = std::string{value.substr(closing + 2)};
            }
        } else {
            host = std::string{value};
        }
    } else {
        const auto last_colon = value.rfind(':');
        if (last_colon != std::string_view::npos) {
            host = std::string{value.substr(0, last_colon)};
            port_str = std::string{value.substr(last_colon + 1)};
        } else {
            host = std::string{value};
        }
    }

    if (!port_str.empty()) {
        std::uint16_t parsed_port = 0;
        if (!parse_uint16(port_str, parsed_port)) {
            return false;
        }
        port_out = parsed_port;
    }

    host_out = trim_copy(host);
    return true;
}

void print_help() {
    std::cout << "Simple OpenTTD Client usage:\n"
              << "  sotc_client [options] [server_host] [player_name]\n\n"
              << "Options:\n"
              << "  -h, --help                 Show this help message.\n"
              << "  -D, --headless             Enable headless (dedicated) mode.\n"
              << "      --no-headless          Disable headless mode.\n"
              << "      --server HOST[:PORT]   Set preferred server host and optional port.\n"
              << "      --server-host HOST     Set preferred server host.\n"
              << "      --server-port PORT     Set preferred server port.\n"
              << "      --player NAME          Set player display name.\n"
              << "      --coordinator HOST[:PORT]  Set coordinator endpoint.\n"
              << "      --coordinator-host HOST    Set coordinator host.\n"
              << "      --coordinator-port PORT    Set coordinator port.\n"
              << "      --game-type TYPE       Set server game type (public, friends, invite).\n"
              << "      --invite-code CODE     Set coordinator invite code.\n"
              << "      --public               Allow public listing.\n"
              << "      --private              Disable public listing.\n"
              << "      --allow-direct         Enable direct UDP connectivity.\n"
              << "      --no-direct            Disable direct UDP connectivity.\n"
              << "      --allow-stun           Enable STUN assistance.\n"
              << "      --no-stun              Disable STUN assistance.\n"
              << "      --allow-turn           Enable TURN relaying.\n"
              << "      --no-turn              Disable TURN relaying.\n"
              << "      --heartbeat SECONDS    Set coordinator heartbeat interval.\n"
              << "      --advertised-grf ID    Add an advertised NewGRF identifier.\n"
              << "      --clear-advertised-grfs  Remove previously advertised NewGRFs.\n"
              << "      --config FILE          Load options from a configuration file.\n"
              << "      --dump-launch-options  Emit key=value launch configuration and exit.\n"
              << "      --dump-registration    Emit coordinator registration payload summary and exit.\n";
}

void emit_launch_summary(const sotc::LaunchOptions &options) {
    const auto join_grfs = [](const std::vector<std::string> &items) {
        std::ostringstream oss;
        for (std::size_t i = 0; i < items.size(); ++i) {
            if (i != 0) {
                oss << ',';
            }
            oss << items[i];
        }
        return oss.str();
    };

    std::cout << "server_host=" << options.server_host << '\n';
    std::cout << "server_port=" << options.server_port << '\n';
    std::cout << "player_name=" << options.player_name << '\n';
    std::cout << "headless=" << (options.headless ? "true" : "false") << '\n';
    std::cout << "coordinator_host=" << options.coordinator_host << '\n';
    std::cout << "coordinator_port=" << options.coordinator_port << '\n';
    std::cout << "server_game_type=";
    switch (options.server_game_type) {
    case sotc::network::ServerGameType::Public:
        std::cout << "public";
        break;
    case sotc::network::ServerGameType::FriendsOnly:
        std::cout << "friends";
        break;
    case sotc::network::ServerGameType::InviteOnly:
        std::cout << "invite";
        break;
    }
    std::cout << '\n';
    std::cout << "invite_code=" << options.invite_code << '\n';
    std::cout << "listed_publicly=" << (options.listed_publicly ? "true" : "false") << '\n';
    std::cout << "allow_direct=" << (options.allow_direct ? "true" : "false") << '\n';
    std::cout << "allow_stun=" << (options.allow_stun ? "true" : "false") << '\n';
    std::cout << "allow_turn=" << (options.allow_turn ? "true" : "false") << '\n';
    std::cout << "heartbeat_interval=" << options.heartbeat_interval.count() << '\n';
    std::cout << "advertised_grfs=" << join_grfs(options.advertised_grfs) << '\n';
}

void emit_registration_summary(const sotc::LaunchOptions &options) {
    sotc::network::CoordinatorClient coordinator{};
    sotc::network::RegistrationConfig config{};

    config.server_name = options.player_name.empty() ? std::string{"Simple OpenTTD Client"}
                                                     : options.player_name + "'s game";
    config.coordinator_host = options.coordinator_host.empty() ? std::string{"coordinator.openttd.org"}
                                                              : options.coordinator_host;
    config.coordinator_port = options.coordinator_port == 0
                                  ? sotc::network::NETWORK_COORDINATOR_SERVER_PORT
                                  : options.coordinator_port;
    config.listen_port = options.server_port;
    config.listed_publicly = options.listed_publicly && !options.headless;
    config.server_game_type = options.server_game_type;
    config.invite_code = options.invite_code;
    config.allow_direct = options.allow_direct;
    config.allow_stun = options.allow_stun;
    config.allow_turn = options.allow_turn;
    config.heartbeat_interval = options.heartbeat_interval;
    config.advertised_grfs = options.advertised_grfs;

    const auto frame = coordinator.build_registration_frame(config);
    const auto payload = frame.serialize();

    std::cout << "coordinator_version=" << static_cast<int>(frame.coordinator_version) << '\n';
    std::cout << "game_info_version=" << static_cast<int>(frame.game_info_version) << '\n';
    std::cout << "admin_version=" << static_cast<int>(frame.admin_version) << '\n';
    std::cout << "listen_port=" << frame.listen_port << '\n';
    std::cout << "heartbeat_seconds=" << frame.heartbeat_seconds << '\n';
    std::cout << "server_game_type=" << static_cast<int>(frame.server_game_type) << '\n';
    std::cout << "nat_capabilities=" << static_cast<int>(frame.nat_capabilities) << '\n';
    std::cout << "public_listing=" << (frame.public_listing ? "true" : "false") << '\n';
    std::cout << "server_name=" << frame.server_name << '\n';
    std::cout << "invite_code=" << frame.invite_code << '\n';
    std::cout << "newgrfs=";
    for (std::size_t i = 0; i < frame.newgrfs.size(); ++i) {
        if (i != 0) {
            std::cout << ',';
        }
        std::cout << frame.newgrfs[i];
    }
    std::cout << '\n';

    std::ostringstream payload_stream;
    payload_stream << std::hex << std::setfill('0');
    for (const auto byte : payload) {
        payload_stream << std::setw(2) << static_cast<int>(std::to_integer<unsigned int>(byte));
    }
    std::cout << "payload_hex=" << payload_stream.str() << '\n';
}

enum class ConfigKeyApplyResult {
    Applied,
    InvalidValue,
    Unknown,
};

[[nodiscard]] bool is_known_config_key(std::string_view key) {
    static constexpr std::array known_keys{
        std::string_view{"server_host"},
        std::string_view{"server_port"},
        std::string_view{"player_name"},
        std::string_view{"headless"},
        std::string_view{"coordinator_host"},
        std::string_view{"coordinator_port"},
        std::string_view{"server_game_type"},
        std::string_view{"game_type"},
        std::string_view{"invite_code"},
        std::string_view{"listed_publicly"},
        std::string_view{"allow_direct"},
        std::string_view{"allow_stun"},
        std::string_view{"allow_turn"},
        std::string_view{"heartbeat_interval"},
        std::string_view{"advertised_grfs"},
    };

    return std::find(known_keys.begin(), known_keys.end(), key) != known_keys.end();
}

ConfigKeyApplyResult apply_config_key(std::string_view key, std::string_view value, sotc::LaunchOptions &options) {
    if (key == "server_host") {
        options.server_host = std::string{value};
        return ConfigKeyApplyResult::Applied;
    }
    if (key == "server_port") {
        std::uint16_t port = 0;
        if (!parse_uint16(value, port)) {
            std::cerr << "Invalid server_port value: " << value << '\n';
            return ConfigKeyApplyResult::InvalidValue;
        }
        options.server_port = port;
        return ConfigKeyApplyResult::Applied;
    }
    if (key == "player_name") {
        options.player_name = std::string{value};
        return ConfigKeyApplyResult::Applied;
    }
    if (key == "headless") {
        bool flag = false;
        if (!parse_bool(value, flag)) {
            std::cerr << "Invalid headless value: " << value << '\n';
            return ConfigKeyApplyResult::InvalidValue;
        }
        options.headless = flag;
        return ConfigKeyApplyResult::Applied;
    }
    if (key == "coordinator_host") {
        options.coordinator_host = std::string{value};
        return ConfigKeyApplyResult::Applied;
    }
    if (key == "coordinator_port") {
        std::uint16_t port = 0;
        if (!parse_uint16(value, port)) {
            std::cerr << "Invalid coordinator_port value: " << value << '\n';
            return ConfigKeyApplyResult::InvalidValue;
        }
        options.coordinator_port = port;
        return ConfigKeyApplyResult::Applied;
    }
    if (key == "server_game_type" || key == "game_type") {
        sotc::network::ServerGameType type = sotc::network::ServerGameType::Public;
        if (!parse_server_game_type(value, type)) {
            std::cerr << "Invalid server_game_type value: " << value << '\n';
            return ConfigKeyApplyResult::InvalidValue;
        }
        options.server_game_type = type;
        return ConfigKeyApplyResult::Applied;
    }
    if (key == "invite_code") {
        options.invite_code = std::string{value};
        return ConfigKeyApplyResult::Applied;
    }
    if (key == "listed_publicly") {
        bool flag = false;
        if (!parse_bool(value, flag)) {
            std::cerr << "Invalid listed_publicly value: " << value << '\n';
            return ConfigKeyApplyResult::InvalidValue;
        }
        options.listed_publicly = flag;
        return ConfigKeyApplyResult::Applied;
    }
    if (key == "allow_direct") {
        bool flag = false;
        if (!parse_bool(value, flag)) {
            std::cerr << "Invalid allow_direct value: " << value << '\n';
            return ConfigKeyApplyResult::InvalidValue;
        }
        options.allow_direct = flag;
        return ConfigKeyApplyResult::Applied;
    }
    if (key == "allow_stun") {
        bool flag = false;
        if (!parse_bool(value, flag)) {
            std::cerr << "Invalid allow_stun value: " << value << '\n';
            return ConfigKeyApplyResult::InvalidValue;
        }
        options.allow_stun = flag;
        return ConfigKeyApplyResult::Applied;
    }
    if (key == "allow_turn") {
        bool flag = false;
        if (!parse_bool(value, flag)) {
            std::cerr << "Invalid allow_turn value: " << value << '\n';
            return ConfigKeyApplyResult::InvalidValue;
        }
        options.allow_turn = flag;
        return ConfigKeyApplyResult::Applied;
    }
    if (key == "heartbeat_interval") {
        std::chrono::seconds heartbeat{};
        if (!parse_seconds(value, heartbeat)) {
            std::cerr << "Invalid heartbeat_interval value: " << value << '\n';
            return ConfigKeyApplyResult::InvalidValue;
        }
        options.heartbeat_interval = heartbeat;
        return ConfigKeyApplyResult::Applied;
    }
    if (key == "advertised_grfs") {
        options.advertised_grfs.clear();
        std::istringstream iss{std::string{value}};
        std::string token;
        while (std::getline(iss, token, ',')) {
            token = trim_copy(token);
            if (!token.empty()) {
                options.advertised_grfs.push_back(std::move(token));
            }
        }
        return ConfigKeyApplyResult::Applied;
    }
    return ConfigKeyApplyResult::Unknown;
}

bool load_config_file(const std::string &path, sotc::LaunchOptions &options) {
    std::ifstream input{path};
    if (!input) {
        std::cerr << "Failed to open configuration file: " << path << '\n';
        return false;
    }

    std::string line;
    std::size_t line_number = 0;
    bool success = true;
    std::unordered_set<std::string> seen_keys;

    while (std::getline(input, line)) {
        ++line_number;
        const auto trimmed = trim_copy(line);
        if (trimmed.empty() || trimmed.front() == '#' || trimmed.front() == ';') {
            continue;
        }
        const auto equals = trimmed.find('=');
        if (equals == std::string::npos) {
            std::cerr << "Ignoring malformed config line " << line_number << " in " << path << '\n';
            continue;
        }
        const auto key = trim_copy(std::string_view{trimmed}.substr(0, equals));
        const auto value = trim_copy(std::string_view{trimmed}.substr(equals + 1));
        const std::string key_string{key};

        if (is_known_config_key(key)) {
            const auto insertion = seen_keys.insert(key_string);
            if (!insertion.second) {
                std::cerr << "Duplicate configuration key '" << key << "' at line " << line_number << '\n';
                success = false;
                continue;
            }
        }

        switch (apply_config_key(key, value, options)) {
        case ConfigKeyApplyResult::Applied:
            break;
        case ConfigKeyApplyResult::InvalidValue:
            success = false;
            break;
        case ConfigKeyApplyResult::Unknown:
            if (!key_string.empty()) {
                std::cerr << "Unknown configuration key '" << key_string << "' at line " << line_number << '\n';
                success = false;
            }
            break;
        }
    }

    return success;
}

} // namespace

int main(int argc, char **argv) {
    sotc::LaunchOptions options{};
    bool dump_launch_options = false;
    bool dump_registration = false;

    std::vector<std::string> positionals;

    for (int index = 1; index < argc; ++index) {
        const std::string current{argv[index]};
        if (current == "-h" || current == "--help") {
            print_help();
            return 0;
        }
        if (current == "-D" || current == "--headless") {
            options.headless = true;
            continue;
        }
        if (current == "--no-headless") {
            options.headless = false;
            continue;
        }
        if (current == "--public") {
            options.listed_publicly = true;
            continue;
        }
        if (current == "--private") {
            options.listed_publicly = false;
            continue;
        }
        if (current == "--allow-direct") {
            options.allow_direct = true;
            continue;
        }
        if (current == "--no-direct") {
            options.allow_direct = false;
            continue;
        }
        if (current == "--allow-stun") {
            options.allow_stun = true;
            continue;
        }
        if (current == "--no-stun") {
            options.allow_stun = false;
            continue;
        }
        if (current == "--allow-turn") {
            options.allow_turn = true;
            continue;
        }
        if (current == "--no-turn") {
            options.allow_turn = false;
            continue;
        }
        if (current == "--dump-launch-options") {
            dump_launch_options = true;
            continue;
        }
        if (current == "--dump-registration") {
            dump_registration = true;
            continue;
        }
        if (current == "--clear-advertised-grfs") {
            options.advertised_grfs.clear();
            continue;
        }

        auto require_value = [&](std::string_view option_name) -> std::string {
            if (index + 1 >= argc) {
                std::cerr << "Missing value for option " << option_name << '\n';
                throw std::runtime_error{"missing option value"};
            }
            ++index;
            return std::string{argv[index]};
        };

        try {
            if (current == "--server") {
                const auto value = require_value(current);
                if (!parse_host_and_port(value, options.server_host, options.server_port)) {
                    std::cerr << "Invalid server endpoint: " << value << '\n';
                    return 1;
                }
                continue;
            }
            if (current == "--server-host") {
                options.server_host = require_value(current);
                continue;
            }
            if (current == "--server-port") {
                const auto value = require_value(current);
                std::uint16_t port = 0;
                if (!parse_uint16(value, port)) {
                    std::cerr << "Invalid server port: " << value << '\n';
                    return 1;
                }
                options.server_port = port;
                continue;
            }
            if (current == "--player" || current == "--player-name") {
                options.player_name = require_value(current);
                continue;
            }
            if (current == "--coordinator") {
                const auto value = require_value(current);
                if (!parse_host_and_port(value, options.coordinator_host, options.coordinator_port)) {
                    std::cerr << "Invalid coordinator endpoint: " << value << '\n';
                    return 1;
                }
                continue;
            }
            if (current == "--coordinator-host") {
                options.coordinator_host = require_value(current);
                continue;
            }
            if (current == "--coordinator-port") {
                const auto value = require_value(current);
                std::uint16_t port = 0;
                if (!parse_uint16(value, port)) {
                    std::cerr << "Invalid coordinator port: " << value << '\n';
                    return 1;
                }
                options.coordinator_port = port;
                continue;
            }
            if (current == "--game-type") {
                const auto value = require_value(current);
                sotc::network::ServerGameType type = sotc::network::ServerGameType::Public;
                if (!parse_server_game_type(value, type)) {
                    std::cerr << "Invalid game type: " << value << '\n';
                    return 1;
                }
                options.server_game_type = type;
                continue;
            }
            if (current == "--invite-code") {
                options.invite_code = require_value(current);
                continue;
            }
            if (current == "--heartbeat") {
                const auto value = require_value(current);
                std::chrono::seconds heartbeat{};
                if (!parse_seconds(value, heartbeat)) {
                    std::cerr << "Invalid heartbeat interval: " << value << '\n';
                    return 1;
                }
                options.heartbeat_interval = heartbeat;
                continue;
            }
            if (current == "--advertised-grf") {
                auto value = require_value(current);
                if (!value.empty()) {
                    options.advertised_grfs.push_back(std::move(value));
                }
                continue;
            }
            if (current == "--config") {
                const auto path = require_value(current);
                if (!load_config_file(path, options)) {
                    return 1;
                }
                continue;
            }
        } catch (const std::runtime_error &) {
            return 1;
        }

        if (!current.empty() && current.front() == '-') {
            std::cerr << "Unknown option: " << current << '\n';
            return 1;
        }
        positionals.push_back(current);
    }

    if (!positionals.empty()) {
        options.server_host = positionals.front();
        if (positionals.size() > 1) {
            options.player_name = positionals[1];
        }
    }

    if (dump_launch_options || dump_registration) {
        if (dump_launch_options) {
            emit_launch_summary(options);
        }
        if (dump_registration) {
            emit_registration_summary(options);
        }
        return 0;
    }

    sotc::ClientApp app;
    app.configure(options);
    app.run();
    return 0;
}
