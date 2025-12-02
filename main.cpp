#include <QApplication>
#include "UserLogger.h"
#include "DatabaseManager.h"
#include "mainwindow.h"
#include "BenchmarkTool.h"
#include "ChatPageWidget.h"
#include "ChatManager.h"


int main(int argc, char *argv[]) {

    // --- ПОЧАТОК БЕНЧМАРКУ ЗАПУСКУ ---
    // QElapsedTimer timer;
    // timer.start();
    // ---------------------------------------------

    QCoreApplication::setOrganizationName("DatingAgency");
    QCoreApplication::setApplicationName("Match++");

    QApplication a(argc, argv);

    QElapsedTimer startupTimer;
    startupTimer.start();
  
    UserLogger::init("app_log.txt");
    UserLogger::log(Info, "Application starting up.");

    DatabaseManager dbManager;
    if (!dbManager.openDatabase()) {
        UserLogger::log(Critical, "Failed to open main database!");
        return -1;
    }
    UserLogger::log(Info, "Application running.");


    // UI
    UserLogger::log(Info, "Initializing MainWindow...");
    MainWindow w(&dbManager);
    w.showMaximized();

    // --- КІНЕЦЬ БЕНЧМАРКУ ЗАПУСКУ ---
    // qInfo() << "--- STARTUP BENCHMARK ---";
    // qInfo() << QString("Total startup time (ms): %1").arg(timer.elapsed());
    // qInfo() << "-------------------------";
    // -------------------------------------------

    return a.exec();
}
