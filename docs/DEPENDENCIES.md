# Dependencies (Draft)

The final client will depend on the following third-party libraries. Versions will be confirmed during the implementation phase.

| Library | Purpose | Notes |
| --- | --- | --- |
| SDL2 | Rendering / input | Required for cross-platform windowing and input handling. |
| SDL2_image | Image loading | For sprite assets used in the client UI. |
| SDL2_ttf | Font rendering | For UI text. |
| libcurl | HTTP(S) communication | For remote configuration and update checks. |
| zlib | Compression | Used by the OpenTTD network protocol. |
| OpenSSL | TLS support | Optional, required for secure connections if supported. |
| spdlog | Logging | Structured logging output. |

## Dependency Management Options
1. **vcpkg:** Integrate via the `CMAKE_TOOLCHAIN_FILE` for Windows-centric workflows.
2. **FetchContent:** Use CMake's built-in module to download and build dependencies during configure time.
3. **Git Submodules:** Vendor exact versions for deterministic builds.

## Decision
We will adopt a **vcpkg manifest-based workflow** as the primary dependency manager. The
project will ship a `vcpkg.json` manifest that pins library versions and enables
deterministic binary caching in CI. Platform-specific triplets will cover MSVC and
GCC/Clang targets so that developers can bootstrap a working toolchain with a single
`vcpkg install` step.

For libraries that are not available in the public vcpkg registry, we will maintain
overlay ports inside the repository. Small, header-only utilities may still be
integrated with CMake `FetchContent` when doing so avoids unnecessary rebuilds or
packaging complexity, but vcpkg remains the source of truth for version locking.
