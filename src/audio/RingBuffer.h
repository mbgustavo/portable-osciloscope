#pragma once

#include <cstddef>
#include <vector>

class RingBuffer {
public:
  // Creates a fixed-size buffer whose snapshots are ordered from oldest to newest sample.
  explicit RingBuffer(std::size_t capacity = 4096);
  // Appends a sample, discarding the oldest one when the buffer is full.
  void push(float sample);
  [[nodiscard]] std::vector<float> snapshot() const;

private:
  std::vector<float> data_;
};
