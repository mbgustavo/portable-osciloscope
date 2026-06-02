# Portable Osciloscope - Architecture Decisions

## Technology Stack

### Language

- C++20

### GUI Framework

- Qt 6
- Qt Widgets (not QML)
- Decision rationale: `docs/adr/0001-use-qt-widgets.md`

### Audio Backend

- PortAudio
- Decision rationale: `docs/adr/0002-use-portaudio.md`

### Build System

- CMake
- Decision rationale: `docs/adr/0003-use-cmake.md`

### FFT Library

Post-MVP Phase 1:

- FFTW
- Decision rationale: `docs/adr/0004-use-fftw.md`

## High-Level Architecture

```text
Audio Device
      ↓
PortAudio Stream
      ↓
Audio Callback
      ↓
Ring Buffer
      ↓
DSP Layer
      ↓
Visualization Layer
      ↓
Qt Widgets
```

## Architectural Principles

### 1. Audio Callback Must Stay Lightweight

The audio callback should only:

- Receive samples
- Push samples into a buffer
- Return immediately

The callback must not perform:

- FFT calculations
- UI updates
- Logging
- File I/O
- Heavy memory allocations

### 2. Separation of Concerns

The application should be divided into:

#### Audio Layer

Responsible for:

- Device enumeration
- Device selection
- Audio stream lifecycle
- Sample acquisition

#### DSP Layer

Responsible for:

- Signal processing
- FFT computation
- Signal metrics

#### Presentation Layer

Responsible for:

- Waveform rendering
- Spectrum rendering
- User interaction

### 3. Thread Isolation

Audio acquisition, DSP, and rendering should be isolated.

```text
Audio Thread
      ↓
Ring Buffer
      ↓
DSP Thread
      ↓
Shared Visualization Buffers
      ↓
GUI Thread
```

This architecture minimizes latency and avoids audio dropouts.

### 4. Configurable Visualization Windows

Visualization components should not assume a fixed display duration.

Waveform and spectrum widgets must support configurable viewing windows so that future controls such as:

- Time-base selection
- Zoom and pan
- Frequency-range selection
- Triggered acquisition

can be implemented without redesigning the rendering pipeline.

The rendering layer should operate on a configurable viewport abstraction rather than a fixed sample count.

## Project Structure

```text
src/
├── main.cpp
├── MainWindow.h
├── MainWindow.cpp
│
├── audio/
│   ├── AudioManager.h
│   ├── AudioManager.cpp
│   ├── RingBuffer.h
│   └── RingBuffer.cpp
│
├── dsp/
│   ├── FFTProcessor.h
│   └── FFTProcessor.cpp
│
├── widgets/
│   ├── WaveformWidget.h
│   ├── WaveformWidget.cpp
│   ├── SpectrumWidget.h
│   └── SpectrumWidget.cpp
```

## Core Components

### AudioManager

Responsibilities:

- Initialize PortAudio
- Enumerate devices
- Open streams
- Start/stop capture
- Forward samples to ring buffer

### RingBuffer

Responsibilities:

- Thread-safe sample transfer
- Decouple audio callback from consumers

Initial implementation:

- Mutex-protected circular buffer

Future implementation:

- Lock-free ring buffer

### FFTProcessor

Responsibilities:

- Windowing
- FFT execution
- Magnitude calculation
- Spectrum generation

Not required in the earliest milestones.

### WaveformWidget

Responsibilities:

- Display time-domain waveform
- Draw using QPainter
- Refresh at fixed frame rate

### SpectrumWidget

Responsibilities:

- Display frequency-domain spectrum
- Consume FFT output
- Draw using QPainter

Post-waveform milestone.
