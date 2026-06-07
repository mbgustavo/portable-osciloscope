# Osciloscope

![Line Coverage](https://github.com/mbgustavo/portable-osciloscope/blob/master/coverage.svg)

Osciloscope is a cross-platform desktop audio analyzer planned for Windows and Linux. It will capture audio from user-selected input devices and visualize the signal in time and frequency domains.

The project is currently in bootstrap stage: the repository has a minimal C++20/Qt Widgets/PortAudio application skeleton, CMake build wiring, CTest wiring, and CI workflow.

## Documentation

- Product requirements: `docs/product/product-spec.md`
- Architecture: `docs/architecture/architecture.md`
- Roadmap: `docs/roadmap/roadmap.md`
- Implementation tasks: `docs/tasks/implementation-and-testing-plan.md`
- T0 bootstrap plan: `docs/tasks/t0-bootstrap-plan.md`
- Architecture decision records: `docs/adr/`

## Requirements

- CMake 3.21+
- C++20 compiler
- Qt 6.2+ with Widgets
- PortAudio development files
- Optional on Windows: vcpkg, using `vcpkg.json`

## Build

```sh
cmake -S . -B build
cmake --build build --config Release
```

If configure fails with missing `Qt6Config.cmake` or PortAudio, install the Qt6 and PortAudio development packages first or provide the relevant CMake paths.

## Test

```sh
ctest --test-dir build --output-on-failure -C Release
```

The current test suite contains bootstrap smoke coverage only. Tests should be added with each implementation task whenever feasible.

## Coverage

Coverage is opt-in and currently supported with GCC/Clang-compatible compilers.

```sh
cmake -S . -B build-coverage -DCMAKE_BUILD_TYPE=Debug -DOSCILLOSCOPE_ENABLE_COVERAGE=ON
cmake --build build-coverage --target coverage
```

The coverage target runs CTest and generates reports under `build-coverage/coverage/`:

- HTML: `build-coverage/coverage/index.html`
- XML: `build-coverage/coverage/coverage.xml`

Install `gcovr` before enabling coverage locally. CI runs coverage on Linux only.

## Current Scope

MVP work should follow the roadmap order:

1. Qt application skeleton
2. PortAudio initialization
3. Device enumeration
4. Device selection UI
5. Audio capture
6. RMS monitoring
7. Waveform rendering

FFT, spectrum, and spectrogram work should wait until reliable audio capture and waveform rendering are complete.
