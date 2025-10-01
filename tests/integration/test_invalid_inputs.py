#!/usr/bin/env python3
"""Integration tests validating error handling for invalid inputs.

These checks exercise the client with intentionally malformed values to ensure
it exits with a non-zero status and surfaces helpful diagnostics. They run
without contacting live servers so they remain stable in continuous integration
setups.
"""

from __future__ import annotations

import argparse
import pathlib
import subprocess
import sys
import tempfile
import textwrap


def run_client(binary: pathlib.Path, *args: str) -> subprocess.CompletedProcess[str]:
    """Execute the client binary capturing stdout/stderr."""

    command = [str(binary), *args]
    return subprocess.run(
        command,
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )


def assert_failure(result: subprocess.CompletedProcess[str], message_fragment: str) -> None:
    """Ensure the process failed and mentioned the expected diagnostic."""

    if result.returncode == 0:
        raise AssertionError(
            f"Expected failure exit code but process succeeded.\n"
            f"stdout: {result.stdout!r}\n"
            f"stderr: {result.stderr!r}"
        )
    if message_fragment not in result.stderr:
        raise AssertionError(
            "Missing expected diagnostic in stderr.\n"
            f"Expected fragment: {message_fragment!r}\n"
            f"Actual stderr: {result.stderr!r}\n"
            f"stdout: {result.stdout!r}"
        )


def test_invalid_heartbeat(binary: pathlib.Path) -> None:
    result = run_client(binary, "--heartbeat", "-5", "--dump-launch-options")
    assert_failure(result, "Invalid heartbeat interval")


def test_invalid_config_flag(binary: pathlib.Path) -> None:
    with tempfile.TemporaryDirectory() as tmpdir:
        config_path = pathlib.Path(tmpdir) / "sotc_invalid.cfg"
        config_path.write_text(
            textwrap.dedent(
                """
                # Invalid allow_stun value to trigger parse failure
                allow_stun = maybe
                """
            ).strip()
            + "\n",
            encoding="utf-8",
        )

        result = run_client(binary, "--config", str(config_path), "--dump-launch-options")
        assert_failure(result, "Invalid allow_stun value")


def test_unknown_config_key(binary: pathlib.Path) -> None:
    with tempfile.TemporaryDirectory() as tmpdir:
        config_path = pathlib.Path(tmpdir) / "sotc_unknown.cfg"
        config_path.write_text(
            textwrap.dedent(
                """
                # Typo in configuration key should be rejected
                alow_direct = true
                """
            ).strip()
            + "\n",
            encoding="utf-8",
        )

        result = run_client(binary, "--config", str(config_path), "--dump-launch-options")
        assert_failure(result, "Unknown configuration key")


def test_duplicate_config_key(binary: pathlib.Path) -> None:
    with tempfile.TemporaryDirectory() as tmpdir:
        config_path = pathlib.Path(tmpdir) / "sotc_duplicate.cfg"
        config_path.write_text(
            textwrap.dedent(
                """
                allow_direct = true
                allow_direct = false
                """
            ).strip()
            + "\n",
            encoding="utf-8",
        )

        result = run_client(binary, "--config", str(config_path), "--dump-launch-options")
        assert_failure(result, "Duplicate configuration key")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", type=pathlib.Path, required=True, help="Path to the sotc client executable")
    args = parser.parse_args()

    test_invalid_heartbeat(args.binary)
    test_invalid_config_flag(args.binary)
    test_unknown_config_key(args.binary)
    test_duplicate_config_key(args.binary)
    return 0


if __name__ == "__main__":
    sys.exit(main())
