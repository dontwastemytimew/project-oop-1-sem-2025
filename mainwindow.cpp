#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "UserLogger.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    profileCard(new ProfileCard(this))
{
    setWindowTitle(tr("Бюро знайомств"));

    //тестові дані профілю
    profileCard->setProfileData("Іван", 25, "Київ", tr("Любить подорожі та каву"));
    setCentralWidget(profileCard);

    UserLogger::log(Info, "Головне вікно ініціалізовано");
}

MainWindow::~MainWindow() {}

void MainWindow::on_btnRegister_clicked()
{
    UserLogger::log(Info, "Користувач натиснув кнопку 'Реєстрація'");
}

void MainWindow::on_btnSearch_clicked()
{
    UserLogger::log(Info, "Користувач натиснув кнопку 'Пошук'");
}
