#ifndef PROFILEPAGEWIDGET_H
#define PROFILEPAGEWIDGET_H

#include <QWidget>
#include "DatabaseManager.h"

class QLineEdit;
class QSpinBox;
class QTextEdit;
class QPushButton;
class QComboBox;

class ProfilePageWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProfilePageWidget(QWidget *parent = nullptr);

    void setDatabaseManager(DatabaseManager* dbManager);
    void loadCurrentProfile();
    void setInternalProfile(const UserProfile& profile);

    signals:
        void profileSaved();

    private slots:
        void on_btn_SaveProfile_clicked();

private:
    DatabaseManager* m_dbManager;
    UserProfile m_currentUser;
    QLineEdit* m_nameEdit;
    QSpinBox* m_ageSpinBox;
    QLineEdit* m_cityEdit;
    QTextEdit* m_bioEdit;
    QLineEdit* m_phoneEdit;
    QLineEdit* m_emailEdit;
    QComboBox* m_genderCombo;
    QComboBox* m_orientationCombo;
    QPushButton* m_saveButton;
};

#endif // PROFILEPAGEWIDGET_H