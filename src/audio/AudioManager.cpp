#include "audio/AudioManager.h"

#include <portaudio.h>

#include <optional>

namespace {

class PortAudioBackend final : public AudioManager::Backend {
public:
  // Initialize PortAudio's process-wide host API state.
  int initialize() override {
    return Pa_Initialize();
  }

  // Release PortAudio's process-wide host API state.
  int terminate() override {
    return Pa_Terminate();
  }

  // Translate backend error codes to readable diagnostics.
  [[nodiscard]] std::string errorText(int errorCode) const override {
    const char* const errorText = Pa_GetErrorText(static_cast<PaError>(errorCode));
    return errorText == nullptr ? "Unknown PortAudio error" : errorText;
  }

  [[nodiscard]] int deviceCount() const override {
    return Pa_GetDeviceCount();
  }

  [[nodiscard]] int defaultInputDevice() const override {
    return Pa_GetDefaultInputDevice();
  }

  // Return details only for devices that can accept input audio.
  [[nodiscard]] std::optional<AudioInputDevice> inputDevice(int index) const override {
    const PaDeviceInfo* const info = Pa_GetDeviceInfo(index);
    if (info == nullptr || info->maxInputChannels <= 0) {
      return std::nullopt;
    }
    return AudioInputDevice{.index = index,
                            .name = info->name == nullptr ? "" : info->name,
                            .max_input_channels = info->maxInputChannels,
                            .default_sample_rate = info->defaultSampleRate};
  }
};

AudioManager::Backend& defaultBackend() {
  // Keep one backend instance alive for the default manager's reference lifetime.
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

std::vector<AudioInputDevice> AudioManager::inputDevices() const {
  std::vector<AudioInputDevice> devices;
  if (state_ != AudioBackendState::Initialized) {
    return devices;
  }

  const int count = backend_.deviceCount();
  if (count <= 0) {
    return devices;
  }
  devices.reserve(static_cast<std::size_t>(count));
  for (int index = 0; index < count; ++index) {
    const auto device = backend_.inputDevice(index);
    if (device.has_value() && device->index == index && device->max_input_channels > 0 && !device->name.empty()) {
      devices.push_back(*device);
    }
  }
  return devices;
}

std::optional<int> AudioManager::defaultInputDevice() const {
  if (state_ != AudioBackendState::Initialized) {
    return std::nullopt;
  }
  const int index = backend_.defaultInputDevice();
  if (index < 0) {
    return std::nullopt;
  }
  // Only report a default that survived the same validation used for the visible device list.
  const auto devices = inputDevices();
  for (const auto& device : devices) {
    if (device.index == index) {
      return index;
    }
  }
  return std::nullopt;
}
