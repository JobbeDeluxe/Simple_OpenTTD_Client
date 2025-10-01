# Phase 3 Execution Hub

This hub condenses the disparate checklists and planning notes into a single
reference so the team can work through Phase 3 activities point by point. Use
it as the top-level tracker and jump to the linked documents for detailed
procedures.

## Foundations Already in Place

- Repository scaffold, documentation structure, and dependency management are in
  place as documented in the roadmap (Phase 0–2 complete).
- Cross-platform CMake build, vcpkg manifest, and CI jobs for Windows/Linux are
  operational. See `docs/ROADMAP.md` for the historical checklist.
- Initial networking port, GUI adjustments, and coordinator-aware configuration
  panels have been merged, enabling the Phase 3 QA focus.

## Current Focus Areas

| Area | Objective | Owner / Notes | Reference |
| ---- | --------- | ------------- | --------- |
| Automated regression coverage | Containerise OpenTTD 14.1 server, extend CTest harness with end-to-end connection flows, store logs as artefacts. | Pending | [`docs/PHASE3_NEXT_STEPS.md`](PHASE3_NEXT_STEPS.md#1-expand-automated-regression-coverage) |
| Configuration & CLI validation | Broaden negative tests for malformed configs, confirm precedence rules between env/config/CLI. | Pending | [`docs/PHASE3_NEXT_STEPS.md`](PHASE3_NEXT_STEPS.md#2-harden-configuration-and-cli-validation) |
| Manual gameplay sign-off | Execute desktop/headless playthroughs, capture findings in checklist. | Pending | [`docs/PHASE3_NEXT_STEPS.md`](PHASE3_NEXT_STEPS.md#3-manual-gameplay-sign-off), [`docs/MANUAL_TEST_CHECKLIST.md`](MANUAL_TEST_CHECKLIST.md) |
| Windows packaging readiness | Finalise packaging script, prototype installer, verify on clean VMs. | Pending | [`docs/PHASE3_NEXT_STEPS.md`](PHASE3_NEXT_STEPS.md#4-windows-packaging-readiness), [`tools/package_windows.ps1`](../tools/package_windows.ps1) |
| Release documentation | Update release notes, seed changelog, prepare download instructions. | Pending | [`docs/PHASE3_NEXT_STEPS.md`](PHASE3_NEXT_STEPS.md#5-release-documentation) |

> **Tip:** Update the *Owner / Notes* column during stand-ups to keep this table
> actionable.

## Detailed Checklist Snapshot

The following checklist distils the actionable items currently open. Mark each
checkbox once the work item has a merged PR and relevant documentation or tests
are updated.

### Automated Regression Coverage

- [ ] Publish OpenTTD 14.1 dedicated server container image for CI.
- [ ] Extend CTest harness to boot server, drive scripted connections, and save
      `server.log`/`client.log` artefacts.
- [ ] Add scenarios for:
  - [ ] company creation and graceful disconnects.
  - [ ] coordinator registration in `public`, `friends`, and `invite` modes.
  - [ ] rejection cases (invalid invite code, coordinator offline, auth
        failure).
- [ ] Gate `main` merges on the `integration` suite in GitHub Actions.

### Configuration & CLI Validation

- [ ] Broaden Python integration tests to cover malformed configuration files
      (unknown keys, duplicates).
- [ ] Verify precedence rules between environment variables, config files, and
      CLI arguments.
- [ ] Ensure failures emit actionable diagnostics for automation tooling.

### Manual Gameplay Sign-off

- [ ] Run desktop play session, log outcomes in
      [`docs/MANUAL_TEST_CHECKLIST.md`](MANUAL_TEST_CHECKLIST.md).
- [ ] Repeat in headless mode, capturing coordinator metadata verification.
- [ ] Sanity-check localisation strings (English, German) for coordinator
      panels and tooltips.

### Windows Packaging Readiness

- [ ] Complete `tools/package_windows.ps1` bundling of runtime DLLs and release
      notes into ZIP artefact.
- [ ] Prototype NSIS installer steps and document any manual steps remaining.
- [ ] Test packages on clean Windows 10/11 VMs, noting prerequisites in the
      README.

### Release Documentation

- [ ] Update `docs/RELEASE_NOTES_0.1.0.md` with latest testing outcomes and
      known issues.
- [ ] Draft initial `docs/CHANGELOG.md` using Keep a Changelog sections and link
      it from the README when ready.
- [ ] Prepare download instructions (hashes, mirrors) for publishing builds.

## Quick Links

- [Roadmap](ROADMAP.md)
- [Quality Assurance Plan](QUALITY_ASSURANCE_PLAN.md)
- [Phase 3 Next Steps](PHASE3_NEXT_STEPS.md)
- [Manual Test Checklist](MANUAL_TEST_CHECKLIST.md)
- [Release Notes 0.1.0](RELEASE_NOTES_0.1.0.md)

Keep this hub updated after every milestone review to maintain a single source
of truth for the team's execution status.
