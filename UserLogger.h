#ifndef USERLOGGER_H
#define USERLOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

enum LogLevel { Info, Warning, Error };

class UserLogger
{
public:
    static void log(LogLevel level, const QString& message);
};

#endif
