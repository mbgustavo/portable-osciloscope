#include "dsp/SignalMonitor.h"

#include <cmath>
#include <cstdlib>
#include <vector>

int main() {
  // Alternating unit-amplitude samples have an RMS value of exactly one.
  const std::vector<float> samples{1.0f, -1.0f, 1.0f, -1.0f};
  const float rms = SignalMonitor::computeRms(samples);
  // Allow a small tolerance for floating-point rounding in the RMS calculation.
  if (std::fabs(rms - 1.0f) > 1e-5f) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
