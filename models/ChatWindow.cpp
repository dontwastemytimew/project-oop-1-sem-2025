#include "ChatWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

ChatWindow::ChatWindow(int userId, int targetId, ChatManager *chatMgr, QWidget *parent)
    : QWidget(parent), m_userId(userId), m_targetId(targetId), m_chatMgr(chatMgr) {

    messageList = new QListWidget(this);
    inputField = new QLineEdit(this);
    sendButton = new QPushButton("Відправити", this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(messageList);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(inputField);
    inputLayout->addWidget(sendButton);
    mainLayout->addLayout(inputLayout);

    connect(sendButton, &QPushButton::clicked, this, &ChatWindow::sendMessage);

    refreshMessages();
}

void ChatWindow::sendMessage() {
    QString text = inputField->text().trimmed();
    if (text.isEmpty()) return;

    // Надіслати повідомлення користувача
    m_chatMgr->sendMessage(m_userId, m_targetId, text);

    // Відповідь бота
    QString botReply = m_chatMgr->getBotReply();
    m_chatMgr->sendMessage(m_targetId, m_userId, botReply);

    inputField->clear();
    refreshMessages();
}

void ChatWindow::refreshMessages() {
    messageList->clear();
    QList<ChatMessage> messages = m_chatMgr->getMessages(m_userId, m_targetId);

    for (const ChatMessage &msg : messages) {
        QString display = QString("%1: %2")
                          .arg(msg.fromUserId == m_userId ? "Ви" : "Користувач")
                          .arg(msg.message);
        messageList->addItem(display);
    }
}
