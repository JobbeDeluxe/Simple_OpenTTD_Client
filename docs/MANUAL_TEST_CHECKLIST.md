# Manual Gameplay Checklist

This checklist tracks the manual verification steps required to close out
Phase 3 of the roadmap. Testers should record their initials and the execution
date once a task is complete. Add notes or issue links for any failures.

| Area | Scenario | Owner | Date | Status | Notes |
|------|----------|-------|------|--------|-------|
| Launch | Start the client in desktop mode, connect to a public server, create a company. | | | ☐ | |
| UI | Review settings, coordinator, company, and message console panels for regressions. | | | ☐ | |
| Gameplay | Fast-forward through 15 in-game years validating vehicles, servicing, and economy graphs. | | | ☐ | |
| Tools | Exercise terraform, industry placement, and map overlays for responsiveness. | | | ☐ | |
| Headless | Run the client in headless mode and verify coordinator metadata from a remote session. | | | ☐ | |
| Localisation | Confirm English and German strings render correctly in menus, tooltips, and status updates. | | | ☐ | |
| Regression | Log defects with reproduction steps, screenshots, and savegames as needed. | | | ☐ | |

## Execution Guidance

- When testing against public servers, coordinate with the community moderators
  to avoid disrupting live games.
- Capture screenshots or video clips for any UI anomalies to aid triage.
- For headless validation, use the dedicated server heartbeat dashboard to
  confirm the advertised metadata matches the configuration.
- Open an issue for any failing check and reference the corresponding row in the
  table when filing the report.

