#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "UserLogger.h"
#include <profilecard.h>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnRegister_clicked();
    void on_btnSearch_clicked();

private:
    ProfileCard* profileCard;
};

#endif // MAINWINDOW_H
