#include "MainWindow.h"

#include <QApplication>
#include <QComboBox>
#include <QLabel>

#include <cstdlib>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  const AudioResult result{
      .error_code = AudioErrorCode::InitializationFailed,
      .detail = "PortAudio initialization failed",
  };
  MainWindow failedWindow(result);
  const auto* status = failedWindow.findChild<QLabel*>("audioStatus");
  const auto* failedSelector = failedWindow.findChild<QComboBox*>("inputDeviceSelector");
  if (status == nullptr || status->text() != "Audio backend unavailable: PortAudio initialization failed" ||
      failedSelector == nullptr || failedSelector->isEnabled() ||
      failedSelector->currentText() != "Audio backend unavailable") {
    return EXIT_FAILURE;
  }

  const AudioResult availableResult{};
  MainWindow availableWindow(
      availableResult,
      {AudioInputDevice{.index = 4, .name = "Microphone A", .max_input_channels = 1, .default_sample_rate = 48000.0},
       AudioInputDevice{.index = 9, .name = "Microphone B", .max_input_channels = 2, .default_sample_rate = 44100.0}},
      9);
  const auto* selector = availableWindow.findChild<QComboBox*>("inputDeviceSelector");
  if (selector == nullptr || !selector->isEnabled() || selector->count() != 2 || selector->itemData(0).toInt() != 4 ||
      selector->itemData(1).toInt() != 9 || availableWindow.selectedInputDevice() != std::optional<int>(9)) {
    return EXIT_FAILURE;
  }

  MainWindow emptyWindow(availableResult);
  const auto* emptySelector = emptyWindow.findChild<QComboBox*>("inputDeviceSelector");
  return emptySelector != nullptr && !emptySelector->isEnabled() &&
                 emptySelector->currentText() == "No input devices available" &&
                 !emptyWindow.selectedInputDevice().has_value()
             ? EXIT_SUCCESS
             : EXIT_FAILURE;
}
