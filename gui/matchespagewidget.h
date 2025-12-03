#ifndef MATCHESPAGEWIDGET_H
#define MATCHESPAGEWIDGET_H

#include <QListWidget>
#include <QLabel>
#include "DatabaseManager.h"
#include "ChatManager.h"

class MatchesPageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MatchesPageWidget(QWidget *parent = nullptr);
    ~MatchesPageWidget();

    void setDatabaseManager(DatabaseManager* db);
    void setCurrentUserId(int id);
    void reloadMatches();
    void setChatManager(ChatManager* chatManager);

    public slots:
        void onMatchCreated(int userId, int targetId);

    signals:
        void openChatRequested(int targetUserId);

protected:
    void changeEvent(QEvent *event) override;

    private slots:
        void onMatchClicked(QListWidgetItem* item);
    void onSearchTextChanged(const QString &text);

private:
    void retranslateUi();

    int m_currentUserId = -1;
    DatabaseManager* m_db = nullptr;
    ChatManager* m_chatManager = nullptr;

    QLabel* m_titleLabel;
    QLineEdit* m_searchField;
    QListWidget* m_list;
};

#endif // MATCHESPAGEWIDGET_H