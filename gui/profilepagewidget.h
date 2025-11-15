#ifndef PROFILEPAGEWIDGET_H
#define PROFILEPAGEWIDGET_H

#include <QWidget>
#include "DatabaseManager.h"

class QLineEdit;
class QSpinBox;
class QTextEdit;
class QPushButton;

class ProfilePageWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProfilePageWidget(QWidget *parent = nullptr);

    void setDatabaseManager(DatabaseManager* dbManager);

    signals:
    void profileSaved();

    private slots:
        void on_btn_SaveProfile_clicked();

private:
    DatabaseManager* m_dbManager;

    QLineEdit* m_nameEdit;
    QSpinBox* m_ageSpinBox;
    QTextEdit* m_bioEdit;
    QLineEdit* m_emailEdit;
    QLineEdit* m_phoneEdit;
    QPushButton* m_saveButton;
};
#endif // PROFILEPAGEWIDGET_H