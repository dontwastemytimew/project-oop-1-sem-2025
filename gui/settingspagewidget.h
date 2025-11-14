#ifndef SETTINGSPAGEWIDGET_H
#define SETTINGSPAGEWIDGET_H

#include <QWidget>


class MainWindow;
class QComboBox;

class SettingsPageWidget : public QWidget {
    Q_OBJECT
public:
    explicit SettingsPageWidget(QWidget *parent = nullptr);
    void setMainWindow(MainWindow* window);

    private slots:
        void on_languageChanged(int index);

private:
    MainWindow* m_mainWindow;
    QComboBox* m_langComboBox;
};
#endif // SETTINGSPAGEWIDGET_H