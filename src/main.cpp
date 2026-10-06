#include <QApplication>
#include <QIcon>
#include <QTimer>
#include <QtLogging>

#include "MainWindow.h"
#include "audio/AudioManager.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QApplication::setWindowIcon(QIcon(":/icons/signal.svg"));
  qInfo() << "Application started";

  AudioManager audioManager;
  const AudioResult audioResult = audioManager.initialize();
  if (audioResult.succeeded()) {
    qInfo() << "PortAudio backend available";
  } else {
    qWarning() << "PortAudio backend unavailable:" << QString::fromStdString(audioResult.detail);
  }

  const std::vector<AudioInputDevice> inputDevices = audioManager.inputDevices();
  const std::optional<int> defaultInputDevice = audioManager.defaultInputDevice();
  MainWindow window(audioResult, inputDevices, defaultInputDevice);
  window.show();

  if (QApplication::arguments().contains("--smoke-test")) {
    QTimer::singleShot(0, &app, &QCoreApplication::quit);
  }

  const int exitCode = QApplication::exec();
  qInfo() << "Application stopped with exit code" << exitCode;
  return exitCode;
}
