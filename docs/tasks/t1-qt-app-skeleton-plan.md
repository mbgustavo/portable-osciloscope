# T1 Plan - Qt Application Skeleton

## Goal

Complete the cross-platform Qt Widgets application shell so it displays a
placeholder main window, emits minimal lifecycle diagnostics, exits cleanly,
and has an automated headless startup verification.

The repository already contains a bootstrap `main.cpp` and `MainWindow`. This
task formalizes and completes that foundation without adding audio behavior.

## Non-Goals

- Initialize or terminate PortAudio.
- Enumerate audio devices or add device-selection controls.
- Implement capture, buffering, DSP, signal monitoring, or waveform rendering.
- Introduce a general logging framework or persistent application settings.

## Scope And Affected Components

- `src/main.cpp`
  - Own the Qt application lifecycle, application icon setup, and main event
    loop.
  - Add minimal startup and shutdown logging hooks.
  - Support a deterministic test-only launch mode.
- `src/MainWindow.h` and `src/MainWindow.cpp`
  - Retain the Qt Widgets `QMainWindow` boundary and a placeholder central
    widget.
  - Keep window title and initial geometry suitable for the future MVP layout.
- `tests/CMakeLists.txt`
  - Build and register an application startup smoke test.
- `tests/app_startup_smoke_test.cpp` (new)
  - Launch the built executable and assert that its test-only mode exits
    successfully.
- `CMakeLists.txt`
  - Adjust only if test target visibility or target dependencies require it.

## Design Decisions

### Lifecycle Logging

Use Qt's built-in `qInfo()` facility for concise application startup and clean
shutdown messages. This supplies diagnostics needed by the initial shell
without committing the project to a custom logging interface before T2 needs
typed audio errors for UI feedback.

### Test-Only Startup Mode

Add a narrow `--smoke-test` command-line mode in `main.cpp`. It must show the
main window, schedule a prompt event-loop exit, and return the normal Qt exit
status. The default interactive launch path must remain unchanged.

This mode is preferable to killing a process from the test because it verifies
Qt application construction, window creation, display setup, event-loop entry,
and orderly teardown.

### Headless Test Execution

Run the smoke test with `QT_QPA_PLATFORM=offscreen` through its CTest
environment. The test process should pass `--smoke-test` to the application,
enforce a short timeout, and fail on launch failure, timeout, crash, or
non-zero exit status. It must not access audio hardware or require a PortAudio
device.

## Implementation Steps

1. Review the existing startup path in `src/main.cpp` and retain its
   responsibility for creating `QApplication`, applying the resource icon,
   constructing `MainWindow`, showing it, and executing the event loop.
2. Add `qInfo()` calls around the normal application lifecycle: one after Qt
   application initialization and one after the event loop returns. Do not log
   from future real-time callback contexts.
3. Parse only the `--smoke-test` option needed by the automated test. When the
   option is present, schedule `QApplication::quit()` with a zero-delay Qt
   timer after the window is shown. Do not add an interactive CLI surface.
4. Keep `MainWindow` limited to the visual application shell: title, initial
   size, and centered placeholder central widget. Avoid includes and APIs from
   the audio, DSP, and waveform modules.
5. Add `tests/app_startup_smoke_test.cpp`. Use a process-launch API to invoke
   the application binary provided by CMake, pass `--smoke-test`, wait for it
   to finish, and report diagnostics on failure. Link only the required Qt Core
   functionality.
6. Update `tests/CMakeLists.txt` to build the smoke-test executable and add it
   to CTest. Pass the app target path to the test using a target-file generator
   expression rather than a hard-coded build path. Set
   `QT_QPA_PLATFORM=offscreen` as the test environment.
7. Make the smallest necessary CMake change so the test target is built after
   the application target. Do not alter PortAudio discovery or production
   application linkage.

## Testing And Verification

### Automated

- Configure with `cmake -S . -B build`.
- Build with `cmake --build build --config Release`.
- Run all CTest tests with
  `ctest --test-dir build --output-on-failure -C Release`.
- Confirm that `app_startup_smoke_test` passes with the offscreen Qt platform
  and that its failure diagnostics identify launch, timeout, crash, or exit
  code failures.
- Run `./scripts/lint.sh --all` after configuring the build directory.
- When dependencies are installed, run the documented coverage configuration
  and ensure the new source does not reduce coverage unexpectedly.

### Manual

- Launch `portable-osciloscope` normally on Linux and Windows.
- Verify the application icon, `Osciloscope` window title, initial window size,
  centered placeholder, and normal close behavior.
- Verify lifecycle messages are visible through Qt's normal logging output.

## Documentation And Configuration

- No product, architecture, or roadmap update is expected: T1 directly
  implements the existing Qt application skeleton priority.
- Update `docs/rules/lint-and-ci.md` only if the final CTest invocation or
  offscreen dependency differs from the documented CI behavior.
- Preserve the existing CI build artifact flow. The registered test will run in
  the existing Linux CTest job; any Windows runtime test limitation must be
  documented if discovered.

## Risks, Assumptions, And Open Questions

- The Qt installation used by CI must include the `offscreen` platform plugin.
  If it does not, retain the smoke test where supported and document the
  platform-specific automation gap rather than weakening the normal startup
  path.
- The short test timeout must be long enough for Windows CI process startup
  while still detecting a hung event loop quickly.
- This plan assumes `QApplication` construction does not require PortAudio;
  T2 must preserve that separation so failures in audio initialization can be
  surfaced gracefully in the UI.

## Exit Criteria

- The application opens a placeholder Qt Widgets main window and closes
  cleanly on Linux and Windows.
- Startup and shutdown lifecycle messages are emitted using Qt logging.
- A CTest smoke test launches the application headlessly, creates the window,
  enters the event loop, and exits successfully without audio hardware.
- The changes pass the repository's configure, build, CTest, and lint checks.
- No PortAudio, capture, DSP, or visualization functionality is introduced.
