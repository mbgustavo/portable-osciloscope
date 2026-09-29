#include "MainWindow.h"

#include <QLabel>

MainWindow::MainWindow(const AudioResult& audioResult, QWidget* parent) : QMainWindow(parent) {
  setWindowTitle("Osciloscope");
  resize(960, 540);

  const QString status =
      audioResult.succeeded()
          ? QStringLiteral("Audio backend available")
          : QStringLiteral("Audio backend unavailable: %1").arg(QString::fromStdString(audioResult.detail));
  auto* placeholder = new QLabel(status, this);
  placeholder->setAlignment(Qt::AlignCenter);
  setCentralWidget(placeholder);
}
