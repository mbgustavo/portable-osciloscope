#!/usr/bin/env bash
# Run repository lint checks (format, CMake style, and optional clang-tidy).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

CLANG_FORMAT="${CLANG_FORMAT:-clang-format}"
CMAKE_FORMAT="${CMAKE_FORMAT:-cmake-format}"
CLANG_TIDY="${CLANG_TIDY:-clang-tidy}"
BUILD_DIR="${BUILD_DIR:-build}"

mapfile -t CXX_SOURCES < <(
  find src tests -type f \( -name '*.cpp' -o -name '*.h' \) -print | LC_ALL=C sort
)

mapfile -t CMAKE_SOURCES < <(
  find . -type f \( -name 'CMakeLists.txt' -o -name '*.cmake' \) \
    ! -path './build/*' ! -path './build-*/*' -print | LC_ALL=C sort
)

require_command() {
  if ! command -v "$1" >/dev/null 2>&1; then
    echo "error: required command not found: $1" >&2
    exit 1
  fi
}

run_format_check() {
  echo "==> Checking C++ formatting (clang-format)"
  require_command "$CLANG_FORMAT"
  "$CLANG_FORMAT" --dry-run --Werror -style=file "${CXX_SOURCES[@]}"
}

run_cmake_format_check() {
  echo "==> Checking CMake formatting (cmake-format)"
  require_command "$CMAKE_FORMAT"
  "$CMAKE_FORMAT" --check "${CMAKE_SOURCES[@]}"
}

run_clang_tidy() {
  local compile_db="${BUILD_DIR}/compile_commands.json"
  if [[ ! -f "$compile_db" ]]; then
    echo "error: ${compile_db} not found; configure the project first:" >&2
    echo "  cmake -S . -B ${BUILD_DIR}" >&2
    exit 1
  fi

  echo "==> Running clang-tidy"
  require_command "$CLANG_TIDY"

  local tidy_config="${ROOT}/.clang-tidy"
  local failed=0
  for source in "${CXX_SOURCES[@]}"; do
    if ! "$CLANG_TIDY" -p "$BUILD_DIR" --config-file="$tidy_config" "$source"; then
      failed=1
    fi
  done

  if [[ "$failed" -ne 0 ]]; then
    exit 1
  fi
}

usage() {
  cat <<EOF
Usage: $(basename "$0") [--format] [--cmake] [--tidy] [--all]

  --format   Check C++ formatting with clang-format (default)
  --cmake    Check CMake formatting with cmake-format
  --tidy     Run clang-tidy (requires configured build directory)
  --all      Run all checks
EOF
}

main() {
  local run_format=0
  local run_cmake=0
  local run_tidy=0

  if [[ $# -eq 0 ]]; then
    run_format=1
    run_cmake=1
  fi

  while [[ $# -gt 0 ]]; do
    case "$1" in
      --format) run_format=1 ;;
      --cmake) run_cmake=1 ;;
      --tidy) run_tidy=1 ;;
      --all) run_format=1; run_cmake=1; run_tidy=1 ;;
      -h|--help) usage; exit 0 ;;
      *) echo "error: unknown option: $1" >&2; usage; exit 1 ;;
    esac
    shift
  done

  if [[ "$run_format" -eq 1 ]]; then
    run_format_check
  fi
  if [[ "$run_cmake" -eq 1 ]]; then
    run_cmake_format_check
  fi
  if [[ "$run_tidy" -eq 1 ]]; then
    run_clang_tidy
  fi

  echo "Lint checks passed."
}

main "$@"
