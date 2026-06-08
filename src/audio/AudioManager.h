#pragma once

#include <string>

class AudioManager {
public:
  bool initialize();
  void shutdown();
  std::string lastError() const;

private:
  std::string last_error_;
};
