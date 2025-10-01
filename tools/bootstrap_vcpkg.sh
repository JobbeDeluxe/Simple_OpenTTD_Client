#!/usr/bin/env bash
set -euo pipefail

print_usage() {
  cat <<'USAGE'
Usage: bootstrap_vcpkg.sh [options]

Options:
  -d, --directory PATH  Destination for the vcpkg checkout. Defaults to <repo>/.vcpkg or $VCPKG_ROOT when set.
  -t, --triplet NAME    vcpkg triplet to install (e.g. x64-linux, x64-windows). May be repeated. Defaults to host triplet.
      --no-install      Skip dependency installation and only ensure vcpkg is bootstrapped.
  -h, --help            Show this message and exit.
USAGE
}

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
default_vcpkg_dir="${VCPKG_ROOT:-"$repo_root/.vcpkg"}"
vcpkg_dir="$default_vcpkg_dir"
install_deps=1
triplets=()

while [[ $# -gt 0 ]]; do
  case "$1" in
    -d|--directory)
      shift
      [[ $# -gt 0 ]] || { echo "Missing argument for --directory" >&2; exit 1; }
      vcpkg_dir="$1"
      ;;
    -t|--triplet)
      shift
      [[ $# -gt 0 ]] || { echo "Missing argument for --triplet" >&2; exit 1; }
      triplets+=("$1")
      ;;
    --no-install)
      install_deps=0
      ;;
    -h|--help)
      print_usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      print_usage >&2
      exit 1
      ;;
  esac
  shift
done

mkdir -p "$(dirname "$vcpkg_dir")"

if [[ ! -d "$vcpkg_dir/.git" ]]; then
  echo "Cloning vcpkg into $vcpkg_dir"
  git clone https://github.com/microsoft/vcpkg.git "$vcpkg_dir"
else
  echo "Updating existing vcpkg checkout in $vcpkg_dir"
  git -C "$vcpkg_dir" pull --ff-only
fi

bootstrap_script="bootstrap-vcpkg.sh"
uname_out="$(uname -s 2>/dev/null || echo)"
case "$uname_out" in
  MINGW*|MSYS*|CYGWIN*)
    bootstrap_script="bootstrap-vcpkg.bat"
    ;;
  *)
    ;;
esac

pushd "$vcpkg_dir" >/dev/null
if [[ ! -x "$vcpkg_dir/vcpkg" ]]; then
  echo "Bootstrapping vcpkg"
  if [[ $bootstrap_script == *.bat ]]; then
    if command -v cmd.exe >/dev/null 2>&1; then
      cmd.exe /c "$bootstrap_script" -disableMetrics
    else
      echo "cmd.exe not available; cannot run $bootstrap_script" >&2
      exit 1
    fi
  else
    ./bootstrap-vcpkg.sh -disableMetrics
  fi
else
  echo "vcpkg already bootstrapped"
fi
popd >/dev/null

if [[ $install_deps -eq 1 ]]; then
  if [[ ${#triplets[@]} -eq 0 ]]; then
    case "$(uname -s 2>/dev/null || echo unknown)" in
      Linux*)
        triplets=("x64-linux")
        ;;
      Darwin*)
        triplets=("x64-osx")
        ;;
      MINGW*|MSYS*|CYGWIN*)
        triplets=("x64-windows")
        ;;
      *)
        triplets=("x64-linux")
        ;;
    esac
  fi
  for triplet in "${triplets[@]}"; do
    echo "Installing dependencies for triplet: $triplet"
    "$vcpkg_dir/vcpkg" install --triplet "$triplet"
  done
fi

echo "All done. Set VCPKG_ROOT=$vcpkg_dir or export it in your shell profile."
