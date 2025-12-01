#include "ChatManager.h"
#include <QtGlobal>
#include <QRandomGenerator>
#include <QDebug> // Для виведення логів
#include <algorithm> // Для std::sort

// --- НОВИЙ КОНСТРУКТОР (з dbManager та parent) ---
ChatManager::ChatManager(DatabaseManager* dbManager, QObject* parent)
    : QObject(parent), m_dbManager(dbManager) // Ініціалізація членів класу
{
    // ініціалізуємо бот-відповіді
    botReplies = {
        QStringLiteral("Привіт! Класне фото!"), // Використовуємо QStringLiteral для і18n
        QStringLiteral("Як справи?"),
        QStringLiteral("Що робиш?"),
        QStringLiteral("Гарний день сьогодні, правда?"),
        QStringLiteral("Класно виглядаєш!")
    };
    // Q_UNUSED(dbManager); // Можна додати, якщо dbManager не використовується
}
// ----------------------------------------------------


void ChatManager::sendMessage(int fromUserId, int toUserId, const QString &message) {
    ChatMessage msg;
    msg.fromUserId = fromUserId;
    msg.toUserId = toUserId;
    msg.message = message;
    msg.timestamp = QDateTime::currentDateTime();

    // Ключ у QMap (пара користувачів)
    // Використовуємо унікальну функцію для створення ключа, щоб уникнути дублювання
    QPair<int,int> key = (fromUserId < toUserId)
                       ? QPair<int,int>(fromUserId, toUserId)
                       : QPair<int,int>(toUserId, fromUserId);

    // ПРИМІТКА: Якщо ви використовуєте лише один ключ, вам не потрібно сортувати в getMessages.
    // Але якщо ви зберігаєте key1 та key2, то логіка getMessages, яку ви написали, коректна.
    chatHistory[key].append(msg);
    qDebug() << "Message sent and stored.";
}

QList<ChatMessage> ChatManager::getMessages(int userId, int targetId) const {
    QList<ChatMessage> result;

    QPair<int,int> key1(userId, targetId);
    QPair<int,int> key2(targetId, userId);

    if (chatHistory.contains(key1)) {
        result.append(chatHistory.value(key1)); // Використовуємо .value() для const
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