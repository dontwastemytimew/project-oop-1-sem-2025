#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include "searchpagewidget.h"
#include "profilepagewidget.h"
#include "matchespagewidget.h"
#include "DatabaseManager.h"
#include "welcomepagewidget.h"
#include "adminpagewidget.h"
#include <QButtonGroup>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class SearchPageWidget;
class ProfilePageWidget;
class MatchesPageWidget;
class SettingsPageWidget;
class WelcomePageWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(DatabaseManager* dbManager, QWidget *parent = nullptr);
    ~MainWindow();

    void switchLanguage(const QString& languageCode);
    void switchTheme(bool isDark);
    void showAdminPage();

    private slots:
    void on_btn_Search_clicked();
    void on_btn_Profile_clicked();
    void on_btn_Matches_clicked();
    void on_btn_Settings_clicked();
    void onProfileSaved();

private:
    Ui::MainWindow *ui;

    WelcomePageWidget *m_welcomePage;
    SearchPageWidget *m_searchPage;
    ProfilePageWidget *m_profilePage;
    MatchesPageWidget *m_matchesPage;
    DatabaseManager* m_dbManager;
    QTranslator m_translator;
    SettingsPageWidget *m_settingsPage;
    QButtonGroup *m_navButtonGroup;
    bool m_userExists;
    AdminPageWidget* m_adminPage;
};

#endif // MAINWINDOW_H