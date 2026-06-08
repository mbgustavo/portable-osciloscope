#pragma once

#include <vector>

class SignalMonitor {
public:
  static float computeRms(const std::vector<float>& samples);
};
