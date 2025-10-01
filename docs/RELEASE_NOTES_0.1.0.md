# Simple OpenTTD Client 0.1.0 Release Notes

> **Status:** Draft – Phase 3 preparation

These notes capture the highlights, fixed issues, and known limitations for the
first public preview of the Simple OpenTTD Client targeting OpenTTD 14.1
servers. The document will graduate from draft status once the Phase 3 exit
criteria described in `docs/QUALITY_ASSURANCE_PLAN.md` are satisfied.

## Highlights

- Modernised coordinator-aware launcher derived from the classic `cmclient`.
- OpenTTD 14.1 protocol compatibility covering coordinator, admin, and
  multiplayer game info exchanges.
- Updated CLI workflows with `--dump-launch-options` and
  `--dump-registration` summaries that align with the coordinator schemas.

## Testing Progress

- CLI-focused integration tests continue to validate configuration precedence
  and coordinator payload generation while the dedicated server harness is being
  containerised for CI.
- Company creation, coordinator registration, and disconnect scenarios have
  been drafted for the upcoming regression suite once the OpenTTD 14.1 image is
  available in the internal registry.
- Manual gameplay sign-off is scheduled: a desktop session is booked for the
  week of 2024-06-10, followed by a headless verification pass in the week of
  2024-06-17. Findings will be recorded in
  `docs/MANUAL_TEST_CHECKLIST.md`.

## Fixed Issues

- Normalise configuration parsing between file-based and command-line inputs.
- Harden invite-only registration payload generation with stricter validation
  and diagnostics.

## Known Issues

- Automated gameplay scenarios that exercise a live OpenTTD server are still in
  progress. Until the dedicated server container and scripted harness ship,
  only the CLI-focused integration tests are available in CI.
- Windows packaging is being stabilised; binaries built from source require the
  Visual C++ redistributables that ship with Visual Studio 2022. The NSIS
  installer prototype remains manual.
- Coordinator invite rejection paths lack automated coverage until the
  integration suite can replay invalid code and offline-coordinator scenarios.

## Upgrade Notes

- The client bundles a refreshed default configuration (`sotc.cfg`) that
  mirrors the 14.1 coordinator defaults. Review your existing overrides before
  upgrading.
- Saved coordinator credentials from earlier previews are invalid. Re-run the
  registration flow or update secrets via the CLI flags.

## Contributors

- Simple OpenTTD Client maintainers

## Changelog Snapshot

- Port networking stack and registration payloads to OpenTTD 14.1.
- Add CLI verbs for launch and registration dumps.
- Expand integration tests covering invalid configuration inputs.

