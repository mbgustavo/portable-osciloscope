#include "MainWindow.h"

#include <QLabel>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Osciloscope");
    resize(960, 540);

    auto* placeholder = new QLabel("Portable Osciloscope bootstrap complete", this);
    placeholder->setAlignment(Qt::AlignCenter);
    setCentralWidget(placeholder);
}
