#include "ChatPageWidget.h"
#include <QHBoxLayout>
#include <QTimer>
#include <QScrollBar>
#include <QLabel>


ChatPageWidget::ChatPageWidget(const UserProfile& matchProfile, ChatManager* chatManager, int currentUserId, QWidget *parent)
    : QWidget(parent),
    m_matchProfile(matchProfile),
    m_chatManager(chatManager),
    m_currentUserId(currentUserId)
{
    setWindowTitle(tr("Чат з %1").arg(m_matchProfile.getName()));

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QLabel* titleLabel = new QLabel(tr("Чат"), this);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #9932CC; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel, 0, Qt::AlignLeft);

    m_chatArea = new QTextEdit(this);
    m_chatArea->setReadOnly(true);
    m_chatArea->setFrameShape(QFrame::NoFrame);
    m_chatArea->setStyleSheet("background-color: #F8F8F8; color: #333333; font-size: 15px;");
    m_chatArea->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_inputField = new QLineEdit(this);
    m_inputField->setStyleSheet("font-size: 15px;");
    m_sendButton = new QPushButton(tr("Відправити"), this);

    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->addWidget(m_inputField);
    inputLayout->addWidget(m_sendButton);

    mainLayout->addWidget(m_chatArea);
    mainLayout->addLayout(inputLayout);

    connect(m_sendButton, &QPushButton::clicked, this, &ChatPageWidget::sendMessage);
    connect(m_inputField, &QLineEdit::returnPressed, this, &ChatPageWidget::sendMessage);

    loadHistory();
}

void ChatPageWidget::setMatchProfile(const UserProfile& matchProfile, int currentUserId) {

    m_matchProfile = matchProfile;
    m_currentUserId = currentUserId;

    setWindowTitle(tr("Чат з %1").arg(m_matchProfile.getName()));
    loadHistory();
}

QString formatMessage(const ChatMessage& msg, int currentUserId, const QString& matchName)
{
    bool isSenderMe = (msg.fromUserId == currentUserId);

    QString borderColor = isSenderMe ? "#9932CC" : "#E0E0E0";
    QString textColor = "#333333";
    QString nameColor = "#9932CC";

    QString html = "<table width='100%' border='0' cellpadding='0' cellspacing='0'><tr>";

    if (isSenderMe) {
        html += "<td width='30%'></td>";
        html += "<td align='right'>";
        html += QString("<table border='0' cellspacing='0' cellpadding='10' style='border: 2px solid %1; background-color: #ffffff;'>")
                    .arg(borderColor);
        html += "<tr><td>";
        html += QString("<span style='font-size: 16px; color: %1;'>%2</span>")
                    .arg(textColor, msg.message);

        html += "</td></tr></table>";
        html += "</td>";

    } else {
        html += "<td align='left'>";
        html += QString("<table border='0' cellspacing='0' cellpadding='10' style='border: 2px solid %1; background-color: #ffffff;'>")
                    .arg(borderColor);
        html += "<tr><td>";
        html += QString("<b style='font-size: 16px; color: %1'>%2:</b> ")
                    .arg(nameColor, matchName);
        html += QString("<span style='font-size: 16px; color: %1;'>%2</span>")
                     .arg(textColor, msg.message);

        html += "</td></tr></table>";
        html += "</td>";
        html += "<td width='30%'></td>";
    }

    html += "</tr></table>";
    html += "<div style='height: 5px;'></div>";

    return html;
}


void ChatPageWidget::loadHistory()
{
    if (!m_chatManager) return;

    QList<ChatMessage> history = m_chatManager->getMessages(m_currentUserId, m_matchProfile.getId());

    m_chatArea->clear();
    for (const auto& msg : history) {
        m_chatArea->append(formatMessage(msg, m_currentUserId, m_matchProfile.getName()));
    }

    m_chatArea->verticalScrollBar()->setValue(m_chatArea->verticalScrollBar()->maximum());
}


void ChatPageWidget::sendMessage()
{
    QString message = m_inputField->text().trimmed();
    if (message.isEmpty()) return;

    if (m_chatManager)
        m_chatManager->sendMessage(m_currentUserId, m_matchProfile.getId(), message);

    ChatMessage myMsg;
    myMsg.fromUserId = m_currentUserId;
    myMsg.message = message;
    m_chatArea->append(formatMessage(myMsg, m_currentUserId, m_matchProfile.getName()));

    m_inputField->clear();

    QTimer::singleShot(1000, this, &ChatPageWidget::botReply);
}

void ChatPageWidget::botReply()
{
    if (!m_chatManager) return;

    QString botMessage = m_chatManager->getBotReply();

    if (m_chatManager)
        m_chatManager->sendMessage(m_matchProfile.getId(), m_currentUserId, botMessage);

    ChatMessage botMsg;
    botMsg.fromUserId = m_matchProfile.getId();
    botMsg.message = botMessage;
    m_chatArea->append(formatMessage(botMsg, m_currentUserId, m_matchProfile.getName()));

    m_chatArea->verticalScrollBar()->setValue(m_chatArea->verticalScrollBar()->maximum());
}
