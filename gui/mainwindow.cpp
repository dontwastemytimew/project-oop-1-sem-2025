#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "UserLogger.h"
#include "searchpagewidget.h"
#include "profilepagewidget.h"
#include "matchespagewidget.h"
#include "settingspagewidget.h"

MainWindow::MainWindow(DatabaseManager* dbManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dbManager(dbManager)
{
    ui->setupUi(this);
    setWindowTitle(tr("project"));

    m_searchPage = new SearchPageWidget(this);
    m_profilePage = new ProfilePageWidget(this);
    m_matchesPage = new MatchesPageWidget(this);
    m_settingsPage = new SettingsPageWidget(this);

    m_profilePage->setDatabaseManager(m_dbManager);
    m_settingsPage->setMainWindow(this);

    m_searchPage->setDatabaseManager(m_dbManager);

    ui->stackedWidget_Pages->addWidget(m_searchPage);
    ui->stackedWidget_Pages->addWidget(m_profilePage);
    ui->stackedWidget_Pages->addWidget(m_matchesPage);
    ui->stackedWidget_Pages->addWidget(m_settingsPage);

    ui->stackedWidget_Pages->setCurrentWidget(m_searchPage);
    UserLogger::log(Info, "MainWindow initialized. Showing Search Page.");
     switchLanguage("ua");

    QLayout* navLayout = ui->nav_menu->layout();

    if (navLayout) {
        navLayout->setContentsMargins(0, 0, 0, 0);
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btn_Search_clicked()
{
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
    ui->stackedWidget_Pages->setCurrentWidget(m_matchesPage);
    UserLogger::log(Info, "User switched to Matches Page");
}

void MainWindow::switchLanguage(const QString& languageCode)
{
    qApp->removeTranslator(&m_translator);

    QString path = ":/translations/app_" + languageCode + ".qm";
    if (m_translator.load(path)) {
        qApp->installTranslator(&m_translator);
        UserLogger::log(Info, "Language switched to: " + languageCode);
    } else {
        UserLogger::log(Warning, "Failed to load translation file: " + path);
        qApp->removeTranslator(&m_translator);
    }
}

void MainWindow::on_btn_Settings_clicked()
{
    ui->stackedWidget_Pages->setCurrentWidget(m_settingsPage);
    UserLogger::log(Info, "User switched to Settings Page");
}