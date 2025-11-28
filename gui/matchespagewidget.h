#ifndef MATCHESPAGEWIDGET_H
#define MATCHESPAGEWIDGET_H

#include <QWidget>
#include <QVector>
#include <QListWidget>
#include "UserProfile.h"
#include "DatabaseManager.h"

namespace Ui {
class MatchesPageWidget;
}

class MatchesPageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MatchesPageWidget(QWidget *parent = nullptr);
    ~MatchesPageWidget();

    void setDatabaseManager(DatabaseManager* db);
    void setCurrentUserId(int id);

public slots:
    void reloadMatches();            //  оновити метчі
    void onMatchCreated(int userId, int targetId);  //  викликається зі SearchPage

private:
    Ui::MatchesPageWidget *ui;

    DatabaseManager* m_db = nullptr;
    int m_currentUserId = -1;

    QListWidget* m_list;  // список метчів
};

#endif // MATCHESPAGEWIDGET_H
