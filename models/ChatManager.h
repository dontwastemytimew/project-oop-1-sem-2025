#pragma once
#include <QString>
#include <QList>
#include <QMap>
#include <QDateTime>

struct ChatMessage {
    int fromUserId;
    int toUserId;
    QString message;
    QDateTime timestamp;
};

class ChatManager {
public:
    ChatManager();

    // Надіслати повідомлення від користувача
    void sendMessage(int fromUserId, int toUserId, const QString &message);

    // Отримати історію повідомлень між двома користувачами
    QList<ChatMessage> getMessages(int userId, int targetId) const;

    // Отримати відповідь бота (заглушка)
    QString getBotReply() const;

private:
    // Зберігаємо повідомлення локально у пам'яті 
    QMap<QPair<int,int>, QList<ChatMessage>> chatHistory;

    // Заготовки для бот-відповідей
    QList<QString> botReplies;
};
