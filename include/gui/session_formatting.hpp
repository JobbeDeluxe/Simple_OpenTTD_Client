#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "network/coordinator_client.hpp"

namespace sotc::ui {

[[nodiscard]] std::string to_string(network::ServerGameType type);

[[nodiscard]] std::string build_server_name(std::string_view player_name,
                                            std::string_view fallback = "Simple OpenTTD Client");

[[nodiscard]] std::string format_endpoint(const std::string &host, std::uint16_t port);

[[nodiscard]] std::string describe_nat_policy(bool allow_direct, bool allow_stun, bool allow_turn);

} // namespace sotc::ui

