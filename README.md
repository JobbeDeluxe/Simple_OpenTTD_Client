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
The project is currently a scaffold. See `docs/ROADMAP.md` for the implementation plan and `docs/REFERENCES.md` for collected research material on the legacy client and OpenTTD 14.1 APIs.

## Building (WIP)
```bash
cmake -S . -B build
cmake --build build
```

Windows builds are not yet configured; once CI is in place, instructions for MSVC will be added.

## Contributing
1. Fork and clone the repository.
2. Configure dependencies (see `docs/DEPENDENCIES.md`).
3. Implement features or fixes.
4. Submit a pull request with a detailed description and testing evidence.

## License
This project is licensed under the MIT License. See `LICENSE` for details.
