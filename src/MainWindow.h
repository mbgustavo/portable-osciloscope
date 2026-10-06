#pragma once

#include <QMainWindow>

#include <optional>
#include <vector>

#include "audio/AudioManager.h"

class QComboBox;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(const AudioResult& audioResult, const std::vector<AudioInputDevice>& devices = {},
                      std::optional<int> defaultDevice = std::nullopt, QWidget* parent = nullptr);
  [[nodiscard]] std::optional<int> selectedInputDevice() const;

private:
  QComboBox* deviceSelector_{};
};
