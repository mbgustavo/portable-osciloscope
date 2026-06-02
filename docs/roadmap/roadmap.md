# Portable Osciloscope - Implementation Plan

## MVP Scope

### MVP 1 - Device Enumeration

Features:

- Initialize PortAudio
- Enumerate audio input devices
- Populate device selector
- Start application successfully

Deliverable:

User can see available input devices.

### MVP 2 - Audio Capture

Features:

- Select device
- Start stream
- Stop stream
- Capture audio samples

Deliverable:

Application successfully receives audio data.

### MVP 3 - Signal Monitoring

Features:

- Calculate RMS level
- Display signal activity

Deliverable:

User can verify audio capture is functioning.

### MVP 4 - Time Domain Visualization

Features:

- Waveform widget
- Real-time rendering
- Fixed refresh loop
- Adjustable time span (time base)

Example time-base presets:

- 10 ms
- 20 ms
- 50 ms
- 100 ms
- 200 ms
- 500 ms
- 1 s
- 2 s
- 5 s

The waveform widget should display a configurable amount of recent audio samples based on the selected time span.

Deliverable:

Basic oscilloscope-style display with adjustable time-base controls.

## Post-MVP Roadmap

### Phase 1 - Frequency Analysis

Features:

- FFTW integration
- Frequency spectrum visualization
- Adjustable FFT size
- Frequency range selection

Example FFT sizes:

- 512
- 1024
- 2048
- 4096
- 8192

Deliverable:

Interactive spectrum analyzer view.

### Phase 2 - Spectrogram

Features:

- Scrolling spectrogram
- Frequency heatmap
- Time-frequency visualization

Deliverable:

Real-time spectrogram display.

### Phase 3 - Signal Diagnostics

Features:

- Device capabilities panel
- Sample rate display
- Channel count display
- Latency information
- Buffer size information

Deliverable:

Advanced signal debugging information.

### Phase 4 - Advanced DSP

Features:

- Window function selection
- Peak detection
- Frequency markers
- Noise floor estimation
- Signal statistics

Deliverable:

Professional-grade analysis tools.

### Phase 5 - Recording

Features:

- WAV recording
- Recording controls
- Session management

Deliverable:

Audio capture and playback workflows.

### Phase 6 - Multi-Channel Support

Features:

- Stereo visualization
- Multi-channel interfaces
- Channel selection

Deliverable:

Support for professional audio hardware.

### Phase 7 - Performance Improvements

Features:

- Lock-free ring buffer
- GPU-assisted rendering
- Optimized FFT pipelines

Deliverable:

Improved scalability and lower latency.

### Phase 8 - Professional Features

Potential features:

- Trigger modes
- Oscilloscope controls
- Time-base presets
- Vertical amplitude scaling
- Zoom and pan
- Spectrum averaging
- Waterfall display
- Marker measurements
- Calibration tools

Deliverable:

Engineering-grade signal analysis application.

## Initial Development Priorities

Order of implementation:

1. Qt application skeleton
2. PortAudio initialization
3. Device enumeration
4. Device selection UI
5. Audio capture
6. RMS monitoring
7. Waveform rendering
8. FFT integration
9. Spectrum rendering
10. Spectrogram

No FFT or advanced DSP work should begin until reliable audio capture and waveform rendering are completed.
