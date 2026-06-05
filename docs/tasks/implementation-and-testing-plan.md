# Portable Osciloscope Implementation and Testing Plan

## Objective

Implement the project in roadmap order while adding automated tests and coverage checks whenever feasible at each step.

## Cross-Cutting Testing Rule

- For every implementation task, add tests when feasible at that stage.
- Run the coverage target for changes that affect executable code when local dependencies support it.
- Treat no coverage regression on changed code as the default completion requirement.
- If automation is not feasible yet, document:
  - what is currently untested
  - why it is not yet automated
  - manual verification performed
  - follow-up task to automate it
  - whether coverage was skipped and why

## Implementation Tasks

### T0 - Bootstrap repo for executable code

- Create src/, tests/, cmake/, and third-party dependency wiring.
- Add top-level CMakeLists.txt for C++20 + Qt6 Widgets + PortAudio.
- Add opt-in coverage tooling through CMake/CTest.
- Add basic CI workflow that configures and builds (no runtime audio tests yet).
- Done when: clean clone can configure and build app skeleton on Linux and Windows, and Linux CI can generate coverage reports.

### T1 - Qt app skeleton (roadmap priority #1)

- Implement main.cpp and MainWindow with placeholder central widget.
- Add app startup/shutdown lifecycle and minimal logging hooks.
- Done when: app launches with window on both target platforms.
- Tests (if feasible now):
  - Add application startup smoke verification in CI/headless mode.

### T2 - PortAudio init/shutdown (priority #2)

- Implement audio/AudioManager init/terminate with robust error handling.
- Expose typed error results for UI feedback (not just console logs).
- Done when: app reports PortAudio availability and handles failure gracefully.
- Tests:
  - Validate success and failure paths.
  - Verify lifecycle state transitions.

### T3 - Device enumeration + UI binding (priorities #3 and #4)

- Add input-device discovery API in AudioManager.
- Populate UI combo box with input devices, default selection behavior, refresh action.
- Done when: users can see/select available input devices at runtime.
- Tests:
  - Validate device descriptor mapping/filtering/sorting logic.
  - Verify selector population with mocked/fake backend.

### T4 - Audio capture lifecycle (priority #5)

- Implement stream open/start/stop against selected device.
- Add callback path that only copies samples to ring buffer (no heavy work in callback).
- Add clear stream-state model (Idle, Starting, Running, Stopping, Error).
- Done when: start/stop works repeatedly without crash/hang/dropout in basic use.
- Tests:
  - Validate start/stop/restart behavior.
  - Validate invalid transition and error handling.

### T5 - Ring buffer implementation

- Implement initial mutex-protected circular buffer API (push, pop/read window, overflow stats).
- Ensure thread-safe producer (audio callback) / consumer (processing/UI) usage.
- Done when: sustained capture runs with predictable memory use and no data races.
- Tests:
  - Validate capacity boundaries and overwrite behavior.
  - Validate producer/consumer correctness and edge conditions.

### T6 - Signal monitoring (priority #6 / MVP 3)

- Add lightweight consumer path that computes RMS from buffered samples.
- Show live signal-activity indicator in UI.
- Done when: speaking into mic updates indicator with stable, plausible values.
- Tests:
  - Deterministic RMS calculation tests using fixed sample fixtures.

### T7 - Waveform widget + render pipeline (priority #7 / MVP 4)

- Implement widgets/WaveformWidget with QPainter rendering from visualization buffer.
- Add fixed refresh timer and viewport abstraction for configurable time windows.
- Implement time-base selector presets (10 ms to 5 s).
- Done when: real-time waveform is smooth, responsive, and time-base changes work live.
- Tests (if feasible now):
  - Validate viewport/time-window logic.
  - Validate rendering data adapters.

### T8 - Thread isolation hardening

- Separate acquisition, processing, and GUI responsibilities per architecture doc.
- Add synchronization strategy for shared visualization buffers.
- Done when: no UI calls from audio thread; no blocking work in callback.
- Tests:
  - Validate thread-safe handoff and shared-buffer behavior.
  - Validate no GUI work is performed in audio callback context.

### T9 - Test foundation (aligned with AGENTS coverage guidance)

- Validate full MVP flow: enumerate -> select -> start -> monitor -> waveform -> stop.
- Tests:
  - Add at least one end-to-end smoke path in CI (mocked backend is acceptable where needed).
  - Document manual hardware verification for gaps.

### T10 - MVP release gate

- Confirm MVP deliverables align with `docs/roadmap/roadmap.md`.
- Validate MVP checklist: enumeration, capture, RMS, waveform, time-base control.
- Document known limitations and performance observations (latency/dropouts).
- Done when: all MVP deliverables in docs/roadmap/roadmap.md are met and demonstrated.
- Tests:
  - Ensure no regressions in existing tests.
  - Close high-risk test gaps before sign-off.

## Sequencing Constraint

- Keep roadmap order.
- Do not begin FFT/spectrum/spectrogram work until reliable audio capture and waveform rendering are complete.

## Dependency Graph

```text
T0 (Bootstrap)
  -> T1 (Qt Skeleton)
  -> T2 (PortAudio Lifecycle)

T1 -> T3 (Device Enumeration UI)
T2 -> T3 (Device Enumeration UI)

T2 -> T4 (Capture Lifecycle)
T3 -> T4 (Capture Lifecycle)

T4 -> T5 (Ring Buffer)
T5 -> T6 (RMS Monitoring)

T4 -> T7 (Waveform Rendering)
T5 -> T7 (Waveform Rendering)
T6 -> T7 (Waveform Rendering)

T4 -> T8 (Thread Isolation Hardening)
T5 -> T8 (Thread Isolation Hardening)
T7 -> T8 (Thread Isolation Hardening)

T8 -> T9 (MVP End-to-End Verification)
T9 -> T10 (MVP Release Gate)
```

## Critical Path

- `T0 -> T2 -> T4 -> T5 -> T7 -> T8 -> T9 -> T10`
- This path delivers MVP capture and waveform reliability before release sign-off.
