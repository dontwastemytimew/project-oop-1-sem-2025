#include "ChatManager.h"
#include <QtGlobal>
#include <QRandomGenerator>
#include <QDebug>
#include <algorithm>

ChatManager::ChatManager(DatabaseManager* dbManager, QObject* parent)
    : QObject(parent), m_dbManager(dbManager)
{
    // ініціалізуємо бот-відповіді
    botReplies = {
        QStringLiteral("Привіт! Класне фото!"), // Використовуємо
        QStringLiteral("Як справи?"),
        QStringLiteral("Що робиш?"),
        QStringLiteral("Гарний день сьогодні, правда?"),
        QStringLiteral("Класно виглядаєш!")
    };
}


void ChatManager::sendMessage(int fromUserId, int toUserId, const QString &message) {
    ChatMessage msg;
    msg.fromUserId = fromUserId;
    msg.toUserId = toUserId;
    msg.message = message;
    msg.timestamp = QDateTime::currentDateTime();

    QPair<int,int> key = (fromUserId < toUserId)
                       ? QPair<int,int>(fromUserId, toUserId)
                       : QPair<int,int>(toUserId, fromUserId);

    chatHistory[key].append(msg);
    qDebug() << "Message sent and stored.";
}

QList<ChatMessage> ChatManager::getMessages(int userId, int targetId) const {
    QList<ChatMessage> result;

    QPair<int,int> key1(userId, targetId);
    QPair<int,int> key2(targetId, userId);

    if (chatHistory.contains(key1)) {
        result.append(chatHistory.value(key1));
    }
    if (chatHistory.contains(key2)) {
        result.append(chatHistory.value(key2));
    }

    // сортуємо за часом
    std::sort(result.begin(), result.end(), [](const ChatMessage &a, const ChatMessage &b){
        return a.timestamp < b.timestamp;
    });

    return result;
}

QString ChatManager::getBotReply() const {
    int index = QRandomGenerator::global()->bounded(botReplies.size());
    return botReplies[index];
}