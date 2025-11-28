#ifndef SEARCHPAGEWIDGET_H
#define SEARCHPAGEWIDGET_H

#include <QWidget>
#include <QVector>
#include "DatabaseManager.h"
#include "UserProfile.h"
#include "profilecard.h"

namespace Ui {
class SearchPageWidget;
}

class SearchPageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchPageWidget(QWidget *parent = nullptr);
    ~SearchPageWidget();

    void setDatabaseManager(DatabaseManager* dbManager);
    void loadProfiles();

signals:
    void matchFound(int userId, int targetId);   // ★ новий сигнал (для 4.4)

private slots:
    void on_Like_clicked();
    void on_Skip_clicked();

private:
    Ui::SearchPageWidget *ui;
    DatabaseManager* m_dbManager = nullptr;

    QVector<UserProfile> m_profiles;
    int m_index = 0;

    UserProfile m_currentUser;

    void showCurrentProfile();
    void showMatchPopup(const UserProfile& target);
};

#endif // SEARCHPAGEWIDGET_H
