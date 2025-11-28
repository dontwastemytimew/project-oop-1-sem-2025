#ifndef MATCHESPAGEWIDGET_H
#define MATCHESPAGEWIDGET_H

#include <QWidget>
#include <QVector>
#include <QListWidget>
#include "UserProfile.h"
#include "DatabaseManager.h"
#include <QLabel>


class MatchesPageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MatchesPageWidget(QWidget *parent = nullptr);
    ~MatchesPageWidget();

    void setDatabaseManager(DatabaseManager* db);
    void setCurrentUserId(int id);

    public slots:
        void reloadMatches();
    void onMatchCreated(int userId, int targetId);

private:

    DatabaseManager* m_db = nullptr;
    int m_currentUserId = -1;
    QLabel* m_titleLabel;
    QListWidget* m_list;
};

#endif // MATCHESPAGEWIDGET_H