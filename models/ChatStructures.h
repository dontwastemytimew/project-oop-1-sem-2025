#ifndef CHATSTRUCTURES_H
#define CHATSTRUCTURES_H

#include <QString>
#include <QDateTime>

/**
 * @brief Структура для зберігання повідомлення чату.
 */
struct ChatMessage {
    int fromUserId;
    int toUserId;
    QString message;
    QDateTime timestamp;
};

#endif // CHATSTRUCTURES_H