#include "ChatManager.h"
#include <QtGlobal>
#include <QRandomGenerator>

ChatManager::ChatManager() {
    // ініціалізуємо бот-відповіді
    botReplies = {
        "Привіт! Класне фото!",
        "Як справи?",
        "Що робиш?",
        "Гарний день сьогодні, правда?",
        "Класно виглядаєш!"
    };
}

void ChatManager::sendMessage(int fromUserId, int toUserId, const QString &message) {
    ChatMessage msg;
    msg.fromUserId = fromUserId;
    msg.toUserId = toUserId;
    msg.message = message;
    msg.timestamp = QDateTime::currentDateTime();

    // ключ у QMap (пара користувачів)
    QPair<int,int> key(fromUserId, toUserId);
    chatHistory[key].append(msg);
}

QList<ChatMessage> ChatManager::getMessages(int userId, int targetId) const {
    QList<ChatMessage> result;

    QPair<int,int> key1(userId, targetId);
    QPair<int,int> key2(targetId, userId);

    if (chatHistory.contains(key1)) {
        result.append(chatHistory[key1]);
    }
    if (chatHistory.contains(key2)) {
        result.append(chatHistory[key2]);
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
