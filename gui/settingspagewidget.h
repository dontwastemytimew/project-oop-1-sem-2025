#ifndef SETTINGSPAGEWIDGET_H
#define SETTINGSPAGEWIDGET_H

#include <QCheckBox>
#include <QLabel>
#include <QWidget>
#include "DatabaseManager.h"

// Forward declarations
class MainWindow;
class QComboBox;
class QPushButton;
class UserProfile;

/**
 * @brief Віджет сторінки "Налаштування".
 *
 * Цей клас надає інтерфейс для зміни глобальних параметрів додатку та
 * управління поточним акаунтом користувача.
 */
class SettingsPageWidget : public QWidget {
    Q_OBJECT
public:
    /**
     * @brief Конструктор SettingsPageWidget.
     * @param parent Батьківський віджет.
     */
    explicit SettingsPageWidget(QWidget *parent = nullptr);

    /**
     * @brief Встановлює посилання на головне вікно.
     * @param window Вказівник на MainWindow.
     */
    void setMainWindow(MainWindow* window);

    /**
     * @brief Оновлює іконку кнопки перемикання теми.
     * @param isDark true, якщо активна темна тема.
     */
    void updateThemeIcon(bool isDark);

    /**
     * @brief Обробляє перемикання чекбокса "Призупинити профіль".
     * @param checked Новий стан чекбокса.
     */
    void on_pauseToggled(bool checked);

    /**
     * @brief Встановлює залежність від менеджера бази даних.
     * @param dbManager Вказівник на DatabaseManager.
     */
    void setDatabaseManager(DatabaseManager* dbManager);

    /**
     * @brief Завантажує налаштування для поточного користувача.
     * @param profile Профіль поточного користувача.
     */
    void loadCurrentSettings(const UserProfile& profile);

    /**
     * @brief Оновлює тексти інтерфейсу при зміні мови.
     */
    void retranslateUi();

// --- ДОДАНО СЕКЦІЮ PROTECTED ---
protected:
    /**
     * @brief Обробник подій зміни стану віджета.
     *
     * Перевизначений метод для перехоплення події зміни мови (QEvent::LanguageChange).
     * Це дозволяє сторінці миттєво оновити переклад без перезапуску.
     *
     * @param event Вказівник на об'єкт події.
     */
    void changeEvent(QEvent *event) override;
// --------------------------------

signals:
    /**
     * @brief Сигнал запиту на відкриття панелі адміністратора.
     */
    void openAdminPanelRequested();

    /**
     * @brief Сигнал про те, що акаунт було успішно видалено.
     */
    void accountDeleted();

private slots:
    /**
     * @brief Слот зміни мови через випадаючий список.
     */
    void on_languageChanged(int index);

    /**
     * @brief Слот натискання кнопки перемикання теми.
     */
    void on_themeToggled();

    /**
     * @brief Слот натискання кнопки видалення акаунту.
     */
    void on_deleteClicked();

    /**
     * @brief Слот натискання кнопки входу в адмінку.
     */
    void onAdminBtnClicked();

private:
    MainWindow* m_mainWindow;           ///< Посилання на головне вікно.
    DatabaseManager* m_dbManager;       ///< Посилання на базу даних.
    int m_currentProfileId;             ///< ID поточного користувача.
    bool m_isDarkTheme;                 ///< Поточний стан теми.

    // Елементи інтерфейсу
    QComboBox* m_langComboBox;
    QPushButton* m_themeToggle;
    QCheckBox* m_pauseToggle;
    QPushButton* m_deleteButton;
    QPushButton* m_btnOpenAdmin;

    // Лейбли (для перекладу)
    QLabel* m_langLabel;
    QLabel* m_themeLabel;
    QLabel* m_accountLabel;
};

#endif // SETTINGSPAGEWIDGET_H