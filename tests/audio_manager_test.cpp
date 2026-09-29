#include "audio/AudioManager.h"

#include <portaudio.h>

#include <cstdlib>
#include <string>

namespace {

class FakeBackend final : public AudioManager::Backend {
public:
  int initialize_result{paNoError};
  int terminate_result{paNoError};
  int initialize_calls{0};
  int terminate_calls{0};
  std::string error_text{"Fake PortAudio error"};

  int initialize() override {
    ++initialize_calls;
    return initialize_result;
  }

  int terminate() override {
    ++terminate_calls;
    return terminate_result;
  }

  [[nodiscard]] std::string errorText(int errorCode) const override {
    static_cast<void>(errorCode);
    return error_text;
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

} // namespace

int main() {
  return testSuccessfulLifecycle() && testInitializationFailure() && testDestructorCleanup() && testTerminationFailure()
             ? EXIT_SUCCESS
             : EXIT_FAILURE;
}
