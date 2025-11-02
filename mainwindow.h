#include "UserLogger.h"

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
