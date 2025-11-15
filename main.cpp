#include <QApplication>
#include "UserLogger.h"
#include "DatabaseManager.h"
#include "mainwindow.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
  
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
    w.show();


    return a.exec();
}