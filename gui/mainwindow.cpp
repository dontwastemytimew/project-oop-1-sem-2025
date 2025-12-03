#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "UserLogger.h"
#include "searchpagewidget.h"
#include "profilepagewidget.h"
#include "matchespagewidget.h"
#include "settingspagewidget.h"
#include "welcomepagewidget.h"
#include "ChatPageWidget.h"
#include "UserProfile.h"
#include "FakeDataManager.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>
#include <QEvent>
#include <QSettings>
#include <QIcon>

MainWindow::MainWindow(DatabaseManager* dbManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dbManager(dbManager) {

    ui->setupUi(this);
    setWindowTitle(tr("Match++"));

    QIcon appIcon(":/resources/icons/logo.jpg");
    this->setWindowIcon(appIcon);

    m_welcomePage = new WelcomePageWidget(this);
    m_searchPage = new SearchPageWidget(this);
    m_profilePage = new ProfilePageWidget(this);
    m_matchesPage = new MatchesPageWidget(this);
    m_settingsPage = new SettingsPageWidget(this);
    m_adminPage = new AdminPageWidget(this);
    m_chatManager = new ChatManager(m_dbManager, this);
    m_chatPage = new ChatPageWidget(UserProfile(), m_chatManager, -1, this);

    // Додавання до StackedWidget
    ui->stackedWidget_Pages->addWidget(m_welcomePage);
    ui->stackedWidget_Pages->addWidget(m_searchPage);
    ui->stackedWidget_Pages->addWidget(m_profilePage);
    ui->stackedWidget_Pages->addWidget(m_matchesPage);
    ui->stackedWidget_Pages->addWidget(m_settingsPage);
    ui->stackedWidget_Pages->addWidget(m_adminPage);
    ui->stackedWidget_Pages->addWidget(m_chatPage);
    m_matchesPage->setChatManager(m_chatManager);

    // Налаштування тем та мови
    QSettings settings("DatingAgency", "Match++");
    bool isDarkDefault = settings.value("isDarkTheme", true).toBool();
    switchTheme(isDarkDefault);
    switchLanguage("ua");

    QLayout* navLayout = ui->nav_menu->layout();
    if (navLayout) {
        navLayout->setContentsMargins(0, 0, 0, 0);
    }

    // ПЕРЕВІРКА СЕСІЇ ТА ЗАВАНТАЖЕННЯ ДАНИХ
    UserProfile currentUser;
    m_userExists = m_dbManager->getCurrentUserProfile(currentUser);
    m_currentProfile = currentUser;

    m_profilePage->setDatabaseManager(m_dbManager);
    m_settingsPage->setDatabaseManager(m_dbManager);
    m_adminPage->setDatabaseManager(m_dbManager);
    m_settingsPage->setMainWindow(this);

    if (m_userExists) {
        m_profilePage->setInternalProfile(m_currentProfile);
        m_settingsPage->loadCurrentSettings(m_currentProfile);

        // ОНОВЛЕННЯ СИНХРОНІЗАЦІЇ
        m_searchPage->setDatabaseManager(m_dbManager);
        m_searchPage->setCurrentUser(m_currentProfile);
        m_matchesPage->setDatabaseManager(m_dbManager);
        m_matchesPage->setCurrentUserId(m_currentProfile.getId());

        FakeDataManager::seedReverseLikes(m_dbManager, m_currentProfile.getId(), 50);

        UserLogger::log(Info, QString("User profile (ID=%1) loaded and synchronized.")
                                  .arg(m_currentProfile.getId()));
    } else {
        m_searchPage->setDatabaseManager(m_dbManager);
        UserLogger::log(Info, "No user found. App running in 'guest' mode.");
    }

    ui->stackedWidget_Pages->setCurrentWidget(m_welcomePage);
    UserLogger::log(Info, "Showing Welcome Page.");

    connect(m_profilePage, &ProfilePageWidget::profileSaved,
            this, &MainWindow::onProfileSaved);

    connect(m_adminPage, &AdminPageWidget::backClicked, this, [this](){
        ui->stackedWidget_Pages->setCurrentWidget(m_settingsPage);
    });

    connect(m_settingsPage, &SettingsPageWidget::openAdminPanelRequested,
            this, &MainWindow::showAdminPage);

    connect(m_settingsPage, &SettingsPageWidget::accountDeleted,
            this, &MainWindow::onAccountDeleted);

    connect(m_matchesPage, &MatchesPageWidget::openChatRequested,
            this, &MainWindow::onOpenChatRequested);

    connect(m_searchPage, &SearchPageWidget::matchFound,
            m_matchesPage, &MatchesPageWidget::onMatchCreated);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        setWindowTitle(tr("Match++"));

        ui->retranslateUi(this);
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::on_btn_Search_clicked()
{
    if (!m_userExists) {
        QMessageBox::warning(this, tr("Профіль не знайдено"),
                             tr("Будь ласка, спочатку створіть свій профіль на сторінці 'Профіль'."));
        return;
    }

    ui->stackedWidget_Pages->setCurrentWidget(m_searchPage);
    UserLogger::log(Info, "User switched to Search Page");
}

void MainWindow::on_btn_Profile_clicked()
{
    ui->stackedWidget_Pages->setCurrentWidget(m_profilePage);
    UserLogger::log(Info, "User switched to Profile Page");
}

void MainWindow::on_btn_Matches_clicked()
{
    if (!m_userExists) {
        QMessageBox::warning(this, tr("Профіль не знайдено"),
                             tr("У вас не може бути метчів, доки ви не створите профіль."));
        return;
    }

    // Перезавантажуємо список на випадок нових матчів
    m_matchesPage->reloadMatches();
    ui->stackedWidget_Pages->setCurrentWidget(m_matchesPage);
    UserLogger::log(Info, "User switched to Matches Page");
}

void MainWindow::on_btn_Settings_clicked()
{
    ui->stackedWidget_Pages->setCurrentWidget(m_settingsPage);
    UserLogger::log(Info, "User switched to Settings Page");
}

void MainWindow::switchLanguage(const QString& languageCode)
{
    qApp->removeTranslator(&m_translator);

    QString path = ":/translations/translations/app_" + languageCode + ".qm";

    UserLogger::log(Info, "Looking for translation at resource: " + path);

    if (m_translator.load(path)) {
        qApp->installTranslator(&m_translator);
        UserLogger::log(Info, "Language switched successfully to: " + languageCode);
    }
    else {
        QString altPath = ":/translations/app_" + languageCode + ".qm";
        UserLogger::log(Warning, "First path failed. Trying alternative: " + altPath);

        if (m_translator.load(altPath)) {
            qApp->installTranslator(&m_translator);
            UserLogger::log(Info, "Language switched using alternative path.");
        } else {
            UserLogger::log(Error, "CRITICAL: Failed to load translation file!");
        }
    }
}

void MainWindow::switchTheme(bool isDark)
{
    QString themePath = isDark ? ":/resources/dark_theme.qss" : ":/resources/light_theme.qss";

    QFile styleFile(themePath);
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QTextStream(&styleFile).readAll();
        qApp->setStyleSheet(styleSheet);
        styleFile.close();
        UserLogger::log(Info, QString("Theme switched to: %1").arg(isDark ? "Dark" : "Light"));
    } else {
        UserLogger::log(Warning, "Failed to load theme file: " + themePath);
    }

    m_settingsPage->updateThemeIcon(isDark);
}

void MainWindow::onProfileSaved()
{
    if (m_dbManager->getCurrentUserProfile(m_currentProfile)) {

        m_userExists = true;

        m_searchPage->setCurrentUser(m_currentProfile);
        m_matchesPage->setDatabaseManager(m_dbManager);
        m_matchesPage->setCurrentUserId(m_currentProfile.getId());
        m_settingsPage->loadCurrentSettings(m_currentProfile);
        m_profilePage->setInternalProfile(m_currentProfile);

        UserLogger::log(Info, "MainWindow received profileSaved signal. Session established.");
    }
}

void MainWindow::showAdminPage() {
    ui->stackedWidget_Pages->setCurrentWidget(m_adminPage);
    m_adminPage->refreshTable();
    UserLogger::log(Info, "Switched to Admin Panel.");
}

void MainWindow::onAccountDeleted() {
    m_userExists = false;

    ui->stackedWidget_Pages->setCurrentWidget(m_welcomePage);

    m_profilePage->clearFields();

    UserLogger::log(Info, "Account deleted. Switching to Welcome Page.");
}

void MainWindow::on_btn_Exit_clicked()
{
    UserLogger::log(Info, "User requested application exit.");

    qApp->quit();
}

void MainWindow::onOpenChatRequested(int targetUserId) {
    UserProfile targetProfile;

    if (m_dbManager->loadProfileById(targetUserId, targetProfile)) {

        m_chatPage->setMatchProfile(targetProfile, m_currentProfile.getId());

        ui->stackedWidget_Pages->setCurrentWidget(m_chatPage);
        UserLogger::log(Info, QString("Switched to Chat with ID %1.").arg(targetUserId));
    }
}