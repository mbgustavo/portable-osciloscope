#include "audio/AudioManager.h"

#include <portaudio.h>

bool AudioManager::initialize() {
  const PaError err = Pa_Initialize();
  if (err != paNoError) {
    last_error_ = Pa_GetErrorText(err);
    return false;
  }
  last_error_.clear();
  return true;
}

void AudioManager::shutdown() {
  Pa_Terminate();
}

std::string AudioManager::lastError() const {
  return last_error_;
}
