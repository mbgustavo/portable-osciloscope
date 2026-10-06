#include "audio/RingBuffer.h"

RingBuffer::RingBuffer(std::size_t capacity) : data_(capacity, 0.0f) {}

void RingBuffer::push(float sample) {
  if (data_.empty()) {
    return;
  }
  // Shift the fixed-size window so its newest value is always at the end.
  data_.erase(data_.begin());
  data_.push_back(sample);
}

std::vector<float> RingBuffer::snapshot() const {
  return data_;
}
