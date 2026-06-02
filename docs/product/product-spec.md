# Portable Osciloscope - Product Requirements

## Overview

Portable Osciloscope is a cross-platform desktop application that captures audio from user-selected input devices and visualizes the signal in both the time and frequency domains.

The application targets Windows and Linux and is intended to evolve from a simple oscilloscope/spectrum analyzer into a more advanced signal analysis tool.

## Goals

### Primary Goals

- Enumerate available audio input devices
- Allow users to select an audio input source
- Capture audio in real time
- Display waveform in the time domain
- Display frequency spectrum using FFT
- Support Windows and Linux
- Maintain low-latency audio processing
- Keep architecture extensible for future DSP features

### Non-Goals (MVP)

- Audio recording/export
- Audio effects processing
- Plugin system
- Spectrograms
- Multi-channel visualization
- Network streaming
- AI-based analysis

## UI Layout (MVP)

```text
+------------------------------------------------+
| Input Device: [ComboBox]                       |
|                                                |
| [Start] [Stop]                                 |
+------------------------------------------------+
|                                                |
|               Waveform Display                 |
|                                                |
+------------------------------------------------+
```

## MVP UI Controls

The initial waveform viewer should provide:

- Input device selection
- Start capture
- Stop capture
- Time-base selection

Future UI controls may include:

- Amplitude scaling
- Trigger controls
- Zoom and pan
- Frequency range selection
