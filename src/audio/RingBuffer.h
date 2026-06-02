#pragma once

#include <cstddef>
#include <vector>

class RingBuffer {
public:
    explicit RingBuffer(std::size_t capacity = 4096);
    void push(float sample);
    std::vector<float> snapshot() const;

private:
    std::vector<float> data_;
};
