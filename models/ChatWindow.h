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
    // --- ФІКС #1: Додано параметр currentUserId ---
    explicit ChatWindow(const UserProfile& matchProfile, ChatManager* chatManager, int currentUserId, QWidget *parent = nullptr);

    private slots:
        void sendMessage();
    void botReply();

private:
    // --- ФІКС #2: Додано оголошення методу loadHistory ---
    void loadHistory();

private:
    UserProfile m_matchProfile;
    ChatManager* m_chatManager;

    QTextEdit* m_chatArea;
    QLineEdit* m_inputField;
    QPushButton* m_sendButton;
    int m_currentUserId;
};

#endif // CHATWINDOW_H