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
 *
 * Основні можливості:
 * - Зміна мови інтерфейсу (Українська/Англійська).
 * - Перемикання теми оформлення (Світла/Темна).
 * - "Призупинення" акаунту (приховування профілю з пошуку).
 * - Повне видалення акаунту.
 * - Доступ до Панелі Адміністратора (кнопка з'являється для всіх, але логіка може бути розширена).
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
     *
     * Необхідно для виклику глобальних методів MainWindow, таких як
     * switchLanguage() та switchTheme().
     * @param window Вказівник на MainWindow.
     */
    void setMainWindow(MainWindow* window);

    /**
     * @brief Оновлює іконку кнопки перемикання теми.
     *
     * Змінює іконку (Сонце/Місяць) залежно від поточного стану теми.
     * @param isDark true, якщо активна темна тема.
     */
    void updateThemeIcon(bool isDark);

    /**
     * @brief Обробляє перемикання чекбокса "Призупинити профіль".
     *
     * Викликає метод DatabaseManager для зміни статусу `is_hidden` у базі даних.
     * @param checked Новий стан чекбокса (true = приховати, false = показати).
     */
    void on_pauseToggled(bool checked);

    /**
     * @brief Встановлює залежність від менеджера бази даних.
     * @param dbManager Вказівник на DatabaseManager.
     */
    void setDatabaseManager(DatabaseManager* dbManager);

    /**
     * @brief Завантажує налаштування для поточного користувача.
     *
     * Ініціалізує стан віджетів (наприклад, чи стоїть галочка "Призупинити")
     * на основі даних профілю.
     * @param profile Профіль поточного користувача.
     */
    void loadCurrentSettings(const UserProfile& profile);

    /**
     * @brief Оновлює тексти інтерфейсу при зміні мови.
     */
    void retranslateUi();

signals:
    /**
     * @brief Сигнал запиту на відкриття панелі адміністратора.
     * Обробляється в MainWindow.
     */
    void openAdminPanelRequested();

    /**
     * @brief Сигнал про те, що акаунт було успішно видалено.
     * Використовується для виходу з системи та скидання інтерфейсу.
     */
    void accountDeleted();

private slots:
    /**
     * @brief Слот зміни мови через випадаючий список.
     * @param index Індекс обраної мови у QComboBox.
     */
    void on_languageChanged(int index);

    /**
     * @brief Слот натискання кнопки перемикання теми.
     */
    void on_themeToggled();

    /**
     * @brief Слот натискання кнопки видалення акаунту.
     *
     * Показує діалогове вікно підтвердження. У разі згоди видаляє дані з БД
     * та емітує сигнал accountDeleted().
     */
    void on_deleteClicked();

    /**
     * @brief Слот натискання кнопки входу в адмінку.
     */
    void onAdminBtnClicked();

private:
    MainWindow* m_mainWindow;           ///< Посилання на головне вікно.
    DatabaseManager* m_dbManager;       ///< Посилання на базу даних.
    int m_currentProfileId;             ///< ID поточного користувача (для операцій з акаунтом).
    bool m_isDarkTheme;                 ///< Поточний стан теми.

    // Елементи інтерфейсу
    QComboBox* m_langComboBox;          ///< Вибір мови.
    QPushButton* m_themeToggle;         ///< Кнопка теми.
    QCheckBox* m_pauseToggle;           ///< Чекбокс приватності.
    QPushButton* m_deleteButton;        ///< Кнопка видалення (червона).
    QPushButton* m_btnOpenAdmin;        ///< Кнопка адмін-панелі.

    // Лейбли (для перекладу)
    QLabel* m_langLabel;
    QLabel* m_themeLabel;
    QLabel* m_accountLabel;
};

#endif // SETTINGSPAGEWIDGET_H