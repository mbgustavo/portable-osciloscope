#pragma once

#include <QWidget>

class WaveformWidget : public QWidget {
  Q_OBJECT

public:
  explicit WaveformWidget(QWidget* parent = nullptr);

protected:
  // Paint the widget's current placeholder waveform and background.
  void paintEvent(QPaintEvent* event) override;
};
