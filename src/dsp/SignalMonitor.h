#pragma once

#include <vector>

class SignalMonitor {
public:
  // Compute the root-mean-square amplitude; an empty sample window has zero amplitude.
  static float computeRms(const std::vector<float>& samples);
};
