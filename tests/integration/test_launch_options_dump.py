#!/usr/bin/env python3
"""Integration test for the Simple OpenTTD Client launch summaries.

This harness exercises the command-line interface by loading a configuration
file, overriding select values via CLI flags, and asserting that both
``--dump-launch-options`` and ``--dump-registration`` reflect the expected
resolved state. The test intentionally avoids network interaction so it can run
in CI environments without an OpenTTD server while still guarding against
regressions in configuration parsing and coordinator payload generation.
"""

from __future__ import annotations

import argparse
import pathlib
import subprocess
import sys
import tempfile
import textwrap
from typing import Dict


def parse_key_value_payload(output: str) -> Dict[str, str]:
    """Convert a key=value payload into a dictionary."""

    result: Dict[str, str] = {}
    for line in output.splitlines():
        if not line.strip():
            continue
        if "=" not in line:
            raise AssertionError(f"Unexpected payload line without '=': {line!r}")
        key, value = line.split("=", 1)
        result[key.strip()] = value.strip()
    return result


def run_client(binary: pathlib.Path, *args: str) -> str:
    """Execute the client binary and return its stdout."""

    command = [str(binary), *args]
    completed = subprocess.run(
        command,
        check=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    if completed.stderr:
        # Surface stderr to aid debugging if the assertions fail later.
        sys.stderr.write(completed.stderr)
    return completed.stdout


def assert_launch_summary(summary: Dict[str, str]) -> None:
    expected = {
        "server_host": "override.example",
        "server_port": "4500",
        "player_name": "CLI Bot",
        "headless": "true",
        "coordinator_host": "coordinator.example.net",
        "coordinator_port": "5000",
        "server_game_type": "invite",
        "invite_code": "CODE123",
        "listed_publicly": "false",
        "allow_direct": "false",
        "allow_stun": "true",
        "allow_turn": "false",
        "heartbeat_interval": "45",
        "advertised_grfs": "11112222,33334444,55556666",
    }

    missing = expected.keys() - summary.keys()
    if missing:
        raise AssertionError(f"Launch summary missing keys: {sorted(missing)!r}\nPayload: {summary!r}")

    for key, value in expected.items():
        actual = summary.get(key)
        if actual != value:
            raise AssertionError(
                f"Launch summary mismatch for '{key}': expected {value!r}, got {actual!r}\nPayload: {summary!r}"
            )


def assert_registration_summary(summary: Dict[str, str]) -> None:
    expected = {
        "listen_port": "4500",
        "heartbeat_seconds": "45",
        "server_game_type": "2",  # InviteOnly
        "nat_capabilities": "2",  # STUN only
        "public_listing": "false",
        "server_name": "CLI Bot's game",
        "invite_code": "CODE123",
        "newgrfs": "11112222,33334444,55556666",
    }

    missing = expected.keys() - summary.keys()
    if missing:
        raise AssertionError(f"Registration summary missing keys: {sorted(missing)!r}\nPayload: {summary!r}")

    for key, value in expected.items():
        actual = summary.get(key)
        if actual != value:
            raise AssertionError(
                f"Registration summary mismatch for '{key}': expected {value!r}, got {actual!r}\nPayload: {summary!r}"
            )

    payload_hex = summary.get("payload_hex", "")
    if not payload_hex:
        raise AssertionError(f"Registration summary payload_hex was empty\nPayload: {summary!r}")
    if len(payload_hex) % 2 != 0:
        raise AssertionError(f"payload_hex length must be even, got {len(payload_hex)}")
    valid_hex_digits = set("0123456789abcdefABCDEF")
    invalid = [ch for ch in payload_hex if ch not in valid_hex_digits]
    if invalid:
        raise AssertionError(f"payload_hex contained non-hex characters: {invalid!r}")



def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", type=pathlib.Path, required=True, help="Path to the sotc client executable")
    args = parser.parse_args()

    with tempfile.TemporaryDirectory() as tmpdir:
        config_path = pathlib.Path(tmpdir) / "sotc_test.cfg"
        config_path.write_text(
            textwrap.dedent(
                """
                # Base configuration that will be overridden by CLI arguments where applicable
                server_host = config.example
                server_port = 3979
                player_name = Config Bot
                headless = false
                coordinator_host = coordinator.example.net
                coordinator_port = 5000
                game_type = invite
                invite_code = CODE123
                listed_publicly = true
                allow_direct = true
                allow_stun = false
                allow_turn = false
                heartbeat_interval = 60
                advertised_grfs = 11112222,33334444
                """
            ).strip()
            + "\n",
            encoding="utf-8",
        )

        base_args = [
            "--config",
            str(config_path),
            "--server",
            "override.example:4500",
            "--player",
            "CLI Bot",
            "--headless",
            "--private",
            "--no-direct",
            "--allow-stun",
            "--heartbeat",
            "45",
            "--advertised-grf",
            "55556666",
        ]

        launch_output = run_client(args.binary, *base_args, "--dump-launch-options")
        launch_summary = parse_key_value_payload(launch_output)
        assert_launch_summary(launch_summary)

        registration_output = run_client(args.binary, *base_args, "--dump-registration")
        registration_summary = parse_key_value_payload(registration_output)
        assert_registration_summary(registration_summary)

    return 0


if __name__ == "__main__":
    sys.exit(main())
