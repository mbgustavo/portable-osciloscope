#include "audio/AudioManager.h"

#include <portaudio.h>

#include <cstdlib>
#include <optional>
#include <string>
#include <vector>

namespace {

class FakeBackend final : public AudioManager::Backend {
public:
  int initialize_result{paNoError};
  int terminate_result{paNoError};
  int initialize_calls{0};
  int terminate_calls{0};
  std::string error_text{"Fake PortAudio error"};
  std::vector<std::optional<AudioInputDevice>> devices;
  int default_input_device{-1};
  mutable int enumeration_calls{0};

  // Records the call and returns the configured result for lifecycle tests.
  int initialize() override {
    ++initialize_calls;
    return initialize_result;
  }

  // Records termination so tests can verify cleanup and idempotency.
  int terminate() override {
    ++terminate_calls;
    return terminate_result;
  }

  // Supplies deterministic error details without depending on PortAudio's host environment.
  [[nodiscard]] std::string errorText(int errorCode) const override {
    static_cast<void>(errorCode);
    return error_text;
  }

  // Exposes the configured device slots, including invalid entries used by filtering tests.
  [[nodiscard]] int deviceCount() const override {
    return static_cast<int>(devices.size());
  }

  // Returns the configured default device index, or -1 when no default is set.
  [[nodiscard]] int defaultInputDevice() const override {
    return default_input_device;
  }

  // Returns a configured device slot and counts accesses to verify initialization gating.
  [[nodiscard]] std::optional<AudioInputDevice> inputDevice(int index) const override {
    ++enumeration_calls;
    return index >= 0 && static_cast<std::size_t>(index) < devices.size() ? devices[static_cast<std::size_t>(index)]
                                                                          : std::nullopt;
  }
};

bool testSuccessfulLifecycle() {
  FakeBackend backend;
  {
    AudioManager manager(backend);

    const AudioResult initializeResult = manager.initialize();
    if (!initializeResult.succeeded() || manager.state() != AudioBackendState::Initialized ||
        backend.initialize_calls != 1) {
      return false;
    }

    const AudioResult repeatedInitializeResult = manager.initialize();
    if (!repeatedInitializeResult.succeeded() || backend.initialize_calls != 1) {
      return false;
    }

    const AudioResult shutdownResult = manager.shutdown();
    if (!shutdownResult.succeeded() || manager.state() != AudioBackendState::Uninitialized ||
        backend.terminate_calls != 1) {
      return false;
    }

    if (!manager.shutdown().succeeded() || backend.terminate_calls != 1) {
      return false;
    }
  }

  return backend.terminate_calls == 1;
}

bool testInitializationFailure() {
  // A failed initialization reports the backend error and must not attempt termination.
  FakeBackend backend;
  backend.initialize_result = paUnanticipatedHostError;
  backend.error_text = "PortAudio initialization failed";
  {
    AudioManager manager(backend);

    const AudioResult result = manager.initialize();
    if (result.succeeded() || result.error_code != AudioErrorCode::InitializationFailed ||
        result.detail != backend.error_text || manager.state() != AudioBackendState::InitializationFailed ||
        backend.initialize_calls != 1) {
      return false;
    }
  }

  return backend.terminate_calls == 0;
}

bool testDestructorCleanup() {
  // Leaving scope after successful initialization releases the backend automatically.
  FakeBackend backend;
  {
    AudioManager manager(backend);
    if (!manager.initialize().succeeded()) {
      return false;
    }
  }
  return backend.terminate_calls == 1;
}

bool testTerminationFailure() {
  // A failed termination keeps the manager initialized so callers can observe the unreleased state.
  FakeBackend backend;
  backend.terminate_result = paUnanticipatedHostError;
  AudioManager manager(backend);
  if (!manager.initialize().succeeded()) {
    return false;
  }

  const AudioResult result = manager.shutdown();
  return !result.succeeded() && result.error_code == AudioErrorCode::TerminationFailed &&
         result.detail == backend.error_text && manager.state() == AudioBackendState::Initialized &&
         backend.terminate_calls == 1;
}

bool testInputDeviceEnumeration() {
  // Enumeration is unavailable before initialization and filters output-only, missing, and unnamed devices.
  FakeBackend backend;
  backend.devices = {
      AudioInputDevice{.index = 0, .name = "Output only", .max_input_channels = 0, .default_sample_rate = 48000.0},
      AudioInputDevice{.index = 1, .name = "Microphone", .max_input_channels = 1, .default_sample_rate = 44100.0},
      std::nullopt, AudioInputDevice{.index = 3, .name = "", .max_input_channels = 2, .default_sample_rate = 48000.0}};
  backend.default_input_device = 1;
  AudioManager manager(backend);
  if (!manager.inputDevices().empty() || backend.enumeration_calls != 0) {
    return false;
  }
  if (!manager.initialize().succeeded()) {
    return false;
  }
  const auto devices = manager.inputDevices();
  return devices.size() == 1 && devices.front().index == 1 && devices.front().name == "Microphone" &&
         manager.defaultInputDevice() == std::optional<int>(1) && backend.enumeration_calls > 0;
}

bool testEnumerationAfterInitializationFailure() {
  // Device queries must not reach the backend when initialization did not succeed.
  FakeBackend backend;
  backend.initialize_result = paUnanticipatedHostError;
  backend.devices = {
      AudioInputDevice{.index = 0, .name = "Microphone", .max_input_channels = 1, .default_sample_rate = 48000.0}};
  AudioManager manager(backend);
  static_cast<void>(manager.initialize());
  return manager.inputDevices().empty() && !manager.defaultInputDevice().has_value() && backend.enumeration_calls == 0;
}

} // namespace

int main() {
  // Run every lifecycle and enumeration check; any failed case makes the executable fail.
  return testSuccessfulLifecycle() && testInitializationFailure() && testDestructorCleanup() &&
                 testTerminationFailure() && testInputDeviceEnumeration() && testEnumerationAfterInitializationFailure()
             ? EXIT_SUCCESS
             : EXIT_FAILURE;
}
