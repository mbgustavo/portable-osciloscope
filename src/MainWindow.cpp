#include "MainWindow.h"

#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(const AudioResult& audioResult, const std::vector<AudioInputDevice>& devices,
                       std::optional<int> defaultDevice, QWidget* parent)
    : QMainWindow(parent) {
  setWindowTitle("Osciloscope");
  resize(960, 540);

  const QString status =
      audioResult.succeeded()
          ? QStringLiteral("Audio backend available")
          : QStringLiteral("Audio backend unavailable: %1").arg(QString::fromStdString(audioResult.detail));
  auto* container = new QWidget(this);
  auto* layout = new QVBoxLayout(container);
  auto* statusLabel = new QLabel(status, container);
  statusLabel->setObjectName(QStringLiteral("audioStatus"));
  statusLabel->setAlignment(Qt::AlignCenter);
  layout->addWidget(statusLabel);

  deviceSelector_ = new QComboBox(container);
  deviceSelector_->setObjectName(QStringLiteral("inputDeviceSelector"));
  deviceSelector_->setAccessibleName(QStringLiteral("Input Device"));
  if (!audioResult.succeeded()) {
    deviceSelector_->addItem(QStringLiteral("Audio backend unavailable"));
  } else if (devices.empty()) {
    deviceSelector_->addItem(QStringLiteral("No input devices available"));
  } else {
    for (const auto& device : devices) {
      deviceSelector_->addItem(QString::fromStdString(device.name), device.index);
    }
    if (defaultDevice.has_value()) {
      const int defaultRow = deviceSelector_->findData(*defaultDevice);
      if (defaultRow >= 0) {
        deviceSelector_->setCurrentIndex(defaultRow);
      }
    }
  }
  deviceSelector_->setEnabled(audioResult.succeeded() && !devices.empty());
  layout->addWidget(deviceSelector_);
  setCentralWidget(container);
}

std::optional<int> MainWindow::selectedInputDevice() const {
  if (deviceSelector_ == nullptr || !deviceSelector_->isEnabled() || deviceSelector_->currentIndex() < 0) {
    return std::nullopt;
  }
  bool validIndex = false;
  const int index = deviceSelector_->currentData().toInt(&validIndex);
  return validIndex ? std::optional<int>(index) : std::nullopt;
}
