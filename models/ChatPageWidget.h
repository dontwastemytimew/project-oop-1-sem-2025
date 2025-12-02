#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include "UserProfile.h"
#include "ChatManager.h"
#include <QWidget>

class ChatPageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPageWidget(const UserProfile& matchProfile, ChatManager* chatManager, int currentUserId, QWidget *parent = nullptr);

    /**
      * @brief Оновлює профіль користувача та завантажує історію нового чату.
      */
    void setMatchProfile(const UserProfile& matchProfile, int currentUserId);

    private slots:
        void sendMessage();
    void botReply();

private:
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