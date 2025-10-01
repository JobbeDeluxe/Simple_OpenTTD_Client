# Developer Setup

This guide explains how to bootstrap the Simple OpenTTD Client development
environment on Linux, macOS, and Windows. The scripts in `tools/` automate the
vcpkg checkout that provides the project's third-party dependencies.

## Prerequisites
- Git 2.40 or newer.
- CMake 3.21+ with the Ninja generator (recommended) or your preferred build
  system.
- A C++20-capable compiler:
  - GCC 11+ or Clang 13+ on Linux and macOS.
  - Visual Studio 2022 on Windows.
- Python 3.9+ for auxiliary scripts.

> **Note**
> The repository does not bundle the vcpkg submodule. The scripts below clone
> the upstream repository into a local folder so that developers can manage
> updates independently.

## Bootstrapping vcpkg (Linux/macOS)

From the repository root, run:

```bash
./tools/bootstrap_vcpkg.sh
```

The script performs the following actions:

1. Clones the official `microsoft/vcpkg` repository into `./.vcpkg` (or the
   location defined via `VCPKG_ROOT`).
2. Runs `bootstrap-vcpkg.sh -disableMetrics` to produce the `vcpkg` binary.
3. Installs the dependencies declared in `vcpkg.json` for the detected host
   triplet (defaults to `x64-linux`).

Additional options:

- `./tools/bootstrap_vcpkg.sh --directory /path/to/vcpkg` – clone vcpkg into a
  custom directory.
- `./tools/bootstrap_vcpkg.sh --triplet x64-linux --triplet x64-osx` – install
  libraries for multiple triplets in one pass.
- `./tools/bootstrap_vcpkg.sh --no-install` – skip the dependency installation
  step if you only need a bootstrapped checkout.

After the script finishes, export `VCPKG_ROOT` so CMake can locate the
manifest-based toolchain:

```bash
export VCPKG_ROOT="$(pwd)/.vcpkg"
```

Consider adding this line to your shell profile for persistence.

## Bootstrapping vcpkg (Windows)

Open a PowerShell prompt with the necessary execution policy (e.g.
`Set-ExecutionPolicy -Scope CurrentUser RemoteSigned`) and run:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\bootstrap_vcpkg.ps1
```

The PowerShell script mirrors the behaviour of the Bash helper by cloning
vcpkg, bootstrapping the executable, and installing dependencies for the
`x64-windows` triplet by default. Pass `-Triplet x64-windows-static` to install
additional variants or `-NoInstall` to skip the install step entirely.

Once complete, set the environment variable so that future shells and CMake
runs pick up the manifest automatically:

```powershell
$env:VCPKG_ROOT = "${PWD}\.vcpkg"
```

To persist the setting, add the line above to your PowerShell profile (run
`notepad $PROFILE` to edit it).

## Next Steps

With vcpkg configured you can proceed to configure the build. Example commands
are available in the project `README.md`. On first configure, CMake will reuse
any libraries already installed via the manifest. If you wish to keep
artefacts separate, use out-of-tree build directories such as `build/linux` or
`build/msvc`.

If you encounter issues with vcpkg integration, consult the official
[documentation](https://learn.microsoft.com/vcpkg/) or inspect the
`vcpkg.json` manifest in this repository for the full dependency list.
