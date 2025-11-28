#include "settingspagewidget.h"
#include "mainwindow.h"
#include "UserLogger.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QIcon>
#include <QMessageBox>
#include <QApplication>

SettingsPageWidget::SettingsPageWidget(QWidget *parent) 
    : QWidget(parent), m_mainWindow(nullptr), m_isDarkTheme(false)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QFormLayout* formLayout = new QFormLayout();
    
    m_langComboBox = new QComboBox(this);
    m_langComboBox->addItem(tr("Українська"), "ua"); // Текст, який бачить юзер + дані (код мови)
    m_langComboBox->addItem(tr("English"), "en");
    m_langComboBox->setObjectName("langComboBox");
    formLayout->addRow(tr("Мова (Language):"), m_langComboBox);

    m_themeToggle = new QPushButton(this);
    m_themeToggle->setCheckable(true);
    m_themeToggle->setObjectName("themeToggleButton");
    m_themeToggle->setFixedWidth(50);
    formLayout->addRow(tr("Тема:"), m_themeToggle);
    updateThemeIcon(m_isDarkTheme);

    m_pauseToggle = new QCheckBox(this);
    m_pauseToggle->setText(tr("Призупинити мій профіль (приховати з пошуку)"));
    m_pauseToggle->setObjectName("pauseToggleSwitch");
    formLayout->addRow(tr("Акаунт:"), m_pauseToggle);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();

    m_deleteButton = new QPushButton(tr("ВИДАЛИТИ МІЙ АКАУНТ"), this);
    m_deleteButton->setObjectName("deleteAccountButton");
    mainLayout->addWidget(m_deleteButton);

    setLayout(mainLayout);

    m_btnOpenAdmin = new QPushButton(tr("Панель Адміністратора"), this);
    m_btnOpenAdmin->setObjectName("settingsAdminBtn");

    mainLayout->addStretch();
    mainLayout->addWidget(m_btnOpenAdmin);

    connect(m_btnOpenAdmin, &QPushButton::clicked, this, &SettingsPageWidget::onAdminBtnClicked);

    connect(m_langComboBox, &QComboBox::currentIndexChanged, this, &SettingsPageWidget::on_languageChanged);
    connect(m_themeToggle, &QPushButton::clicked, this, &SettingsPageWidget::on_themeToggled);
    connect(m_pauseToggle, &QCheckBox::toggled, this, &SettingsPageWidget::on_pauseToggled);
    connect(m_deleteButton, &QPushButton::clicked, this, &SettingsPageWidget::on_deleteClicked);
}

void SettingsPageWidget::setMainWindow(MainWindow* window) {
    m_mainWindow = window;
}

void SettingsPageWidget::setDatabaseManager(DatabaseManager* dbManager) {
    m_dbManager = dbManager;
}

// Ця функція заповнює налаштування при завантаженні
void SettingsPageWidget::loadCurrentSettings(const UserProfile& profile) {
    m_currentProfileId = profile.getId();
    // (Тут ми ще не маємо доступу до is_hidden, бо UserProfile не оновлений,
    // але ми можемо завантажити QCheckBox стан з БД)

    // TODO: Завантажити стан m_pauseToggle з БД
}

void SettingsPageWidget::on_languageChanged(int index) {
    if (!m_mainWindow) return;
    QString langCode = m_langComboBox->itemData(index).toString();
    m_mainWindow->switchLanguage(langCode);
    UserLogger::log(Info, "Language selection changed.");
}

void SettingsPageWidget::on_themeToggled() {
    if (!m_mainWindow) return;

    m_isDarkTheme = !m_isDarkTheme;
    m_mainWindow->switchTheme(m_isDarkTheme);
    updateThemeIcon(m_isDarkTheme);
    UserLogger::log(Info, "Theme toggled.");
}

void SettingsPageWidget::updateThemeIcon(bool isDark) {
    if (isDark) {
        m_themeToggle->setIcon(QIcon(":/resources/icons/sun.png"));
    } else {
        m_themeToggle->setIcon(QIcon(":/resources/icons/moon.png"));
    }
}

void SettingsPageWidget::on_pauseToggled(bool checked) {
    if (!m_dbManager || m_currentProfileId == -1) return;

    m_dbManager->setProfileHidden(m_currentProfileId, checked);
    UserLogger::log(Info, QString("Pause Account toggled to: %1").arg(checked));
}

void SettingsPageWidget::on_deleteClicked() {
    if (!m_dbManager || m_currentProfileId == -1) return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, tr("Видалити акаунт"),
                                 tr("ВИ ВПЕВНЕНІ?\nВся інформація (профіль, матчі, лайки) буде видалена НАЗАВЖДИ!"),
                                 QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (m_dbManager->deleteProfile(m_currentProfileId)) {
            UserLogger::log(Info, "Profile deleted successfully.");
            QMessageBox::information(this, tr("Успіх"), tr("Ваш акаунт видалено."));

            // TODO: Потрібно "розлогінити" юзера
            // (напр., закрити додаток або перекинути на сторінку "Створити Профіль")
            qApp->quit(); // Найпростіший варіант - закрити додаток
        } else {
            UserLogger::log(Error, "Failed to delete profile from DB.");
        }
    } else {
        UserLogger::log(Info, "User cancelled account deletion.");
    }
}

void SettingsPageWidget::onAdminBtnClicked() {
    UserLogger::log(Info, "User requested Admin Panel from Settings.");
    emit openAdminPanelRequested();
}