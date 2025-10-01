#include "gui/session_formatting.hpp"

#include <algorithm>

namespace sotc::ui {

std::string to_string(network::ServerGameType type) {
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

std::string build_server_name(std::string_view player_name, std::string_view fallback) {
    if (player_name.empty()) {
        return std::string{fallback};
    }
    std::string name{player_name};
    name += "'s game";
    return name;
}

std::string format_endpoint(const std::string &host, std::uint16_t port) {
    std::string endpoint = host.empty() ? std::string{"<not set>"} : host;
    endpoint += ':';
    endpoint += std::to_string(port);
    return endpoint;
}

std::string describe_nat_policy(bool allow_direct, bool allow_stun, bool allow_turn) {
    std::string description;
    if (allow_direct) {
        description += "Direct";
    }
    if (allow_stun) {
        if (!description.empty()) description += ", ";
        description += "STUN";
    }
    if (allow_turn) {
        if (!description.empty()) description += ", ";
        description += "TURN";
    }
    if (description.empty()) {
        description = "None";
    }
    return description;
}

} // namespace sotc::ui

