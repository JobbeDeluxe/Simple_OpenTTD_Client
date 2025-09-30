# OpenTTD 14.1 Protocol & API Deltas

This note summarises the key wire-protocol and multiplayer API differences between the OpenTTD 1.10.x baseline targeted by `cmclient` and upstream OpenTTD 14.1. It focuses on areas that affect network compatibility, lobby discovery, and admin/bot integrations so that follow-up porting work can be scoped accurately.

> **Baseline** – `cmclient` tracks OpenTTD 1.10.x era networking code. Unless stated otherwise, "1.10" below refers to OpenTTD 1.10.3 and "14.1" refers to OpenTTD 14.1.

## 1. Discovery & Coordinator Services

| Area | 1.10 behaviour | 14.1 behaviour | Porting implications |
| --- | --- | --- | --- |
| Master/coordinator backend | Single UDP master server (`master.openttd.org`, port 3978) and HTTP content mirror discovery (`binaries.openttd.org`). | Dedicated Game Coordinator (`NETWORK_COORDINATOR_SERVER_PORT` 3976) plus STUN/TURN services for NAT traversal, with coordinator/content endpoints supplied through helper accessors (e.g., `NetworkCoordinatorConnectionString()`). | Replace legacy master server handshake with coordinator registration (`PACKET_COORDINATOR_SERVER_REGISTER`) and periodic updates. Implement invite-code storage and NAT capability reporting within the client. |
| Server listing | Clients query master server via UDP (`PACKET_UDP_MASTER_SERVER_*`). | Clients request listings via TCP coordinator (`PACKET_COORDINATOR_CLIENT_LISTING`) and receive serialized `NetworkGameInfo` payloads with optional NewGRF lookup cursors. | Rework lobby code to perform coordinator-driven listing and handle multi-packet responses including NewGRF lookup tables. |
| Direct connects & invites | Direct hostname:port connections only. | Coordinator supports invite codes and TURN/STUN assisted NAT punching (`PACKET_COORDINATOR_CLIENT_CONNECT`, `PACKET_COORDINATOR_GC_TURN_CONNECT`). | Add invite-code entry UI/CLI, maintain coordinator token state machine, and surface connection diagnostics to the user. |

## 2. Core Protocol Versions & Limits

| Token | 1.10 value | 14.1 value | Notes |
| --- | --- | --- | --- |
| `NETWORK_GAME_ADMIN_VERSION` | 1 | 3 | Admin interface adds extended chat actions and error detail strings. |
| `NETWORK_GAME_INFO_VERSION` | 4 | 7 | `NetworkGameInfo` now carries additional metadata (NewGRF lookup cursor, content survey flags, invite support). |
| `NETWORK_MASTER_SERVER_VERSION` / `NETWORK_COORDINATOR_VERSION` | 2 (master server) | 6 (coordinator) | Coordinator protocol introduces richer registration payloads and NAT negotiation. |
| `NETWORK_GAMESCRIPT_JSON_LENGTH` | `SEND_MTU - 3` (≈1457 bytes) | 9000 bytes | Larger JSON payloads for GameScript admin calls; requires resizing buffers and validation. |
| `NETWORK_MAX_GRF_COUNT` | 62 | 255 | Game info packets can now advertise substantially more NewGRFs, impacting compatibility checks and UI pagination. |
| Packet MTU | Fixed 1460 byte limit for TCP/UDP payloads. | 1460 byte UDP compatibility MTU plus expanded 32 KiB TCP MTU with backward-compatible framing. | Update send/receive code paths to accept large TCP payloads and ensure compatibility framing for legacy peers. |

## 3. Runtime Data Structures & Enums

| Structure/Enum | 1.10 | 14.1 | Migration work |
| --- | --- | --- | --- |
| `NetworkCompanyState::password` | Fixed-size char array (`NETWORK_PASSWORD_LENGTH`). | `std::string` with dynamic length, aligning with invite-code secret handling. | Update serialization to read/write string lengths, remove assumptions about null-terminated buffers. |
| `NetworkAction` | Values through `NETWORK_ACTION_KICKED`. | Adds `NETWORK_ACTION_EXTERNAL_CHAT` for cross-service chat relays. | Extend admin/chat handlers to accept and display external chat events. |
| `NetworkErrorCode` | No `NETWORK_ERROR_INVALID_CLIENT_NAME`. | Adds new error to report display-name validation failures. | Surface descriptive error for invalid names and synchronise validation rules with server expectations. |
| Server presentation | No explicit server type enumeration. | `ServerGameType` enum communicates public/local/invite-only state via coordinator packets and GUI. | Persist server game type selection in client settings UI and include it in coordinator registration payloads. |

## 4. New Subsystems

- **Game Coordinator TCP stack** – 14.1 adds `network_coordinator.cpp/h` and `core/tcp_coordinator.h`, encapsulating coordinator registration, listing, STUN/TURN workflow, and invite token management. `cmclient` lacks these files entirely, so the port must introduce the coordinator socket layer and integrate it with the existing network event loop.
- **Survey telemetry** – Coordinator configuration exposes `NetworkSurveyUriString()` and `NETWORK_SURVEY_VERSION`, enabling optional server surveys. Client needs toggles and consent handling when adopting upstream defaults.
- **Content metadata expansion** – Content mirror constants are extended (name/version/URL/tag length caps). Download UI and caching logic must handle the new bounds and ensure strings stay within the revised limits.

## 5. Compatibility Considerations

1. **Handshake gating** – Servers running 14.1 reject clients advertising `NETWORK_GAME_INFO_VERSION` < 7. The port must advertise 14.1 versions once all structures are updated; until then, a compatibility shim may be required for incremental bring-up.
2. **Admin console bots** – With admin protocol v3, bots can receive external chat and richer error strings. Existing CityMania bots must be adjusted to parse the extended packet formats before toggling the version number.
3. **UI & settings persistence** – New fields (invite code, server game type, NAT status) must be surfaced in configuration dialogs and persisted in the settings backend to align with upstream defaults.
4. **Testing strategy** – Regression tests should include coordinator-assisted joins, large GameScript JSON payloads, and GRF lists > 62 entries to ensure buffer sizing regressions are caught.

Capturing these deltas completes Phase 0, Punkt 3 of the roadmap and provides the groundwork for planning the networking port to OpenTTD 14.1.
