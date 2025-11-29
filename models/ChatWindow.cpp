#include "ChatWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QScrollBar>
#include <QTime>
#include <QRandomGenerator>

ChatWindow::ChatWindow(const UserProfile& matchProfile, ChatManager* chatManager, QWidget *parent)
    : QDialog(parent), m_matchProfile(matchProfile), m_chatManager(chatManager)
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

    // Підключення кнопки та Enter
    connect(m_sendButton, &QPushButton::clicked, this, &ChatWindow::sendMessage);
    connect(m_inputField, &QLineEdit::returnPressed, this, &ChatWindow::sendMessage);
}

void ChatWindow::sendMessage()
{
    QString message = m_inputField->text().trimmed();
    if (message.isEmpty()) return;

    // Додаємо своє повідомлення
    m_chatArea->append(QString("<b>Ви:</b> %1").arg(message));
    m_inputField->clear();
    m_chatArea->verticalScrollBar()->setValue(m_chatArea->verticalScrollBar()->maximum());

    // Надсилаємо у ChatManager
    if (m_chatManager)
        m_chatManager->sendMessage(0, m_matchProfile.getId(), message); // 0 — це тимчасово currentUserId, можна замінити

    // Затримка бот-відповіді
    QTimer::singleShot(1000, this, &ChatWindow::botReply);
}

void ChatWindow::botReply()
{
    QStringList replies = {"Привіт!", "Як справи?", "Цікаво!", "Розкажи більше...", "😊"};
    int idx = QRandomGenerator::global()->bounded(replies.size());

    m_chatArea->append(QString("<b>%1:</b> %2").arg(m_matchProfile.getName()).arg(replies[idx]));
    m_chatArea->verticalScrollBar()->setValue(m_chatArea->verticalScrollBar()->maximum());

    // Надсилаємо у ChatManager
    if (m_chatManager)
        m_chatManager->sendMessage(m_matchProfile.getId(), 0, replies[idx]); // 0 — currentUserId
}
