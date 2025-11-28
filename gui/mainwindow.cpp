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

MainWindow::MainWindow(DatabaseManager* dbManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dbManager(dbManager) {
    ui->setupUi(this);
    setWindowTitle(tr("project"));

    m_welcomePage = new WelcomePageWidget(this);
    m_searchPage = new SearchPageWidget(this);
    m_profilePage = new ProfilePageWidget(this);
    m_matchesPage = new MatchesPageWidget(this);
    m_settingsPage = new SettingsPageWidget(this);
    m_adminPage = new AdminPageWidget(this);

    m_profilePage->setDatabaseManager(m_dbManager);
    m_settingsPage->setMainWindow(this);
    m_settingsPage->setDatabaseManager(m_dbManager);
    m_searchPage->setDatabaseManager(m_dbManager);
    m_adminPage->setDatabaseManager(m_dbManager);

    ui->stackedWidget_Pages->addWidget(m_welcomePage);
    ui->stackedWidget_Pages->addWidget(m_searchPage);
    ui->stackedWidget_Pages->addWidget(m_profilePage);
    ui->stackedWidget_Pages->addWidget(m_matchesPage);
    ui->stackedWidget_Pages->addWidget(m_settingsPage);
    ui->stackedWidget_Pages->addWidget(m_adminPage);

    // Тема + мова
    switchTheme(false);
    switchLanguage("ua");

    QLayout* navLayout = ui->nav_menu->layout();
    if (navLayout) {
        navLayout->setContentsMargins(0, 0, 0, 0);
    }

    // Перевіряємо, чи є збережений користувач
    UserProfile currentUser;
    m_userExists = m_dbManager->getCurrentUserProfile(currentUser);

    if (m_userExists) {
        // Завантажуємо налаштування
        m_settingsPage->loadCurrentSettings(currentUser);
        m_profilePage->setInternalProfile(currentUser);

        UserLogger::log(Info, QString("User profile (ID=%1) loaded and passed to ProfilePage.")
                                  .arg(currentUser.getId()));

        //  ОНОВЛЕННЯ: передаємо MatchesPage userId + DB
        m_matchesPage->setDatabaseManager(m_dbManager);
        m_matchesPage->setCurrentUserId(currentUser.getId());
    } else {
        UserLogger::log(Info, "No user found. App running in 'guest' mode.");
    }

    ui->stackedWidget_Pages->setCurrentWidget(m_welcomePage);
    UserLogger::log(Info, "Showing Welcome Page.");

    //  ОНОВЛЕННЯ: підключаємо MATCH сигнал
    connect(m_searchPage, &SearchPageWidget::matchFound,
            m_matchesPage, &MatchesPageWidget::onMatchCreated);

    connect(m_profilePage, &ProfilePageWidget::profileSaved,
            this, &MainWindow::onProfileSaved);

    connect(m_adminPage, &AdminPageWidget::backClicked, this, [this](){
        ui->stackedWidget_Pages->setCurrentWidget(m_settingsPage);
    });

    connect(m_settingsPage, &SettingsPageWidget::openAdminPanelRequested,
            this, &MainWindow::showAdminPage);
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
}

void MainWindow::on_btn_Profile_clicked()
{
    ui->stackedWidget_Pages->setCurrentWidget(m_profilePage);
}

void MainWindow::on_btn_Matches_clicked()
{
    if (!m_userExists) {
        QMessageBox::warning(this, tr("Профіль не знайдено"),
                             tr("У вас не може бути метчів, доки ви не створите профіль."));
        return;
    }

    //  перезавантажуємо список на випадок нових матчів
    m_matchesPage->reloadMatches();

    ui->stackedWidget_Pages->setCurrentWidget(m_matchesPage);
}

void MainWindow::on_btn_Settings_clicked()
{
    ui->stackedWidget_Pages->setCurrentWidget(m_settingsPage);
}

void MainWindow::switchLanguage(const QString& languageCode)
{
    qApp->removeTranslator(&m_translator);

    QString path = ":/translations/app_" + languageCode + ".qm";
    if (m_translator.load(path)) {
        qApp->installTranslator(&m_translator);
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
    }

    m_settingsPage->updateThemeIcon(isDark);
}

void MainWindow::onProfileSaved()
{
    UserProfile currentUser;

    if (m_dbManager->getCurrentUserProfile(currentUser)) {

        m_userExists = true;

        //  ВАЖЛИВО: передаємо userId в MatchesPage тільки після реєстрації
        m_matchesPage->setDatabaseManager(m_dbManager);
        m_matchesPage->setCurrentUserId(currentUser.getId());

        m_settingsPage->loadCurrentSettings(currentUser);
    }
}

void MainWindow::showAdminPage() {
    ui->stackedWidget_Pages->setCurrentWidget(m_adminPage);
    m_adminPage->refreshTable();
}
