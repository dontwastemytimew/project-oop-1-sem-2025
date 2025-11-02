#include "UserLogger.h"

void UserLogger::log(LogLevel level, const QString& message)
{
    QString prefix;
    switch (level) {
    case Info: prefix = "[INFO] "; break;
    case Warning: prefix = "[WARNING] "; break;
    case Error: prefix = "[ERROR] "; break;
    }

    QString fullMessage = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")
                          + prefix + message;

    qDebug().noquote() << fullMessage;

    QFile file("app_log.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << fullMessage << "\n";
        file.close();
    }
}
