# Quality Assurance Plan

This document expands the Phase 3 roadmap items with actionable guidance so the
team can stabilise the Simple OpenTTD Client and prepare a public release.

## Automated Regression Tests

The goal is to exercise a real OpenTTD 14.1 server using scripted scenarios.

1. **Test harness**
   - Use the existing Docker images from the OpenTTD project or package the
     upstream dedicated server binaries inside a container image pinned to
     14.1.
   - Provide a CTest target (`ctest --label-regex integration`) that starts the
     server in the background and executes the client with scripted inputs.
   - Capture logs (`server.log`, `client.log`) and expose them as CTest
     attachments for CI artefacts.
2. **Coverage**
   - Connection lifecycle (handshake, company join, graceful disconnect).
   - Coordinator registration flows for `public`, `friends`, and `invite`
     sessions, verifying payload schemas.
   - Regression scenarios for configuration parsing (`--config` file, command
     line overrides) to guarantee determinism.
   - Negative cases: invalid invite code, unreachable coordinator, rejected
     authentication.
3. **CI Integration**
   - Extend the existing GitHub Actions workflow to install the OpenTTD server
     artefacts, run the labelled integration suite, and store the logs.
   - Gate merges into the release branch on the automated suite.

### Current Status

The repository includes an initial integration harness at
`tests/integration/test_launch_options_dump.py` wired into CTest under the
`integration` label. It launches the client with a representative configuration
file, applies CLI overrides, and asserts that both `--dump-launch-options` and
`--dump-registration` outputs match the expected OpenTTD 14.1 coordinator
schema. This provides early regression coverage for configuration parsing and
payload generation while the full server-backed suite is being implemented.

A companion script, `tests/integration/test_invalid_inputs.py`, exercises
malformed heartbeat intervals and configuration flags to confirm the
client exits with a failure status while emitting clear diagnostics.
These checks seed the negative coverage required for Phase 3 while the
server-backed scenarios are developed.

## Manual Gameplay Checklist

Purpose: verify end-to-end playability, UI expectations, and behavioural
regressions before cutting a release.

- Launch the client in desktop mode, connect to a public server, and create a
  company.
- Validate UI panels: settings, coordinator options, company list, and message
  console.
- Play for 15 in-game years (fast-forward accepted) ensuring:
  - vehicle purchasing, orders, servicing work without crashes;
  - economy graphs render as expected;
  - map tools (terraform, industries) respond promptly.
- Repeat the session in headless mode and confirm the dedicated server exposes
  correct metadata to the coordinator.
- Sanity-check localisation strings in English and German (menus, tooltips,
  status updates).
- Record defects, screenshots, and reproduction steps in the issue tracker.

## Windows Packaging

- Produce two artefacts from the `Release` configuration:
  - `SimpleOpenTTDClient-<version>-win64.zip` containing the binaries and
    documentation.
  - `SimpleOpenTTDClient-<version>-Setup.exe` built via `WiX` or `NSIS`, with
    code-signing optional for internal builds.
- Add a `tools/package_windows.ps1` script that automates the packaging steps
  and collects dependent DLLs via `vcpkg` manifest inspection.
- Verify the installer on a clean Windows VM (no Visual Studio runtime) and
  document any prerequisite redistributables.

## Release Notes & Documentation

- Draft `docs/RELEASE_NOTES_<version>.md` summarising new features, known
  issues, and upgrade considerations.
- Update the main `README.md` with download links once binaries are hosted.
- Prepare a `CHANGELOG.md` entry following Keep a Changelog format.
- Announce availability on the project website or forum with clear upgrade
  instructions and checksums.

## Exit Criteria for Phase 3

- All automated tests green in CI for at least two consecutive runs.
- Manual checklist executed and signed off by two team members.
- Windows packages uploaded and verified by testers.
- Release notes published together with the binary artefacts.
