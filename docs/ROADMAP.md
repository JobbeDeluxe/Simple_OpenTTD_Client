# Roadmap

This document tracks the work required to deliver a modernized OpenTTD client derived from `cmclient`.

## Phase 0 – Research & Planning (Current)
- [x] Establish repository scaffold and documentation structure.
- [x] Inventory `cmclient` architecture (modules, dependencies, build system).
- [x] Map OpenTTD 14.1 protocol and API deltas compared to the version targeted by `cmclient`.
- [x] Decide on dependency management (vcpkg manifest with overlay ports, FetchContent for small header-only libs).

## Phase 1 – Build System & Tooling
- [x] Create CMake-based build configuration for Windows (MSVC) and Linux.
- [x] Integrate vcpkg or FetchContent for dependencies (SDL2, libcurl, zlib, etc.).
- [x] Set up continuous integration for Windows and Linux builds.
- [ ] Provide developer setup scripts and documentation.

## Phase 2 – Core Client Port
- [ ] Port networking layer to OpenTTD 14.1 protocol changes.
- [ ] Update serialization/deserialization logic.
- [ ] Implement GUI adjustments for new features.
- [ ] Ensure compatibility with dedicated server management tools.

## Phase 3 – Quality Assurance & Release
- [ ] Automated regression tests against OpenTTD 14.1 server.
- [ ] Manual gameplay testing checklist.
- [ ] Package Windows binaries (installer/ZIP).
- [ ] Draft release notes and user documentation.

Progress will be tracked via GitHub issues and milestone boards once the repository is published.
