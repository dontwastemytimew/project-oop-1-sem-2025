#include "ChatManager.h"
#include <QtGlobal>
#include <QRandomGenerator>
#include <QDebug>
#include <algorithm>
#include "DatabaseManager.h"

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

    if (m_dbManager) {
        m_dbManager->saveChatMessage(msg);
    }
    qDebug() << "Message sent and stored.";
}

QList<ChatMessage> ChatManager::getMessages(int userId, int targetId) const {
    if (m_dbManager) {
        return m_dbManager->loadChatHistory(userId, targetId);
    }
    return QList<ChatMessage>();
}

QString ChatManager::getBotReply() const {
    int index = QRandomGenerator::global()->bounded(botReplies.size());
    return botReplies[index];
}