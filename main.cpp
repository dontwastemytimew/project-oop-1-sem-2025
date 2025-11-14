#include <QApplication>
#include <QTranslator>
#include "UserLogger.h"
#include "DatabaseManager.h"
#include "mainwindow.h"
#include "Preference.h"
#include "MatchEngine.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QFile styleFile(":/resources/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QTextStream(&styleFile).readAll();
        a.setStyleSheet(styleSheet);
        styleFile.close();
    } else {
        qWarning("Failed to load style.qss!");
    }
  
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