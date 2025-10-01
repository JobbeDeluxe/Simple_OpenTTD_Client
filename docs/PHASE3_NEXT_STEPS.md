# Phase 3 Next Steps

This checklist distils the outstanding Phase 3 deliverables into actionable
work items. Use it alongside `docs/QUALITY_ASSURANCE_PLAN.md` for detailed test
coverage guidance and `docs/ROADMAP.md` for milestone context.

## 1. Expand Automated Regression Coverage
- Containerise an OpenTTD 14.1 dedicated server image and publish it to the
  internal registry for CI consumption.
- Extend the CTest integration harness to boot the server, drive scripted
  connection flows, and persist `server.log` / `client.log` as artefacts.
- Add scenarios for:
  - company creation and graceful disconnects;
  - coordinator registrations covering `public`, `friends`, and `invite` modes;
  - rejection cases (invalid invite code, coordinator offline, auth failure).
- Wire the new suite into GitHub Actions, gating `main` merges on the
  `integration` label.

## 2. Harden Configuration and CLI Validation
- Broaden the existing Python-based integration tests to include malformed
  configuration files (unknown keys, duplicate entries).
- Verify precedence rules when combining environment variables, config files,
  and CLI arguments once that feature lands.
- Ensure failing inputs produce actionable error messages for automation tools.

## 3. Manual Gameplay Sign-off
- Schedule two playthrough sessions covering desktop and headless modes and log
  findings in `docs/MANUAL_TEST_CHECKLIST.md`.
- Capture screenshots or recordings of UI regressions and raise GitHub issues
  for follow-up fixes.
- Sanity-check localisation strings (English, German) for coordinator-centric
  panels and tooltips.

## 4. Windows Packaging Readiness
- Finalise `tools/package_windows.ps1` to bundle runtime DLLs and release notes
  into the ZIP artefact automatically.
- Prototype the NSIS installer section of the script and document any manual
  steps required while automation is incomplete.
- Test the resulting packages on a clean Windows 10/11 VM and note
  redistributable prerequisites in the README.

## 5. Release Documentation
- Update `docs/RELEASE_NOTES_0.1.0.md` with the testing progress and known
  issues discovered during the above activities.
- Draft a `CHANGELOG.md` skeleton adopting the Keep a Changelog format and link
  it from the README once initial content is ready.
- Prepare publish-ready download instructions (hashes, mirrors) for when builds
  are uploaded.

Revisit this list after each milestone review to capture new findings or adjust
priorities based on test outcomes.
