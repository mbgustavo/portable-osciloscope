# AGENTS

## Repository Reality Check

- The repo now includes a bootstrap C++/Qt/PortAudio codebase (`src/`, `tests/`, `CMakeLists.txt`).
- Build/test/lint automation exists via CMake/CTest, `scripts/lint.sh`, GitHub Actions, and pre-commit.

## Verified Commands

- Configure: `cmake -S . -B build`
- Build: `cmake --build build --config Release`
- Test: `ctest --test-dir build --output-on-failure -C Release`
- Lint (format + CMake style): `./scripts/lint.sh`
- Lint (include clang-tidy): `./scripts/lint.sh --all` (requires configured `build/`)
- Pre-commit (requires pre-commit): `pre-commit run --all-files`
- Lint/CI details and configuration reference: `docs/rules/lint-and-ci.md`
- Coverage: `cmake -S . -B build-coverage -DCMAKE_BUILD_TYPE=Debug -DOSCILLOSCOPE_ENABLE_COVERAGE=ON` then `cmake --build build-coverage --target coverage`
- If local configure fails with missing `Qt6Config.cmake` or PortAudio, install dependencies first; CI reference is `.github/workflows/bootstrap-build.yml`.
- Coverage requires `gcovr` and is currently supported only with GCC/Clang-compatible compilers.

## Canonical Docs (use these first)

- Product requirements: `docs/product/product-spec.md`
- Architecture decisions and planned module boundaries: `docs/architecture/architecture.md`
- Implementation sequencing and scope boundaries: `docs/roadmap/roadmap.md`
- Lint, pre-commit, CI, and editor formatting: `docs/rules/lint-and-ci.md`

## Working Conventions For Future Code Changes

- Keep proposed code/layout aligned with the planned C++/Qt/PortAudio architecture in `docs/architecture/architecture.md` (audio callback -> ring buffer -> DSP -> GUI).
- Respect roadmap gating in `docs/roadmap/roadmap.md`: waveform/audio-capture milestones come before FFT/spectrum/spectrogram work.
- If docs and future executable config/scripts diverge, trust executable config/scripts and update docs to match.

## Agent Execution Notes

- Before claiming a command works, verify it exists in the repo (scripts, CMake targets, CI workflow, or task runner config).
- Keep edits focused and minimal; this repository is currently specification-first.

## Test Coverage Guidelines

- Add tests for all new behavior and bug fixes.
- Cover both core component behavior and end-to-end application flows where they provide confidence.
- Prioritize high-risk paths as they are implemented: audio capture flow, ring buffer handoff, DSP pipeline, and visualization data flow.
- Do not reduce coverage in touched areas without documenting the rationale and a follow-up plan.
- If a change cannot be automated yet, document the testing gap and the manual verification performed.
- Treat no coverage regression on changed code as the default expectation.
- Document manual verification and follow-up automation tasks when coverage cannot be collected for a change.
