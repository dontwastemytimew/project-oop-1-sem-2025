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
#include <QSettings>
#include <QEvent>

SettingsPageWidget::SettingsPageWidget(QWidget *parent)
    : QWidget(parent), m_mainWindow(nullptr)
{
    QSettings settings("DatingAgency", "Match++");
    m_isDarkTheme = settings.value("isDarkTheme", true).toBool();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(10);

    QLabel* header = new QLabel("class <span style='color:#9932CC'>Settings</span> {", this);
    QFont headerFont("Consolas", 22, QFont::Bold);
    headerFont.setStyleHint(QFont::Monospace);
    header->setFont(headerFont);
    header->setTextFormat(Qt::RichText);
    mainLayout->addWidget(header);

    QLabel* publicLabel = new QLabel("public:", this);
    publicLabel->setStyleSheet("font-family: 'Consolas'; font-size: 16px; font-weight: bold; color: #9932CC; margin-top: 5px;");
    mainLayout->addWidget(publicLabel);

    QWidget* contentWidget = new QWidget(this);
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(30, 10, 0, 10);
    contentLayout->setSpacing(20);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_langLabel = new QLabel(this);
    m_langLabel->setStyleSheet("font-size: 14px; font-weight: bold;");

    m_langComboBox = new QComboBox(this);
    m_langComboBox->addItem("Українська", "ua");
    m_langComboBox->addItem("English", "en");
    m_langComboBox->setObjectName("langComboBox");
    m_langComboBox->setMinimumHeight(35);

    formLayout->addRow(m_langLabel, m_langComboBox);

    m_themeLabel = new QLabel(this);
    m_themeLabel->setStyleSheet("font-size: 14px; font-weight: bold;");

    m_themeToggle = new QPushButton(this);
    m_themeToggle->setCheckable(true);
    m_themeToggle->setObjectName("themeToggleButton");
    m_themeToggle->setFixedSize(50, 50);
    m_themeToggle->setIconSize(QSize(32, 32));

    m_themeToggle->setChecked(m_isDarkTheme);
    updateThemeIcon(m_isDarkTheme);

    formLayout->addRow(m_themeLabel, m_themeToggle);

    m_accountLabel = new QLabel(this);
    m_accountLabel->setStyleSheet("font-size: 14px; font-weight: bold;");

    m_pauseToggle = new QCheckBox(this);
    m_pauseToggle->setObjectName("pauseToggleSwitch");
    m_pauseToggle->setMinimumHeight(30);

    formLayout->addRow(m_accountLabel, m_pauseToggle);

    contentLayout->addLayout(formLayout);
    contentLayout->addSpacing(20);

    m_deleteButton = new QPushButton(this);
    m_deleteButton->setObjectName("deleteAccountButton");
    m_deleteButton->setMinimumHeight(45);
    m_deleteButton->setCursor(Qt::PointingHandCursor);
    m_deleteButton->setStyleSheet("font-weight: bold; font-size: 14px; border-radius: 5px;");
    contentLayout->addWidget(m_deleteButton);

    m_btnOpenAdmin = new QPushButton(this);
    m_btnOpenAdmin->setObjectName("settingsAdminBtn");
    m_btnOpenAdmin->setMinimumHeight(45);
    m_btnOpenAdmin->setCursor(Qt::PointingHandCursor);
    m_btnOpenAdmin->setStyleSheet("font-weight: bold; font-size: 14px; border-radius: 5px;");
    contentLayout->addWidget(m_btnOpenAdmin);

    mainLayout->addWidget(contentWidget);
    mainLayout->addStretch();

    QLabel* footer = new QLabel("};", this);
    footer->setFont(headerFont);
    mainLayout->addWidget(footer);

    setLayout(mainLayout);

    retranslateUi();

    connect(m_btnOpenAdmin, &QPushButton::clicked, this, &SettingsPageWidget::onAdminBtnClicked);
    connect(m_langComboBox, &QComboBox::currentIndexChanged, this, &SettingsPageWidget::on_languageChanged);
    connect(m_themeToggle, &QPushButton::clicked, this, &SettingsPageWidget::on_themeToggled);
    connect(m_pauseToggle, &QCheckBox::toggled, this, &SettingsPageWidget::on_pauseToggled);
    connect(m_deleteButton, &QPushButton::clicked, this, &SettingsPageWidget::on_deleteClicked);
}

void SettingsPageWidget::retranslateUi() {
    m_langLabel->setText(tr("Мова (Language):"));
    m_themeLabel->setText(tr("Тема:"));
    m_accountLabel->setText(tr("Акаунт:"));

    m_pauseToggle->setText(tr("Призупинити мій профіль (приховати з пошуку)"));
    m_deleteButton->setText(tr("ВИДАЛИТИ МІЙ АКАУНТ"));
    m_btnOpenAdmin->setText(tr("Панель Адміністратора"));

    m_langComboBox->setItemText(0, tr("Українська"));
    m_langComboBox->setItemText(1, tr("English"));
}

void SettingsPageWidget::setMainWindow(MainWindow* window) {
    m_mainWindow = window;

    if (m_mainWindow) {
        m_mainWindow->switchTheme(m_isDarkTheme);
    }
}

void SettingsPageWidget::setDatabaseManager(DatabaseManager* dbManager) {
    m_dbManager = dbManager;
}

void SettingsPageWidget::loadCurrentSettings(const UserProfile& profile) {
    m_currentProfileId = profile.getId();
}

void SettingsPageWidget::on_languageChanged(int index) {
    if (!m_mainWindow) return;
    QString langCode = m_langComboBox->itemData(index).toString();

    QSettings settings("DatingAgency", "Match++");
    settings.setValue("language", langCode);

    m_mainWindow->switchLanguage(langCode);
}

void SettingsPageWidget::on_themeToggled() {
    if (!m_mainWindow) return;
    m_isDarkTheme = m_themeToggle->isChecked();
    m_mainWindow->switchTheme(m_isDarkTheme);
    updateThemeIcon(m_isDarkTheme);

    QSettings settings("DatingAgency", "Match++");
    settings.setValue("isDarkTheme", m_isDarkTheme);

    UserLogger::log(Info, QString("Theme toggled. New state: %1").arg(m_isDarkTheme ? "Dark" : "Light"));
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
            QSettings settings("DatingAgency", "Match++");
            settings.remove("current_user_id");
            emit accountDeleted();
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

void SettingsPageWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}