#include <QApplication>
#include "UserLogger.h"
#include "DatabaseManager.h"
#include "mainwindow.h"
#include "ChatPageWidget.h"
#include <QSettings>
#include <QTranslator>
#include <QElapsedTimer>
#include <QIcon>


int main(int argc, char *argv[]) {
    QCoreApplication::setOrganizationName("DatingAgency");
    QCoreApplication::setApplicationName("Match++");

    QApplication a(argc, argv);

    QIcon appIcon(":/resources/icons/logo.jpg");
    a.setWindowIcon(appIcon);

    QSettings settings;
    QString langCode = settings.value("language", "ua").toString();

    QTranslator translator;
    if (translator.load(":/translations/translations/app_" + langCode + ".qm") ||
        translator.load(":/translations/app_" + langCode + ".qm")) {
        a.installTranslator(&translator);
        }

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


    return a.exec();
}
