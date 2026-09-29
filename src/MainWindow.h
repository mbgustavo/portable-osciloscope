#pragma once

#include <QMainWindow>

#include "audio/AudioManager.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(const AudioResult& audioResult, QWidget* parent = nullptr);
};
