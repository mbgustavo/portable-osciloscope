# T0 Plan - Bootstrap Repository

## Goal

Create the first executable project skeleton so future tasks can implement features and tests incrementally.

## Scope

- Set up baseline C++20 + CMake project layout.
- Wire Qt Widgets and PortAudio dependency scaffolding.
- Add initial CI build workflow.
- Add test framework wiring and one smoke test target if feasible.
- Add opt-in coverage reporting for executable code.

## Deliverables

- Top-level `CMakeLists.txt` with build options and standard compiler settings.
- Initial source tree:
  - `src/main.cpp`
  - `src/MainWindow.h`
  - `src/MainWindow.cpp`
  - `src/audio/`
  - `src/dsp/`
  - `src/widgets/`
- Initial test tree:
  - `tests/CMakeLists.txt`
  - one minimal smoke test target (or documented reason if deferred)
- Coverage target that runs tests and writes reports under `build-coverage/coverage/`.
- CI workflow to configure, build, test, and run Linux coverage.
- Short setup notes in docs.

## Execution Steps

1. Create build skeleton

- Add root `CMakeLists.txt` for C++20.
- Define app target and include directories.
- Add dependency discovery placeholders for Qt6 Widgets and PortAudio.
- Fail with clear messages if required dependencies are missing.

2. Create minimal app entry

- Implement `main.cpp` and a minimal `MainWindow`.
- Ensure the app opens a basic window and exits cleanly.

3. Prepare module boundaries

- Create folders and placeholder headers/sources for `audio`, `dsp`, and `widgets`.
- Keep structure aligned with `docs/architecture/architecture.md`.

4. Add test wiring

- Enable CTest in CMake.
- Add `tests/` target and register at least one smoke test if feasible now.
- If runtime GUI/audio test is not feasible in CI, add a minimal non-GUI smoke target and document limitation.

5. Add CI bootstrap build

- Create workflow to configure and build on Linux and Windows.
- Keep Windows CI scope to build/smoke only for T0.
- Run coverage on Linux using the CMake `coverage` target.

6. Add coverage reporting

- Add `OSCILLOSCOPE_ENABLE_COVERAGE` CMake option.
- Generate text, HTML, and XML reports with `gcovr`.
- Limit coverage support to GCC/Clang-compatible compilers during bootstrap.

7. Document verified commands

- Record exact configure/build/test commands that were verified during T0.
- Update `AGENTS.md` only with facts confirmed by repo config.

## Testing Plan (T0)

- Required:
  - Build succeeds in CI for Linux and Windows.
  - CTest discovers at least one test target, if feasible.
  - Linux coverage job generates report artifacts.
- Preferred:
  - Minimal startup smoke verification.
- If not feasible yet:
  - Document unautomated check, reason, and follow-up task.

## Exit Criteria

- A fresh clone can configure and build with documented commands.
- CI executes and reports build status on both target OSes.
- Project structure supports T1-T3 without rework.
- Test wiring exists, with at least one smoke test or a documented short-term gap.
- Coverage wiring exists and can be run with the documented CMake target when `gcovr` and dependencies are installed.

## Risks and Mitigations

- Dependency discovery differs by OS.
  - Mitigation: use clear CMake errors and OS-specific CI setup notes.
- GUI tests may not run headless in early CI.
  - Mitigation: keep T0 smoke test non-GUI when needed; add GUI smoke in later tasks.
- PortAudio packaging variance across environments.
  - Mitigation: decouple compile-time discovery from runtime device checks in T0.
- Coverage support differs by compiler.
  - Mitigation: support GCC/Clang first and keep Windows/MSVC coverage out of T0 scope.
