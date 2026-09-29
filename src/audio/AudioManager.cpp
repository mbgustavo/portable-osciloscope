#include "audio/AudioManager.h"

#include <portaudio.h>

namespace {

class PortAudioBackend final : public AudioManager::Backend {
public:
  int initialize() override {
    return Pa_Initialize();
  }

  int terminate() override {
    return Pa_Terminate();
  }

  [[nodiscard]] std::string errorText(int errorCode) const override {
    const char* const errorText = Pa_GetErrorText(static_cast<PaError>(errorCode));
    return errorText == nullptr ? "Unknown PortAudio error" : errorText;
  }
};

AudioManager::Backend& defaultBackend() {
  static PortAudioBackend backend;
  return backend;
}

} // namespace

bool AudioResult::succeeded() const {
  return error_code == AudioErrorCode::None;
}

AudioManager::AudioManager() : AudioManager(defaultBackend()) {}

AudioManager::AudioManager(Backend& backend) : backend_(backend) {}

AudioManager::~AudioManager() {
  static_cast<void>(shutdown());
}

AudioResult AudioManager::initialize() {
  if (state_ == AudioBackendState::Initialized) {
    return {};
  }

  const int errorCode = backend_.initialize();
  if (errorCode != paNoError) {
    state_ = AudioBackendState::InitializationFailed;
    return {.error_code = AudioErrorCode::InitializationFailed, .detail = backend_.errorText(errorCode)};
  }

  state_ = AudioBackendState::Initialized;
  return {};
}

AudioResult AudioManager::shutdown() {
  if (state_ != AudioBackendState::Initialized) {
    return {};
  }

  const int errorCode = backend_.terminate();
  if (errorCode != paNoError) {
    return {.error_code = AudioErrorCode::TerminationFailed, .detail = backend_.errorText(errorCode)};
  }

  state_ = AudioBackendState::Uninitialized;
  return {};
}

AudioBackendState AudioManager::state() const {
  return state_;
}
