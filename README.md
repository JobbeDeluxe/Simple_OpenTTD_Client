# Simple OpenTTD Client

This repository aims to provide a modernized fork of the `cmclient` project that is compatible with OpenTTD 14.1. The initial goal is to scaffold the project structure, document the required work, and prepare a build system that targets Windows (MSVC) and Linux.

## Project Goals
- Maintain feature parity with the original `cmclient` where possible.
- Ensure compatibility with OpenTTD 14.1 network and gameplay features.
- Provide straightforward build and packaging instructions for Windows.

## Repository Layout
- `cmake/` – CMake toolchain helpers and platform specific configuration.
- `vcpkg.json` – Manifest describing third-party dependencies managed by vcpkg.
- `src/` – Client implementation sources.
- `include/` – Public headers.
- `docs/` – Design documents and migration notes.
- `tools/` – Developer utilities and scripts.

## Current Status
The project is currently a scaffold. See `docs/ROADMAP.md` for the implementation plan, `docs/REFERENCES.md` for collected research material, and `docs/OPEN_TTD_PROTOCOL_DELTAS.md` for a breakdown of the networking differences between OpenTTD 1.10.x and 14.1.

## Building
The project uses CMake 3.21 or newer. Out-of-tree builds are recommended to keep generated artefacts separate from the source tree.

### Dependency bootstrap
Dependencies are managed through the [`vcpkg`](https://github.com/microsoft/vcpkg) manifest in this repository. Install vcpkg and
set the `VCPKG_ROOT` environment variable, then let CMake pick up the manifest automatically:

```bash
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh # or bootstrap-vcpkg.bat on Windows
export VCPKG_ROOT="$(pwd)/vcpkg"
```

You can pre-fetch all libraries with `vcpkg install` or allow CMake to install them on demand during the first configure.

### Linux (GCC/Clang)
```bash
cmake -S . -B build/linux -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
cmake --build build/linux
```

The configuration enables POSIX threads automatically. Enable IPO/LTO with `-DSOTC_ENABLE_IPO=ON` when supported by your toolchain.

### Windows (MSVC)
```powershell
$env:VCPKG_ROOT = "C:\path\to\vcpkg"
cmake -S . -B build\msvc -G "Visual Studio 17 2022" -A x64 \
  -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake"
cmake --build build\msvc --config Release
```

The generated Visual Studio solution configures common warnings and disables legacy CRT deprecation noise. You can enable IPO with `/p:SOTC_ENABLE_IPO=ON` or set the option from the CMake GUI. TLS
support is enabled by default; disable it with `-DSOTC_USE_OPENSSL=OFF` if you do not need secure transports or prefer a lighter
dependency set.

## Contributing
1. Fork and clone the repository.
2. Configure dependencies (see `docs/DEPENDENCIES.md`).
3. Implement features or fixes.
4. Submit a pull request with a detailed description and testing evidence.

## License
This project is licensed under the MIT License. See `LICENSE` for details.
