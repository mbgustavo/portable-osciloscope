#pragma once

#include <QMainWindow>

#include <optional>
#include <vector>

#include "audio/AudioManager.h"

class QComboBox;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  // Builds the status and device selector from the backend result and available input devices.
  explicit MainWindow(const AudioResult& audioResult, const std::vector<AudioInputDevice>& devices = {},
                      std::optional<int> defaultDevice = std::nullopt, QWidget* parent = nullptr);
  // Returns the selected backend device index only while device selection is available.
  [[nodiscard]] std::optional<int> selectedInputDevice() const;

private:
  QComboBox* deviceSelector_{};
};
