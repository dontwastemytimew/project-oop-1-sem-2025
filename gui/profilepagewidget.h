#ifndef PROFILEPAGEWIDGET_H
#define PROFILEPAGEWIDGET_H

#include <QWidget>
#include "DatabaseManager.h"
#include "UserProfile.h"

namespace Ui {
class ProfilePageWidget;
}

class ProfilePageWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProfilePageWidget(QWidget *parent = nullptr);
    ~ProfilePageWidget();

    void setDatabaseManager(DatabaseManager* dbManager);
    void loadCurrentProfile();
    void setInternalProfile(const UserProfile& profile);

signals:
    void profileSaved();

private slots:
    void on_btn_SaveProfile_clicked();
    void onChoosePhoto();

private:
    Ui::ProfilePageWidget *ui;
    DatabaseManager* m_dbManager;
    UserProfile m_currentUser;
    QString m_photoPath;
};

#endif // PROFILEPAGEWIDGET_H
