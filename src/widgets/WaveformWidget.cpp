#include "widgets/WaveformWidget.h"

#include <QPainter>

WaveformWidget::WaveformWidget(QWidget* parent) : QWidget(parent) {}

void WaveformWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.fillRect(rect(), QColor(18, 18, 20));
    painter.setPen(QPen(QColor(98, 201, 255), 2));
    painter.drawLine(0, height() / 2, width(), height() / 2);
}
