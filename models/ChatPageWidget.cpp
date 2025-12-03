#include "ChatPageWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QScrollBar>
#include <QLabel>
#include <QDateTime>

ChatPageWidget::ChatPageWidget(const UserProfile& matchProfile, ChatManager* chatManager, int currentUserId, QWidget *parent)
    : QWidget(parent),
    m_matchProfile(matchProfile),
    m_chatManager(chatManager),
    m_currentUserId(currentUserId)
{
    setWindowTitle(tr("Terminal - %1").arg(m_matchProfile.getName()));

    // ОСНОВНИЙ ФОН - ПРИБРАНО ХАРДКОД
    this->setObjectName("chatPageWidget");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- 1. ВЕРХНЯ ПАНЕЛЬ (ЗАГОЛОВОК) ---
    QWidget* headerPanel = new QWidget(this);
    headerPanel->setObjectName("chatHeaderPanel"); // Для фону та лінії
    QHBoxLayout* headerLayout = new QHBoxLayout(headerPanel);
    headerLayout->setContentsMargins(15, 10, 15, 10);

    QLabel* headerLabel = new QLabel(tr("TERMINAL: %1").arg(m_matchProfile.getName()), this);
    headerLabel->setObjectName("chatHeaderLabel"); // Для стилізації шрифту та кольору
    headerLayout->addWidget(headerLabel);
    headerLayout->addStretch();

    mainLayout->addWidget(headerPanel);

    // --- 2. ОБЛАСТЬ ЧАТУ ---
    m_chatArea = new QTextEdit(this);
    m_chatArea->setObjectName("chatArea"); // Для фону, шрифту та відступів
    m_chatArea->setReadOnly(true);
    m_chatArea->setFrameShape(QFrame::NoFrame);
    m_chatArea->setTextInteractionFlags(Qt::TextSelectableByMouse);

    mainLayout->addWidget(m_chatArea);

    // --- 3. ОБЛАСТЬ ВВОДУ ---
    QWidget* inputPanel = new QWidget(this);
    inputPanel->setObjectName("chatInputPanel"); // Для фону та лінії
    QHBoxLayout* inputLayout = new QHBoxLayout(inputPanel);
    inputLayout->setContentsMargins(10, 10, 10, 10);

    QLabel* promptLbl = new QLabel(">", this);
    promptLbl->setObjectName("chatPromptLabel"); // Для кольору промпта (>)
    inputLayout->addWidget(promptLbl);

    m_inputField = new QLineEdit(this);
    m_inputField->setObjectName("chatInputField"); // Для стилізації поля вводу
    m_inputField->setPlaceholderText("std::cin >> message;");

    // Кнопка ВІДПРАВИТИ
    m_sendButton = new QPushButton(tr("Send"), this);
    m_sendButton->setObjectName("btnSendChat"); // Для стилізації кнопки
    m_sendButton->setCursor(Qt::PointingHandCursor);

    inputLayout->addWidget(m_inputField);
    inputLayout->addWidget(m_sendButton);

    mainLayout->addWidget(inputPanel);

    connect(m_sendButton, &QPushButton::clicked, this, &ChatPageWidget::sendMessage);
    connect(m_inputField, &QLineEdit::returnPressed, this, &ChatPageWidget::sendMessage);

    loadHistory();
}

void ChatPageWidget::setMatchProfile(const UserProfile& matchProfile, int currentUserId) {
    m_matchProfile = matchProfile;
    m_currentUserId = currentUserId;

    m_chatArea->clear();
    m_inputField->clear();

    setWindowTitle(tr("Terminal - %1").arg(m_matchProfile.getName()));
    loadHistory();
}

void ChatPageWidget::sendMessage() {
    QString text = m_inputField->text().trimmed();
    if (text.isEmpty()) return;

    if (m_chatManager) {
        m_chatManager->sendMessage(m_currentUserId, m_matchProfile.getId(), text);
    }

    QString time = QDateTime::currentDateTime().toString("HH:mm");

    // HTML-теги залишаються для підсвічування СИНТАКСИСУ, але кольори винесені у константи.
    // Використовуємо класи для керування кольором тексту, які ви оголосите в QSS
    m_chatArea->append(
        QString(
            "<div style='margin-bottom: 4px;'>"
            "<span class='time_stamp'>[%1]</span> " // Час
            "<b class='user_me'>Me:</b> "          // Me
            "<span class='message_text'>%2</span>" // Текст
            "</div>"
        ).arg(time, text)
    );

    m_inputField->clear();

    QScrollBar *sb = m_chatArea->verticalScrollBar();
    sb->setValue(sb->maximum());

    QTimer::singleShot(1500, this, &ChatPageWidget::botReply);
}

void ChatPageWidget::botReply() {
    if (!m_chatManager) return;

    QString reply = m_chatManager->getBotReply();
    QString time = QDateTime::currentDateTime().toString("HH:mm");

    m_chatManager->sendMessage(m_matchProfile.getId(), m_currentUserId, reply);

    // Стиль відповіді:
    m_chatArea->append(
        QString(
            "<div style='margin-bottom: 4px;'>"
            "<span class='time_stamp'>[%1]</span> " // Час
            "<b class='user_target'>%2:</b> "     // Ім'я співрозмовника
            "<span class='message_text'>%3</span>"// Текст
            "</div>"
        ).arg(time, m_matchProfile.getName(), reply)
    );

    QScrollBar *sb = m_chatArea->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void ChatPageWidget::loadHistory() {
    if (!m_chatManager) return;

    QList<ChatMessage> messages = m_chatManager->getMessages(m_currentUserId, m_matchProfile.getId());

    for (const ChatMessage& msg : messages) {
        QString time = msg.timestamp.toString("HH:mm");

        if (msg.fromUserId == m_currentUserId) {
            // Моє повідомлення
            m_chatArea->append(
                QString(
                    "<div style='margin-bottom: 4px;'>"
                    "<span class='time_stamp'>[%1]</span> "
                    "<b class='user_me'>Me:</b> "
                    "<span class='message_text'>%2</span>"
                    "</div>"
                ).arg(time, msg.message)
            );
        } else {
            // Повідомлення співрозмовника
            m_chatArea->append(
                QString(
                    "<div style='margin-bottom: 4px;'>"
                    "<span class='time_stamp'>[%1]</span> "
                    "<b class='user_target'>%2:</b> "
                    "<span class='message_text'>%3</span>"
                    "</div>"
                ).arg(time, m_matchProfile.getName(), msg.message)
            );
        }
    }

    QScrollBar *sb = m_chatArea->verticalScrollBar();
    sb->setValue(sb->maximum());
}