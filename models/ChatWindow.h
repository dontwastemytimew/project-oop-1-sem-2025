#pragma once
#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include "ChatManager.h"

class ChatWindow : public QWidget {
    Q_OBJECT
public:
    ChatWindow(int userId, int targetId, ChatManager *chatMgr, QWidget *parent = nullptr);

private slots:
    void sendMessage();

private:
    int m_userId;
    int m_targetId;
    ChatManager *m_chatMgr;

    QListWidget *messageList;
    QLineEdit *inputField;
    QPushButton *sendButton;

    void refreshMessages();
};
