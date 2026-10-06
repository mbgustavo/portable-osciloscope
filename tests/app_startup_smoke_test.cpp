#include <QDebug>
#include <QProcess>

#include <cstdlib>

int main(int argc, char* argv[]) {
  // CTest passes the built application path as the sole argument.
  if (argc != 2) {
    qCritical() << "Expected the application executable path.";
    return EXIT_FAILURE;
  }

  QProcess process;
  process.setProgram(QString::fromLocal8Bit(argv[1]));
  process.setArguments({QStringLiteral("--smoke-test")});
  process.start();

  // Catch launch failures and hangs before checking the application's exit result.
  if (!process.waitForStarted()) {
    qCritical() << "Failed to launch application:" << process.errorString();
    return EXIT_FAILURE;
  }

  if (!process.waitForFinished(5000)) {
    qCritical() << "Application did not exit promptly:" << process.errorString();
    return EXIT_FAILURE;
  }

  // Treat both crashes and ordinary nonzero exits as smoke-test failures.
  if (process.exitStatus() == QProcess::CrashExit) {
    qCritical() << "Application crashed:" << process.readAllStandardError();
    return EXIT_FAILURE;
  }

  if (process.exitCode() != EXIT_SUCCESS) {
    qCritical() << "Application exited with code" << process.exitCode() << ":" << process.readAllStandardError();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
