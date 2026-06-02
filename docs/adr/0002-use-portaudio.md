# ADR 0002: Use PortAudio for audio I/O

## Status

Accepted

## Decision

Use PortAudio as the cross-platform audio backend for MVP.

## Rationale

- Cross-platform abstraction
- Device enumeration
- Device selection
- WASAPI support on Windows
- ALSA/PulseAudio/PipeWire compatibility on Linux
- Avoid direct integration with native audio APIs during MVP
