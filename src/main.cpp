#include <QApplication>
#include <QIcon>
#include <QTimer>
#include <QtLogging>

#include "MainWindow.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QApplication::setWindowIcon(QIcon(":/icons/signal.svg"));
  qInfo() << "Application started";

  MainWindow window;
  window.show();

  if (QApplication::arguments().contains("--smoke-test")) {
    QTimer::singleShot(0, &app, &QCoreApplication::quit);
  }

  const int exitCode = QApplication::exec();
  qInfo() << "Application stopped with exit code" << exitCode;
  return exitCode;
}
