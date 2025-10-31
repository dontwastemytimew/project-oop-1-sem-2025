#include <QApplication>
#include <QTranslator>
#include "UserLogger.h"
#include "DatabaseManager.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    UserLogger::init("app_log.txt");
    UserLogger::log(Info, "Application starting up.");

    // Ініціалізація Інтернаціоналізації
    QTranslator translator;
    if (translator.load(":/translations/app_ua.qm")) {
        a.installTranslator(&translator);
        UserLogger::log(Info, "Ukrainian translator loaded.");
    } else {
        UserLogger::log(Warning, "Ukrainian translation file not found.");
    }

    DatabaseManager dbManager;
    if (!dbManager.openDatabase()) {
        UserLogger::log(Critical, "Failed to open main database!");
        return -1;
    }

    UserLogger::log(Info, "Application running.");


    return a.exec();
}