#include "network/coordinator_client.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <span>
#include <vector>

namespace sotc::network {

namespace {

constexpr std::size_t kMaxCoordinatorPayloadLength = 32 * 1024;

[[nodiscard]] std::uint16_t clamp_port(std::uint16_t port) {
    if (port == 0) {
        return NETWORK_DEFAULT_GAME_PORT;
    }
    return port;
}

[[nodiscard]] std::string truncate_string(std::string_view value, std::size_t max_length) {
    if (value.size() <= max_length) {
        return std::string{value};
    }
    return std::string{value.substr(0, max_length)};
}

void append_uint8(std::vector<std::byte> &buffer, std::uint8_t value) {
    buffer.push_back(static_cast<std::byte>(value));
}

void append_uint16_be(std::vector<std::byte> &buffer, std::uint16_t value) {
    buffer.push_back(static_cast<std::byte>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<std::byte>(value & 0xFF));
}

void append_string(std::vector<std::byte> &buffer, const std::string &value) {
    if (value.size() > std::numeric_limits<std::uint16_t>::max()) {
        throw std::length_error{"String too long to serialise into coordinator payload"};
    }
    append_uint16_be(buffer, static_cast<std::uint16_t>(value.size()));
    for (unsigned char ch : value) {
        buffer.push_back(static_cast<std::byte>(ch));
    }
}

[[nodiscard]] std::uint8_t read_uint8(std::span<const std::byte> payload, std::size_t &offset) {
    if (offset >= payload.size()) {
        throw std::out_of_range{"Coordinator payload ended unexpectedly while reading uint8"};
    }
    return std::to_integer<std::uint8_t>(payload[offset++]);
}

[[nodiscard]] std::uint16_t read_uint16_be(std::span<const std::byte> payload, std::size_t &offset) {
    if (offset + 1 >= payload.size()) {
        throw std::out_of_range{"Coordinator payload ended unexpectedly while reading uint16"};
    }
    const auto high = std::to_integer<std::uint8_t>(payload[offset++]);
    const auto low = std::to_integer<std::uint8_t>(payload[offset++]);
    return static_cast<std::uint16_t>((static_cast<std::uint16_t>(high) << 8U) | low);
}

[[nodiscard]] std::string read_string(
    std::span<const std::byte> payload,
    std::size_t &offset,
    std::size_t max_length,
    std::string_view field_name) {
    const auto length = read_uint16_be(payload, offset);
    if (length > max_length) {
        throw std::length_error{std::string{"Coordinator "} + std::string{field_name} + " exceeds supported length"};
    }
    if (offset + length > payload.size()) {
        throw std::out_of_range{"Coordinator payload ended unexpectedly while reading string"};
    }

    std::string value;
    value.reserve(length);
    for (std::size_t index = 0; index < length; ++index) {
        value.push_back(static_cast<char>(std::to_integer<unsigned char>(payload[offset++])));
    }
    return value;
}

} // namespace

CoordinatorClient::CoordinatorClient() = default;

CoordinatorHandshakeFrame CoordinatorClient::build_registration_frame(const RegistrationConfig &config) const {
    CoordinatorHandshakeFrame frame{};

    frame.listen_port = clamp_port(config.listen_port);
    frame.heartbeat_seconds = static_cast<std::uint16_t>(std::clamp<std::int64_t>(
        config.heartbeat_interval.count(), 5, std::numeric_limits<std::uint16_t>::max()));
    frame.server_game_type = static_cast<std::uint8_t>(config.server_game_type);

    std::uint8_t nat_flags = 0;
    if (config.allow_direct) {
        nat_flags |= static_cast<std::uint8_t>(NatCapability::Direct);
    }
    if (config.allow_stun) {
        nat_flags |= static_cast<std::uint8_t>(NatCapability::Stun);
    }
    if (config.allow_turn) {
        nat_flags |= static_cast<std::uint8_t>(NatCapability::Turn);
    }
    frame.nat_capabilities = nat_flags;
    frame.public_listing = static_cast<std::uint8_t>(config.listed_publicly ? 1 : 0);

    frame.server_name = truncate_string(config.server_name, NETWORK_MAX_SERVER_NAME_LENGTH);
    frame.invite_code = truncate_string(config.invite_code, NETWORK_MAX_INVITE_CODE_LENGTH);

    frame.newgrfs.clear();
    frame.newgrfs.reserve(std::min(config.advertised_grfs.size(), NETWORK_MAX_GRF_COUNT));
    for (std::size_t i = 0; i < config.advertised_grfs.size() && i < NETWORK_MAX_GRF_COUNT; ++i) {
        frame.newgrfs.emplace_back(truncate_string(config.advertised_grfs[i], NETWORK_MAX_SERVER_NAME_LENGTH));
    }

    return frame;
}

std::vector<std::byte> CoordinatorHandshakeFrame::serialize() const {
    std::vector<std::byte> buffer;
    buffer.reserve(256);

    append_uint8(buffer, coordinator_version);
    append_uint8(buffer, game_info_version);
    append_uint8(buffer, admin_version);
    append_uint16_be(buffer, listen_port);
    append_uint16_be(buffer, heartbeat_seconds);
    append_uint8(buffer, server_game_type);
    append_uint8(buffer, nat_capabilities);
    append_uint8(buffer, public_listing);

    append_string(buffer, server_name);
    append_string(buffer, invite_code);

    append_uint8(buffer, static_cast<std::uint8_t>(std::min<std::size_t>(newgrfs.size(), NETWORK_MAX_GRF_COUNT)));
    for (const auto &grf_id : newgrfs) {
        append_string(buffer, grf_id);
    }

    if (buffer.size() > kMaxCoordinatorPayloadLength) {
        throw std::length_error{"Coordinator payload exceeds supported size"};
    }

    return buffer;
}

CoordinatorHandshakeFrame CoordinatorHandshakeFrame::deserialize(std::span<const std::byte> payload) {
    CoordinatorHandshakeFrame frame{};
    std::size_t offset = 0;

    frame.coordinator_version = read_uint8(payload, offset);
    frame.game_info_version = read_uint8(payload, offset);
    frame.admin_version = read_uint8(payload, offset);
    frame.listen_port = read_uint16_be(payload, offset);
    frame.heartbeat_seconds = read_uint16_be(payload, offset);
    frame.server_game_type = read_uint8(payload, offset);
    frame.nat_capabilities = read_uint8(payload, offset);
    frame.public_listing = read_uint8(payload, offset);

    frame.server_name = read_string(payload, offset, NETWORK_MAX_SERVER_NAME_LENGTH, "server name");
    frame.invite_code = read_string(payload, offset, NETWORK_MAX_INVITE_CODE_LENGTH, "invite code");

    const auto grf_count = read_uint8(payload, offset);
    if (grf_count > NETWORK_MAX_GRF_COUNT) {
        throw std::length_error{"Coordinator payload lists more GRFs than supported"};
    }

    frame.newgrfs.clear();
    frame.newgrfs.reserve(grf_count);
    for (std::uint8_t index = 0; index < grf_count; ++index) {
        frame.newgrfs.emplace_back(
            read_string(payload, offset, NETWORK_MAX_SERVER_NAME_LENGTH, "GRF identifier"));
    }

    if (offset != payload.size()) {
        throw std::invalid_argument{"Coordinator payload contains unexpected trailing data"};
    }

    return frame;
}

std::string describe_capabilities(std::uint8_t nat_capabilities) {
    std::string description;
    if (nat_capabilities & static_cast<std::uint8_t>(NatCapability::Direct)) {
        description += "direct";
    }
    if (nat_capabilities & static_cast<std::uint8_t>(NatCapability::Stun)) {
        if (!description.empty()) description += ", ";
        description += "STUN";
    }
    if (nat_capabilities & static_cast<std::uint8_t>(NatCapability::Turn)) {
        if (!description.empty()) description += ", ";
        description += "TURN";
    }
    if (description.empty()) {
        description = "none";
    }
    return description;
}

} // namespace sotc::network
