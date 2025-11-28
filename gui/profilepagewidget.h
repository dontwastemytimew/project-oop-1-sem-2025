#ifndef PROFILEPAGEWIDGET_H
#define PROFILEPAGEWIDGET_H

#include <QWidget>
#include "DatabaseManager.h"
#include "UserProfile.h"

class QLineEdit;
class QSpinBox;
class QTextEdit;
class QPushButton;
class QComboBox;
class QLabel;

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
        void onChoosePhoto(); // <-- Слот для вибору фото

private:
    DatabaseManager* m_dbManager;
    UserProfile m_currentUser;

    QString m_photoPath;
    QLineEdit* m_nameEdit;
    QSpinBox* m_ageSpinBox;
    QLineEdit* m_cityEdit;
    QTextEdit* m_bioEdit;
    QLineEdit* m_phoneEdit;
    QLineEdit* m_emailEdit;
    QComboBox* m_genderCombo;
    QComboBox* m_orientationCombo;
    QLabel* m_photoLabel; 
    QPushButton* m_choosePhotoButton;
    QPushButton* m_saveButton;
    
    // --- ПРИВАТНІ МЕТОДИ ---
    void setupCityAutocomplete(); // <-- Метод для автодоповнення
};

#endif // PROFILEPAGEWIDGET_H