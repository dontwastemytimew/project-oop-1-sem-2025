#include "profilepagewidget.h"
#include "UserLogger.h"
#include "UserProfile.h"
#include "ContactInfo.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>

ProfilePageWidget::ProfilePageWidget(QWidget *parent)
    : QWidget(parent), m_dbManager(nullptr)
{
    QFormLayout* formLayout = new QFormLayout();

    m_nameEdit = new QLineEdit(this);
    m_ageSpinBox = new QSpinBox(this);
    m_ageSpinBox->setRange(18, 99);
    m_bioEdit = new QTextEdit(this);
    m_emailEdit = new QLineEdit(this);
    m_phoneEdit = new QLineEdit(this);
    m_saveButton = new QPushButton(tr("Зберегти профіль"), this);

    formLayout->addRow(tr("Ім'я:"), m_nameEdit);
    formLayout->addRow(tr("Вік:"), m_ageSpinBox);
    formLayout->addRow(tr("Email:"), m_emailEdit);
    formLayout->addRow(tr("Телефон:"), m_phoneEdit);
    formLayout->addRow(tr("Про себе:"), m_bioEdit);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(m_saveButton);
    setLayout(mainLayout);

    connect(m_saveButton, &QPushButton::clicked, this, &ProfilePageWidget::on_btn_SaveProfile_clicked);
}

void ProfilePageWidget::setDatabaseManager(DatabaseManager* dbManager) {
    m_dbManager = dbManager;
}

void ProfilePageWidget::on_btn_SaveProfile_clicked() {
    UserLogger::log(Info, "Save Profile button clicked.");

    if (!m_dbManager) {
        UserLogger::log(Error, "DatabaseManager is not set for ProfilePage!");
        QMessageBox::critical(this, tr("Помилка"), tr("Помилка бази даних!"));
        return;
    }

    QString name = m_nameEdit->text();
    int age = m_ageSpinBox->value();
    QString bio = m_bioEdit->toPlainText();
    QString email = m_emailEdit->text();
    QString phone = m_phoneEdit->text();

    ContactInfo contacts(phone, email);
    UserProfile profile(-1, name, age, "", bio); // (City можна додати пізніше)
    profile.setContactInfo(contacts);

    if (m_dbManager->profileExists(email)) {
        UserLogger::log(Warning, "Profile already exists for: " + email);
        QMessageBox::warning(this, tr("Помилка"), tr("Профіль з таким email вже існує."));
        return;
    }

    if (m_dbManager->saveProfile(profile)) {
        UserLogger::log(Info, "Profile saved successfully for: " + name);
        QMessageBox::information(this, tr("Успіх"), tr("Профіль успішно збережено!"));
        emit profileSaved();
        m_nameEdit->clear();
        m_ageSpinBox->setValue(18);
        m_bioEdit->clear();
        m_emailEdit->clear();
        m_phoneEdit->clear();
    } else {
        UserLogger::log(Error, "Failed to save profile (check validation).");
        QMessageBox::critical(this, tr("Помилка"), tr("Не вдалося зберегти профіль. Перевірте дані (напр., вік > 18)."));
    }
}