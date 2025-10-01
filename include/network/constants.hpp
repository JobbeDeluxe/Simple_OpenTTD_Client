#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc::network {

inline constexpr std::uint16_t NETWORK_COORDINATOR_SERVER_PORT = 3976;
inline constexpr std::uint16_t NETWORK_DEFAULT_GAME_PORT = 3979;

inline constexpr std::uint8_t NETWORK_COORDINATOR_VERSION = 6;
inline constexpr std::uint8_t NETWORK_GAME_INFO_VERSION = 7;
inline constexpr std::uint8_t NETWORK_GAME_ADMIN_VERSION = 3;

inline constexpr std::size_t NETWORK_GAMESCRIPT_JSON_LENGTH = 9000;
inline constexpr std::size_t NETWORK_MAX_GRF_COUNT = 255;

inline constexpr std::size_t NETWORK_MAX_SERVER_NAME_LENGTH = 255;
inline constexpr std::size_t NETWORK_MAX_INVITE_CODE_LENGTH = 63;

} // namespace sotc::network
