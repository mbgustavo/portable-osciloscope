# T2 Plan - PortAudio Lifecycle

## Goal

Make `AudioManager` the sole owner of PortAudio initialization and termination,
with explicit lifecycle state and typed failures that the Qt presentation layer
can display. The application must continue to open when PortAudio is unavailable
and clearly report the backend status.

## Non-Goals

- Enumerate audio devices or add device-selection controls; that is T3.
- Open, start, stop, or receive samples from an audio stream; that is T4.
- Add a ring buffer, DSP processing, waveform rendering, or FFT work.
- Add a general-purpose logging, notification, or settings system.
- Depend on physical audio hardware for automated tests.

## Scope And Affected Components

- `src/audio/AudioManager.h` and `src/audio/AudioManager.cpp`
  - Replace the boolean/string error contract and unconditional static shutdown
    with an instance-owned lifecycle API.
  - Represent initialization outcomes and lifecycle state with project-defined,
    typed values suitable for later UI binding.
  - Ensure termination is attempted only for a successful initialization and is
    performed during orderly ownership teardown.
- `src/main.cpp`
  - Create the manager after `QApplication` is constructed, initialize it before
    the main window is shown, and retain it through the event loop.
  - Log concise availability or failure diagnostics without preventing the Qt
    application from running.
- `src/MainWindow.h` and `src/MainWindow.cpp`
  - Display a minimal audio-backend availability or unavailable status in the
    existing placeholder surface. Keep this temporary status boundary narrow so
    T3 can replace it with device-selection UI.
- `tests/audio_manager_test.cpp` (new) and `tests/CMakeLists.txt`
  - Add deterministic lifecycle tests using a fake PortAudio boundary.
- `tests/app_startup_smoke_test.cpp`
  - Extend only if needed to verify that the normal test launch remains
    successful when initialization is attempted.

## Interfaces And Design Decisions

### Lifecycle Ownership

`AudioManager` owns one successful `Pa_Initialize()` / `Pa_Terminate()` pair.
It is non-copyable and non-movable so its ownership cannot be duplicated. Its
destructor performs the same guarded shutdown used by explicit teardown, making
all normal early-return and Qt event-loop paths safe.

Use an explicit state model such as `Uninitialized`, `Initialized`, and
`InitializationFailed`. `initialize()` is idempotent after success and must not
call PortAudio a second time. `shutdown()` is idempotent and calls
`Pa_Terminate()` only while the manager owns an initialized backend. A failed
initialization never permits a termination call.

### Typed Result Contract

Expose a small project-owned result type, containing a typed status or error
code and a user-facing detail string. Map the PortAudio return code into that
type at the audio boundary; callers must not infer failure solely from a log
message or a mutable `lastError()` string. Preserve the PortAudio error text as
diagnostic detail, while the typed code lets later UI code distinguish available,
unavailable, and unexpected backend states without parsing text.

The API should expose the current lifecycle state for UI and future T3/T4
precondition checks. Do not expose stream or device concepts in this task.

### Testable PortAudio Boundary

Wrap only the PortAudio calls needed by T2: initialize, terminate, and error-text
lookup. Inject that narrow boundary into `AudioManager` for tests, while the
production constructor uses the real PortAudio implementation. This avoids
requiring hardware or forcing unit tests to manipulate PortAudio's process-wide
state, and keeps the wrapper small enough to remove or extend deliberately in
T3 and T4.

### Application Behavior

`main.cpp` initializes the manager during startup and passes the resulting typed
status to `MainWindow`. On success, emit an informational availability message.
On failure, emit a warning with diagnostic detail and show the window with an
unavailable status rather than exiting or attempting capture. On shutdown, let
the manager terminate PortAudio after the Qt event loop ends. No PortAudio calls
belong in `MainWindow`.

## Implementation Steps

1. Review the existing `AudioManager` call sites and retain PortAudio inclusion
   and linking solely within the audio layer. Define the public lifecycle state,
   typed result/error data, and ownership restrictions in its header.
2. Introduce the minimal production PortAudio-call adapter and the injectable
   interface or constructor seam used by tests. Keep PortAudio error-code mapping
   localized in `AudioManager.cpp`.
3. Implement guarded, idempotent initialize and shutdown behavior. Clear stale
   diagnostics on success, preserve the latest initialization failure detail,
   and ensure the destructor cannot terminate an uninitialized backend.
4. Update `main.cpp` to own a single `AudioManager` for the application lifetime,
   initialize it before showing `MainWindow`, and use the typed result for
   `qInfo()` or `qWarning()` diagnostics. Do not make initialization failure a
   process failure.
5. Add the smallest `MainWindow` input or setter required to render the backend
   status in the existing central label. Do not introduce device controls or a
   persistent status-bar design ahead of T3.
6. Add fake-backed unit tests for successful initialization, initialization
   failure, repeated initialization, explicit shutdown, destructor cleanup, and
   repeated shutdown. Assert both the observable result/state and the fake's
   initialize/terminate call counts.
7. Register the unit-test target in CTest, include the implementation sources it
   needs, and retain coverage instrumentation. Keep the existing offscreen app
   smoke test passing without an audio device.
8. Run the documented configure, build, CTest, lint, and, where dependencies are
   available, coverage commands. Record any platform-specific PortAudio runtime
   limitation only if it changes the documented workflow.

## Testing And Verification

### Automated

- Build fake-backed `AudioManager` tests without accessing audio hardware.
- Verify a successful initialization transitions to `Initialized`, reports a
  success result, and calls the backend initializer once.
- Verify a backend initialization error transitions to `InitializationFailed`,
  preserves typed error information and PortAudio diagnostic text, and never
  calls terminate.
- Verify repeated successful initialization and repeated shutdown are idempotent,
  including a destructor after explicit shutdown.
- Verify the existing `app_startup_smoke_test` still launches and exits through
  the offscreen Qt event loop when PortAudio initialization is part of startup.
- Run `cmake -S . -B build`, `cmake --build build --config Release`, and
  `ctest --test-dir build --output-on-failure -C Release`.
- Run `./scripts/lint.sh --all` after configuring `build`.
- When supported locally, run the documented coverage configuration and confirm
  the new audio lifecycle paths are covered without an unexplained regression.

### Manual

- Launch `portable-osciloscope` on Linux and Windows with a normal PortAudio
  installation; verify the window states that the audio backend is available.
- Launch in an environment where PortAudio initialization fails, when practical;
  verify the window remains usable, displays an unavailable state, and the
  diagnostic output identifies the failure without a crash or hang.
- Close the application after each outcome and verify orderly shutdown.

## Documentation And Configuration

- No product, architecture, or roadmap changes are expected: this task directly
  implements roadmap priority #2 and preserves the documented audio-to-buffer
  boundary for later tasks.
- Update `docs/rules/lint-and-ci.md` or CI configuration only if the new test has
  an actual runtime dependency beyond the existing PortAudio package and Qt
  offscreen setup.
- Do not modify CMake PortAudio discovery unless the new test target reveals a
  concrete visibility or linkage requirement.

## Risks, Assumptions, And Open Questions

- PortAudio initialization is process-global. This plan assumes one
  `AudioManager` exists in the application; future ownership requirements must
  be designed rather than allowing multiple independent managers.
- Backend initialization can fail due to host audio configuration even if the
  PortAudio library is installed. That outcome is expected and must remain
  non-fatal to the GUI.
- The exact public result names should follow established project style during
  implementation, but must remain project-owned rather than exposing a mutable
  error string as the only contract.
- T3 must decide where the temporary availability message sits relative to its
  device selector; it should reuse the typed state rather than reinvoking
  `Pa_Initialize()`.

## Exit Criteria

- `AudioManager` has explicit, idempotent, instance-owned PortAudio lifecycle
  management with no unconditional `Pa_Terminate()` call.
- Initialization success and failure are exposed through typed results and
  lifecycle state suitable for UI feedback.
- The application reports audio availability, remains open on backend failure,
  and terminates an owned backend cleanly on exit.
- Deterministic tests cover lifecycle success, failure, idempotence, and cleanup
  without an audio device.
- Configure, build, CTest, lint, and applicable coverage checks pass.
- No device enumeration, capture, buffering, DSP, or visualization functionality
  is introduced.
