#pragma once

#include "network/constants.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace sotc::network {

enum class ServerGameType : std::uint8_t {
    Public = 0,
    FriendsOnly = 1,
    InviteOnly = 2,
};

enum class NatCapability : std::uint8_t {
    Direct = 0x01,
    Stun = 0x02,
    Turn = 0x04,
};

inline constexpr NatCapability operator|(NatCapability lhs, NatCapability rhs) {
    return static_cast<NatCapability>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
}

inline constexpr NatCapability operator&(NatCapability lhs, NatCapability rhs) {
    return static_cast<NatCapability>(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
}

struct RegistrationConfig {
    std::string server_name{"Simple OpenTTD Client"};
    std::string coordinator_host{"coordinator.openttd.org"};
    std::uint16_t coordinator_port{NETWORK_COORDINATOR_SERVER_PORT};
    std::uint16_t listen_port{NETWORK_DEFAULT_GAME_PORT};
    std::string invite_code{};
    std::vector<std::string> advertised_grfs{};
    ServerGameType server_game_type{ServerGameType::Public};
    bool allow_direct{true};
    bool allow_stun{true};
    bool allow_turn{true};
    bool listed_publicly{true};
    std::chrono::seconds heartbeat_interval{std::chrono::seconds{30}};
};

struct CoordinatorHandshakeFrame {
    std::uint8_t coordinator_version{NETWORK_COORDINATOR_VERSION};
    std::uint8_t game_info_version{NETWORK_GAME_INFO_VERSION};
    std::uint8_t admin_version{NETWORK_GAME_ADMIN_VERSION};
    std::uint16_t listen_port{NETWORK_DEFAULT_GAME_PORT};
    std::uint16_t heartbeat_seconds{30};
    std::uint8_t server_game_type{static_cast<std::uint8_t>(ServerGameType::Public)};
    std::uint8_t nat_capabilities{static_cast<std::uint8_t>(NatCapability::Direct) | static_cast<std::uint8_t>(NatCapability::Stun)};
    std::uint8_t public_listing{1U};
    std::string server_name{};
    std::string invite_code{};
    std::vector<std::string> newgrfs{};

    [[nodiscard]] std::vector<std::byte> serialize() const;
};

class CoordinatorClient {
public:
    CoordinatorClient();

    [[nodiscard]] CoordinatorHandshakeFrame build_registration_frame(const RegistrationConfig &config) const;
};

[[nodiscard]] std::string describe_capabilities(std::uint8_t nat_capabilities);

} // namespace sotc::network
