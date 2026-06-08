# Lint, Pre-commit, and CI

This document describes how code quality checks are configured and run in the Osciloscope repository.

## Overview

Quality checks are enforced at three layers:

| Layer | When it runs | Behavior |
| --- | --- | --- |
| **Editor** | On save (optional) | Auto-formats C++ and CMake files |
| **Pre-commit** | Before each `git commit` | Auto-fixes format; runs clang-tidy |
| **CI** | On push and pull requests | Check-only; fails if anything is wrong |

All layers share the same configuration files at the repository root.

```text
.clang-format          C++ formatting rules
.clang-tidy            C++ static analysis rules
.cmake-format.py       CMake formatting rules
scripts/lint.sh        Local and CI lint runner
.pre-commit-config.yaml Pre-commit hook definitions
.github/workflows/ci.yml GitHub Actions workflow
.vscode/settings.json  Editor format-on-save settings
```

## Configuration files

### `.clang-format`

Defines C++ code style for `clang-format`. Key settings:

- Based on LLVM style
- 2-space indentation, 120-column limit
- Left pointer/reference alignment (`QWidget* parent`)
- Attached braces (`void foo() {`)
- Include sorting enabled (`SortIncludes: true`)

Used by:

- `scripts/lint.sh --format`
- Pre-commit `clang-format` hook
- Editor format-on-save (via clangd)

### `.clang-tidy`

Defines C++ static analysis checks for `clang-tidy`. Enabled groups:

- `bugprone-*` — likely bugs
- `clang-analyzer-*` — deeper static analysis
- `cppcoreguidelines-init-variables`, `cppcoreguidelines-pro-type-member-init` — initialization safety
- `modernize-*` — modern C++ usage
- `performance-*` — inefficient patterns
- `readability-*` — clarity issues

Several noisy checks are disabled (for example `readability-magic-numbers`, `readability-identifier-length`).

`HeaderFilterRegex: '(src|tests)/.*'` limits analysis to project headers under `src/` and `tests/`, not third-party includes.

Requires `build/compile_commands.json` from CMake.

### `.cmake-format.py`

Defines CMake formatting for `cmake-format` (from the `cmakelang` package). Key settings:

- 120-character line width
- 2-space indentation

Used by:

- `scripts/lint.sh --cmake`
- Pre-commit `cmake-format` hook
- Editor format-on-save (via cmake-format extension)

### `.pre-commit-config.yaml`

Configures [pre-commit](https://pre-commit.com) hooks:

| Hook | Source | Action |
| --- | --- | --- |
| `trailing-whitespace` | pre-commit-hooks | Remove trailing whitespace |
| `end-of-file-fixer` | pre-commit-hooks | Ensure files end with a newline |
| `check-yaml` | pre-commit-hooks | Validate YAML syntax |
| `check-added-large-files` | pre-commit-hooks | Block files larger than 512 KB |
| `clang-format` | mirrors-clang-format v19.1.4 | Auto-format staged C++ files |
| `cmake-format` | cmake-format-precommit v0.6.13 | Auto-format staged CMake files |
| `clang-tidy` | local (`scripts/lint.sh --tidy`) | Static analysis on all `src/` and `tests/` C++ files |

The clang-tidy hook uses `language: system`, so it requires `clang-tidy` and a configured `build/` directory on the developer machine.

### `.vscode/settings.json` and `.vscode/extensions.json`

Workspace editor settings for Cursor/VS Code:

- **clangd** reads `build/compile_commands.json` for IntelliSense
- **Format on save** is enabled for C/C++ and CMake
- C++ formatting uses **clangd** (reads `.clang-format`)
- CMake formatting uses the **cmake-format** extension (reads `.cmake-format.py`)

Recommended extensions: `llvm-vs-code-extensions.vscode-clangd`, `cheshirekow.cmake-format`.

## `scripts/lint.sh`

The central lint runner used locally and in CI.

### What it checks

| Flag | Check | Tools | Config |
| --- | --- | --- | --- |
| `--format` | C++ formatting | `clang-format --dry-run --Werror` | `.clang-format` |
| `--cmake` | CMake formatting | `cmake-format --check` | `.cmake-format.py` |
| `--tidy` | C++ static analysis | `clang-tidy` | `.clang-tidy` + `build/compile_commands.json` |
| `--all` | All of the above | — | — |
| *(no args)* | `--format` + `--cmake` | — | — |

### File discovery

- **C++**: all `.cpp` and `.h` files under `src/` and `tests/`
- **CMake**: all `CMakeLists.txt` and `*.cmake` files in the repo, excluding `build/` and `build-*/`

### Environment overrides

```sh
CLANG_FORMAT=/path/to/clang-format ./scripts/lint.sh --format
CMAKE_FORMAT=/path/to/cmake-format ./scripts/lint.sh --cmake
CLANG_TIDY=/path/to/clang-tidy BUILD_DIR=build ./scripts/lint.sh --tidy
```

### Examples

```sh
# Quick format checks (no build required)
./scripts/lint.sh

# Full lint including clang-tidy
cmake -S . -B build
./scripts/lint.sh --all

# Individual checks
./scripts/lint.sh --format
./scripts/lint.sh --cmake
./scripts/lint.sh --tidy
```

## Pre-commit

### Setup

```sh
pip install pre-commit
pre-commit install
cmake -S . -B build   # required for the clang-tidy hook
```

### Usage

Pre-commit runs automatically on `git commit`. To run manually:

```sh
pre-commit run --all-files
```

### Behavior notes

- **Format hooks auto-fix** staged files. If a hook reformats a file, the commit is aborted; stage the changes and commit again.
- **clang-tidy** runs on all C++ sources in `src/` and `tests/` when any C++ file is staged, not just the staged files.
- To bypass hooks in an emergency: `git commit --no-verify` (not recommended).

## CI

Defined in `.github/workflows/ci.yml`. On push to `master`, pull requests, and manual `workflow_dispatch`, jobs run in this order:

### Parallel initial jobs

| Job | Platform | Purpose |
| --- | --- | --- |
| `linux-build` | Ubuntu | Configure and build; upload `linux-build` artifact |
| `windows-build` | Windows | Configure and build; upload `windows-build` artifact |
| `coverage` | Ubuntu | Coverage build, badge update on `master` |

### Jobs after `linux-build`

| Job | Depends on | Purpose |
| --- | --- | --- |
| `test` | `linux-build` | Download Linux build artifact and run CTest |
| `lint` | `linux-build` | Download Linux build artifact and run `./scripts/lint.sh --all` |

`lint` reuses the Linux build artifact for `compile_commands.json`; it does not reconfigure CMake.

### Manual release (`release-tag`)

Runs only on `workflow_dispatch` from `master`, after `linux-build` and `windows-build` complete. Reads the project version from `CMakeLists.txt`, creates a `v<version>` Git tag and GitHub release, and attaches zipped Linux and Windows build artifacts.

## Local dependencies

| Tool | Used by | Install (Debian/Ubuntu) |
| --- | --- | --- |
| `clang-format` | `lint.sh --format` | `sudo apt install clang-format` |
| `clang-tidy` | `lint.sh --tidy`, pre-commit | `sudo apt install clang-tidy` |
| `cmake-format` | `lint.sh --cmake`, pre-commit, editor | `pip install cmakelang` |
| `pre-commit` | Git hooks | `pip install pre-commit` |
| `pip` | pre-commit | `sudo apt install python3-pip` |

Pre-commit's `clang-format` hook bundles its own binary and does not require a system install. `scripts/lint.sh` and CI use the system `clang-format`.

## Troubleshooting

### `compile_commands.json not found`

Configure the project first:

```sh
cmake -S . -B build
```

### Pre-commit clang-tidy fails on a fresh clone

Install `clang-tidy` and configure the build directory before committing C++ changes.

### Editor format-on-save does not work

1. Install recommended extensions from `.vscode/extensions.json`
2. Run `cmake -S . -B build` so clangd can format with correct context
3. For CMake, install `cmake-format` (`pip install cmakelang`)
4. Reload the editor window after installing extensions
