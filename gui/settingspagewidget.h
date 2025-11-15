#ifndef SETTINGSPAGEWIDGET_H
#define SETTINGSPAGEWIDGET_H

#include <QWidget>
#include <QCheckBox>


#include "DatabaseManager.h"


class MainWindow;
class QComboBox;
class QPushButton;
class MainWindow;

class UserProfile;

class SettingsPageWidget : public QWidget {
    Q_OBJECT
public:
    explicit SettingsPageWidget(QWidget *parent = nullptr);
    void setMainWindow(MainWindow* window);
    void updateThemeIcon(bool isDark);
    void on_pauseToggled(bool checked);
    void setDatabaseManager(DatabaseManager* dbManager);
    void loadCurrentSettings(const UserProfile& profile);

    private slots:
        void on_languageChanged(int index);
    void on_themeToggled();
    void on_deleteClicked();

private:
    MainWindow* m_mainWindow;
    QComboBox* m_langComboBox;
    QPushButton* m_themeToggle;
    bool m_isDarkTheme;
    QCheckBox* m_pauseToggle;
    DatabaseManager* m_dbManager;
    QPushButton* m_deleteButton;
    int m_currentProfileId;
};
#endif // SETTINGSPAGEWIDGET_H