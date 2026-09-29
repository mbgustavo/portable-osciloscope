#pragma once

#include <cstdint>
#include <string>

enum class AudioBackendState : std::uint8_t {
  Uninitialized,
  Initialized,
  InitializationFailed,
};

enum class AudioErrorCode : std::uint8_t {
  None,
  InitializationFailed,
  TerminationFailed,
};

struct AudioResult {
  AudioErrorCode error_code{AudioErrorCode::None};
  std::string detail;

  [[nodiscard]] bool succeeded() const;
};

class AudioManager {
public:
  class Backend {
  public:
    virtual ~Backend() = default;

    virtual int initialize() = 0;
    virtual int terminate() = 0;
    [[nodiscard]] virtual std::string errorText(int error_code) const = 0;
  };

  AudioManager();
  explicit AudioManager(Backend& backend);
  ~AudioManager();

  AudioManager(const AudioManager&) = delete;
  AudioManager& operator=(const AudioManager&) = delete;
  AudioManager(AudioManager&&) = delete;
  AudioManager& operator=(AudioManager&&) = delete;

  [[nodiscard]] AudioResult initialize();
  [[nodiscard]] AudioResult shutdown();
  [[nodiscard]] AudioBackendState state() const;

private:
  Backend& backend_;
  AudioBackendState state_{AudioBackendState::Uninitialized};
};
