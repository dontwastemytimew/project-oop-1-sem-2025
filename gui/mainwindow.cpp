#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "UserLogger.h"
#include "searchpagewidget.h"
#include "profilepagewidget.h"
#include "matchespagewidget.h"
#include "settingspagewidget.h"
#include "welcomepagewidget.h"
#include "UserProfile.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>

MainWindow::MainWindow(DatabaseManager* dbManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dbManager(dbManager) {

    ui->setupUi(this);
    setWindowTitle(tr("Match++"));

    // Ініціалізація сторінок
    m_welcomePage = new WelcomePageWidget(this);
    m_searchPage = new SearchPageWidget(this);
    m_profilePage = new ProfilePageWidget(this);
    m_matchesPage = new MatchesPageWidget(this);
    m_settingsPage = new SettingsPageWidget(this);
    m_adminPage = new AdminPageWidget(this);
    m_chatManager = new ChatManager(m_dbManager, this);


    // Додавання до StackedWidget
    ui->stackedWidget_Pages->addWidget(m_welcomePage);
    ui->stackedWidget_Pages->addWidget(m_searchPage);
    ui->stackedWidget_Pages->addWidget(m_profilePage);
    ui->stackedWidget_Pages->addWidget(m_matchesPage);
    ui->stackedWidget_Pages->addWidget(m_settingsPage);
    ui->stackedWidget_Pages->addWidget(m_adminPage);
    m_matchesPage->setChatManager(m_chatManager);

    // Налаштування тем та мови
    switchTheme(false);
    switchLanguage("ua");

    QLayout* navLayout = ui->nav_menu->layout();
    if (navLayout) {
        navLayout->setContentsMargins(0, 0, 0, 0);
    }

    // ПЕРЕВІРКА СЕСІЇ ТА ЗАВАНТАЖЕННЯ ДАНИХ
    UserProfile currentUser;
    m_userExists = m_dbManager->getCurrentUserProfile(currentUser);

    m_profilePage->setDatabaseManager(m_dbManager);
    m_settingsPage->setDatabaseManager(m_dbManager);
    m_adminPage->setDatabaseManager(m_dbManager);

    m_settingsPage->setMainWindow(this);

    if (m_userExists) {
        m_profilePage->setInternalProfile(currentUser);
        m_settingsPage->loadCurrentSettings(currentUser);

        // ОНОВЛЕННЯ СИНХРОНІЗАЦІЇ
        m_searchPage->setDatabaseManager(m_dbManager);
        m_searchPage->setCurrentUser(currentUser);
        m_matchesPage->setDatabaseManager(m_dbManager);
        m_matchesPage->setCurrentUserId(currentUser.getId());

        UserLogger::log(Info, QString("User profile (ID=%1) loaded and synchronized.")
                                  .arg(currentUser.getId()));
    } else {
        m_searchPage->setDatabaseManager(m_dbManager);
        UserLogger::log(Info, "No user found. App running in 'guest' mode.");
    }

    // Початковий екран
    ui->stackedWidget_Pages->setCurrentWidget(m_welcomePage);
    UserLogger::log(Info, "Showing Welcome Page.");

    connect(m_profilePage, &ProfilePageWidget::profileSaved,
            this, &MainWindow::onProfileSaved);

    connect(m_adminPage, &AdminPageWidget::backClicked, this, [this](){
        ui->stackedWidget_Pages->setCurrentWidget(m_settingsPage);
    });

    connect(m_settingsPage, &SettingsPageWidget::openAdminPanelRequested,
            this, &MainWindow::showAdminPage);

    // ЗВ'ЯЗОК МІЖ ПОШУКОМ І МЕТЧАМИ
    connect(m_searchPage, &SearchPageWidget::matchFound,
            m_matchesPage, &MatchesPageWidget::onMatchCreated);

    connect(m_settingsPage, &SettingsPageWidget::accountDeleted,
            this, &MainWindow::onAccountDeleted);
}

MainWindow::~MainWindow()
{
    delete ui;
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

    QString appDir = QCoreApplication::applicationDirPath();

    QString path = appDir + "/translations/app_" + languageCode + ".qm";

    UserLogger::log(Info, "Looking for translation at: " + path);

    if (m_translator.load(path)) {
        qApp->installTranslator(&m_translator);
        UserLogger::log(Info, "Language switched to: " + languageCode);
    } else {
        UserLogger::log(Warning, "Failed to load translation file. Path: " + path);
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
    UserProfile currentUser;

    if (m_dbManager->getCurrentUserProfile(currentUser)) {

        m_userExists = true;

        m_searchPage->setCurrentUser(currentUser);

        m_matchesPage->setDatabaseManager(m_dbManager);
        m_matchesPage->setCurrentUserId(currentUser.getId());

        m_settingsPage->loadCurrentSettings(currentUser);

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
