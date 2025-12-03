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

SettingsPageWidget::SettingsPageWidget(QWidget *parent)
    // Встановлюємо m_isDarkTheme = true за замовчуванням
    // *АБО* зчитуємо збережені налаштування
    : QWidget(parent), m_mainWindow(nullptr)
{
    // Зчитуємо тему зі збережених налаштувань (для персистентності)
    QSettings settings("DatingAgency", "Match++");
    m_isDarkTheme = settings.value("isDarkTheme", true).toBool(); // true - темна тема за замовчуванням

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QFormLayout* formLayout = new QFormLayout();

    m_langLabel = new QLabel(this);
    m_langComboBox = new QComboBox(this);
    m_langComboBox->addItem("Українська", "ua");
    m_langComboBox->addItem("English", "en");
    m_langComboBox->setObjectName("langComboBox");

    formLayout->addRow(m_langLabel, m_langComboBox);

    m_themeLabel = new QLabel(this);
    m_themeToggle = new QPushButton(this);
    m_themeToggle->setCheckable(true);
    m_themeToggle->setObjectName("themeToggleButton");
    m_themeToggle->setFixedWidth(50);

    // Встановлюємо початковий стан кнопки
    m_themeToggle->setChecked(m_isDarkTheme);

    formLayout->addRow(m_themeLabel, m_themeToggle);
    updateThemeIcon(m_isDarkTheme);

    m_accountLabel = new QLabel(this);
    m_pauseToggle = new QCheckBox(this);
    m_pauseToggle->setObjectName("pauseToggleSwitch");
    formLayout->addRow(m_accountLabel, m_pauseToggle);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();

    m_deleteButton = new QPushButton(this);
    m_deleteButton->setObjectName("deleteAccountButton");
    mainLayout->addWidget(m_deleteButton);

    m_btnOpenAdmin = new QPushButton(this);
    m_btnOpenAdmin->setObjectName("settingsAdminBtn");

    mainLayout->addStretch();
    mainLayout->addWidget(m_btnOpenAdmin);

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

    // Якщо головне вікно встановлено, застосовуємо тему, збережену в налаштуваннях
    if (m_mainWindow) {
        m_mainWindow->switchTheme(m_isDarkTheme);
    }
}

void SettingsPageWidget::setDatabaseManager(DatabaseManager* dbManager) {
    m_dbManager = dbManager;
}

void SettingsPageWidget::loadCurrentSettings(const UserProfile& profile) {
    m_currentProfileId = profile.getId();
    // Приклад: якщо ви зберігаєте налаштування теми в QSettings, а не в профілі
}

void SettingsPageWidget::on_languageChanged(int index) {
    if (!m_mainWindow) return;
    QString langCode = m_langComboBox->itemData(index).toString();

    QSettings settings("DatingAgency", "Match++");
    settings.setValue("language", langCode);

    m_mainWindow->switchLanguage(langCode);

    retranslateUi();

    UserLogger::log(Info, "Language saved and switched to: " + langCode);
}

void SettingsPageWidget::on_themeToggled() {
    if (!m_mainWindow) return;
    m_isDarkTheme = m_themeToggle->isChecked();
    m_mainWindow->switchTheme(m_isDarkTheme);
    updateThemeIcon(m_isDarkTheme);

    // Зберігання налаштування теми
    QSettings settings("DatingAgency", "Match++");
    settings.setValue("isDarkTheme", m_isDarkTheme);

    // ВИПРАВЛЕННЯ: Використовуємо QString::arg для конкатенації
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
            accountDeleted();
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