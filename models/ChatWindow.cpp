#include "ChatWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QScrollBar>
#include <QRandomGenerator>
#include <QDebug>

ChatWindow::ChatWindow(const UserProfile& matchProfile, ChatManager* chatManager, int currentUserId, QWidget *parent)
    : QDialog(parent), m_matchProfile(matchProfile), m_chatManager(chatManager), m_currentUserId(currentUserId)
{
    setWindowTitle(tr("Чат з %1").arg(m_matchProfile.getName()));
    resize(400, 500);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    m_chatArea = new QTextEdit(this);
    m_chatArea->setReadOnly(true);

    m_inputField = new QLineEdit(this);
    m_sendButton = new QPushButton(tr("Відправити"), this);

    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->addWidget(m_inputField);
    inputLayout->addWidget(m_sendButton);

    mainLayout->addWidget(m_chatArea);
    mainLayout->addLayout(inputLayout);

    connect(m_sendButton, &QPushButton::clicked, this, &ChatWindow::sendMessage);
    connect(m_inputField, &QLineEdit::returnPressed, this, &ChatWindow::sendMessage);

    loadHistory();
}

void ChatWindow::loadHistory()
{
    if (!m_chatManager) return;

    // 1. Отримуємо історію
    QList<ChatMessage> history = m_chatManager->getMessages(m_currentUserId, m_matchProfile.getId());

    // 2. Очищуємо та заповнюємо поле
    m_chatArea->clear();
    for (const auto& msg : history) {
        QString senderName = (msg.fromUserId == m_currentUserId)
                            ? tr("Ви")
                            : m_matchProfile.getName();

        QString html = QString("<b>%1:</b> %2").arg(senderName).arg(msg.message);
        m_chatArea->append(html);
    }

    m_chatArea->verticalScrollBar()->setValue(m_chatArea->verticalScrollBar()->maximum());
}


void ChatWindow::sendMessage()
{
    QString message = m_inputField->text().trimmed();
    if (message.isEmpty()) return;

    // 1. Додаємо своє повідомлення
    m_chatArea->append(QString("<b>%1:</b> %2").arg(tr("Ви")).arg(message));
    m_inputField->clear();

    // 2. Надсилаємо у ChatManager
    if (m_chatManager)
        m_chatManager->sendMessage(m_currentUserId, m_matchProfile.getId(), message);

    // 3. Затримка бот-відповіді
    QTimer::singleShot(1000, this, &ChatWindow::botReply);
}

void ChatWindow::botReply()
{
    if (!m_chatManager) return;

    QString botMessage = m_chatManager->getBotReply();

    m_chatArea->append(QString("<b>%1:</b> %2").arg(m_matchProfile.getName()).arg(botMessage));

    m_chatArea->verticalScrollBar()->setValue(m_chatArea->verticalScrollBar()->maximum());
    if (m_chatManager)
        m_chatManager->sendMessage(m_matchProfile.getId(), m_currentUserId, botMessage);
}
