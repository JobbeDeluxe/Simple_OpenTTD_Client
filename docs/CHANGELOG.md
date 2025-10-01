# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- Tracking of automated integration progress for the dedicated server harness and
  manual playthrough scheduling as Phase 3 stabilises.
- Draft packaging tasks for Windows ZIP and installer artefacts pending
  completion of automation.

### Known Issues
- OpenTTD 14.1 dedicated server container image is not yet published to the
  internal registry; CI integration will follow once image validation is
  complete.
- Manual gameplay sign-off is scheduled but outstanding until the desktop and
  headless sessions are executed.

## [0.1.0] - TBD
### Added
- Modernised coordinator-aware launcher derived from the classic `cmclient`.
- OpenTTD 14.1 protocol compatibility covering coordinator, admin, and
  multiplayer game info exchanges.
- Updated CLI workflows with `--dump-launch-options` and
  `--dump-registration` summaries that align with the coordinator schemas.

### Fixed
- Normalised configuration parsing between file-based and command-line inputs.
- Hardened invite-only registration payload generation with stricter validation
  and diagnostics.

### Testing
- CLI-focused integration tests covering configuration precedence and
  coordinator registration payload validation.
