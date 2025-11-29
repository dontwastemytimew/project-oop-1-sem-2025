#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include "UserProfile.h"
#include "ChatManager.h"

class ChatWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ChatWindow(const UserProfile& matchProfile, ChatManager* chatManager, QWidget *parent = nullptr);

private slots:
    void sendMessage();
    void botReply();

private:
    UserProfile m_matchProfile;
    ChatManager* m_chatManager;

    QTextEdit* m_chatArea;
    QLineEdit* m_inputField;
    QPushButton* m_sendButton;
};

#endif // CHATWINDOW_H
