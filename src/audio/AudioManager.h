#pragma once

#include <string>

class AudioManager {
public:
  bool initialize();
  static void shutdown();
  [[nodiscard]] std::string lastError() const;

private:
  std::string last_error_;
};
