#include "MainWindow.h"

#include <QApplication>
#include <QLabel>

#include <cstdlib>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  const AudioResult result{
      .error_code = AudioErrorCode::InitializationFailed,
      .detail = "PortAudio initialization failed",
  };
  MainWindow window(result);
  const auto* status = window.findChild<QLabel*>();

  return status != nullptr && status->text() == "Audio backend unavailable: PortAudio initialization failed"
             ? EXIT_SUCCESS
             : EXIT_FAILURE;
}
