# Simple OpenTTD Client

This repository aims to provide a modernized fork of the `cmclient` project that is compatible with OpenTTD 14.1. The initial goal is to scaffold the project structure, document the required work, and prepare a build system that targets Windows (MSVC) and Linux.

## Project Goals
- Maintain feature parity with the original `cmclient` where possible.
- Ensure compatibility with OpenTTD 14.1 network and gameplay features.
- Provide straightforward build and packaging instructions for Windows.

## Repository Layout
- `cmake/` – CMake toolchain helpers and platform specific configuration.
- `external/` – Third-party dependencies (as Git submodules or source drops).
- `src/` – Client implementation sources.
- `include/` – Public headers.
- `docs/` – Design documents and migration notes.
- `tools/` – Developer utilities and scripts.

## Current Status
The project is currently a scaffold. See `docs/ROADMAP.md` for the implementation plan, `docs/REFERENCES.md` for collected research material, and `docs/OPEN_TTD_PROTOCOL_DELTAS.md` for a breakdown of the networking differences between OpenTTD 1.10.x and 14.1.

## Building
The project uses CMake 3.21 or newer. Out-of-tree builds are recommended to keep generated artefacts separate from the source tree.

### Linux (GCC/Clang)
```bash
cmake -S . -B build/linux -G Ninja
cmake --build build/linux
```

The configuration enables POSIX threads automatically. Enable IPO/LTO with `-DSOTC_ENABLE_IPO=ON` when supported by your toolchain.

### Windows (MSVC)
```powershell
cmake -S . -B build\msvc -G "Visual Studio 17 2022" -A x64
cmake --build build\msvc --config Release
```

The generated Visual Studio solution configures common warnings and disables legacy CRT deprecation noise. You can enable IPO with `/p:SOTC_ENABLE_IPO=ON` or set the option from the CMake GUI.

## Contributing
1. Fork and clone the repository.
2. Configure dependencies (see `docs/DEPENDENCIES.md`).
3. Implement features or fixes.
4. Submit a pull request with a detailed description and testing evidence.

## License
This project is licensed under the MIT License. See `LICENSE` for details.
