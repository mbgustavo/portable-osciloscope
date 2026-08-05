#include <QApplication>
#include <QIcon>

#include "MainWindow.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QApplication::setWindowIcon(QIcon(":/icons/signal.svg"));
  MainWindow window;
  window.show();
  return QApplication::exec();
}
