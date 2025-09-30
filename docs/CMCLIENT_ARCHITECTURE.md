# `cmclient` Architecture Inventory

This document records the current understanding of the upstream `cmclient` code base. It summarises the major modules, build tooling, and third-party dependencies so that follow-up migration work can reference a single source of truth.

## High-level Overview

`cmclient` is a fork of the OpenTTD client with CityMania-specific additions. The repository is organised as a large CMake project that builds the primary `openttd` binary alongside a collection of asset generators and developer tools. The source tree is split into domain-specific modules (gameplay, networking, rendering, platform integration, etc.) with companion data generators that produce tables and assets at build time.

Key top-level directories include:

- `src/` – core gameplay, rendering, networking, scripting, and tool sources organised into submodules.
- `projects/` – generated Visual Studio projects for legacy MSVC workflows.
- `cmake/` – shared CMake modules, options, and scripts used during configuration.
- `os/` – platform-specific launchers, packaging metadata, and Objective-C++ wrappers for macOS.
- `grf/`, `media/`, and `release_files/` – data assets, NewGRF resources, and packaging artefacts consumed by the build.
- `docs/` – upstream technical documentation covering debugging, networking, savegame formats, and tooling guides.

## Module Inventory (`src/`)

The `src` tree is structured by domain. The most relevant subdirectories are listed below with a short description of their responsibilities:

| Directory | Purpose |
| --- | --- |
| `3rdparty/` | Bundled third-party code (e.g., miniz, sha1) that ships with the client. |
| `ai/` | Integration shims between the core engine and AI/game scripts, including scripting runtimes. |
| `blitter/` | Rendering backends that implement different blitting strategies for the 2D graphics pipeline. |
| `citymania/` | CityMania-specific extensions layered on top of the upstream OpenTTD functionality. |
| `core/` | Foundational utilities (logging, threading, memory, file I/O abstractions) shared across the project. |
| `game/` | Core gameplay logic (companies, economy, map mechanics, vehicles, towns, industries). |
| `linkgraph/` | Graph structures used for routing analysis and service planning. |
| `misc/` | Cross-cutting helpers (error handling, profiler hooks, assertions) that do not fit other domains. |
| `music/` & `sound/` | Audio playback backends and music driver integrations. |
| `network/` | Multiplayer networking, protocol handlers, admin console, and server discovery logic. |
| `newgrf/` | NewGRF parser, sprite management, and runtime patching of graphics data. |
| `os/` | OS abstraction layers for filesystem paths, clipboard, dialogs, and integration glue. |
| `pathfinder/` | Routing algorithms for road, rail, and ship pathfinding along with heuristics. |
| `saveload/` | Savegame serialisation, versioning, and migration helpers. |
| `script/` | GameScript virtual machine and bindings that expose game state to scripts. |
| `settingsgen/`, `strgen/`, `spriteloader/` | Build-time generators for settings tables, string tables, and sprite metadata. |
| `table/` | Generated lookup tables (cargo, terrain, GUI elements) referenced by runtime modules. |
| `timer/` | Scheduling and timing utilities, including the main game tick dispatcher. |
| `video/`, `widgets/`, `fontcache/` | GUI rendering, widget definitions, font rasterisation, and UI event handling. |

Supporting directories such as `lang/`, `music/`, `sound/`, and `widgets/` contain data files and code required for localisation and user interface rendering. The `tests/` tree hosts unit and integration tests that exercise selected subsystems.

### Tooling & Generators

Several helper binaries are built from within `src/`:

- `settingsgen` reads the declarative settings definitions and emits generated C++ code consumed by the main target.
- `strgen` compiles translation source files into binary tables.
- `spriteloader` and related utilities convert GRF assets into runtime sprite packs.

These generators run as part of the configure/build steps and place their output under `generated/` in the build tree.

## Build System

`cmclient` standardises on CMake (minimum 3.17) to orchestrate builds across Windows, macOS, Linux, and Emscripten targets. Key traits of the build system include:

- The primary target is the `openttd` executable with optional dedicated-server and tools-only variants controlled via cache options.
- Extensive use of modular CMake scripts (`cmake/Options.cmake`, `cmake/CompileFlags.cmake`, `cmake/scripts/FindVersion.cmake`, etc.) to configure platform flags, IPO/LTO, and generated sources.
- Custom targets for documentation (`docs`, `docs_source`), asset generators, and version stamping executed through `find_version`.
- Legacy MSVC project files under `projects/` for Visual Studio 2015–2019 that mirror the CMake target graph.
- CI configurations via Azure Pipelines (`azure-pipelines/` and `azure-pipelines.yml`) driving multi-platform builds, packaging, and release automation.

## Dependency Inventory

`cmclient` manages external libraries through both vcpkg (`vcpkg.json`) and traditional package discovery. The notable runtime and build-time dependencies are:

- Compression & codecs: `zlib`, `liblzma`, `lzo`, `zstd`, `libpng`.
- Audio: `opusfile`, `fluidsynth` (Linux/macOS), XAudio2 (Windows), Allegro (non-Windows fallback).
- Rendering & text: `SDL2`, `OpenGL`, `Freetype`, `Fontconfig`, `Harfbuzz`, `ICU` (for i18n), platform-specific Cocoa/Quartz components on macOS.
- Networking & crash reporting: `libcurl` (non-Windows), `breakpad`.
- Toolchain helpers: `Threads`, `SSE` feature detection, `Grfcodec` for NewGRF handling.

Dependencies are conditionally resolved based on platform (e.g., HTTP handled via WinHTTP on Windows, libcurl elsewhere; some Linux-only packages such as `dbus` and `fontconfig` are declared in `vcpkg.json`).

## Build Outputs & Packaging

The standard build produces:

- `openttd` game client binary.
- Optional dedicated server binary when `OPTION_DEDICATED` is enabled.
- Auxiliary tooling binaries for asset generation and debugging.
- Packaging assets assembled via CPack (`CPackProperties.cmake.in`) and platform-specific scripts under `os/` and `release_files/`.

Understanding these components provides the foundation for planning our OpenTTD 14.1 port and identifying the areas that require updates or replacements in this repository.
