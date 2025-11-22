#include "UserLogger.h"
#include <QCoreApplication>

QFile *UserLogger::logFile = nullptr;
bool UserLogger::initialized = false;

void UserLogger::init(const QString &fileName) {
    if (initialized) return;

    logFile = new QFile(fileName);
    if (logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        initialized = true;
        log(Info, "Logger initialized successfully.");
    } else {
        qCritical() << "Failed to open log file:" << logFile->errorString();
    }
}

void UserLogger::log(LogLevel level, const QString &message) {
    if (!initialized) {
        qWarning() << "Logger not initialized!";
        return;
    }

    QString levelStr;
    switch (level) {
        case Debug:    levelStr = "[DEBUG]"; break;
        case Info:     levelStr = "[INFO] "; break;
        case Warning:  levelStr = "[WARN] "; break;
        case Error:    levelStr = "[ERROR]"; break;
        case Critical: levelStr = "[CRIT] "; break;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString fullMessage = QString("%1 %2 %3").arg(timestamp, levelStr, message);

    // Вивід у консоль
    // qDebug() << fullMessage;

    // Вивід у файл
    QTextStream stream(logFile);
    stream << fullMessage << "\n";
    stream.flush();
}