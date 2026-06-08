#include "dsp/SignalMonitor.h"

#include <cmath>

float SignalMonitor::computeRms(const std::vector<float>& samples) {
  if (samples.empty()) {
    return 0.0f;
  }

  double sum = 0.0;
  for (float sample : samples) {
    sum += static_cast<double>(sample) * static_cast<double>(sample);
  }

  return static_cast<float>(std::sqrt(sum / static_cast<double>(samples.size())));
}
