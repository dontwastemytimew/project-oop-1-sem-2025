#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include <QButtonGroup>
#include "searchpagewidget.h"
#include "profilepagewidget.h"
#include "matchespagewidget.h"
#include "DatabaseManager.h"
#include "welcomepagewidget.h"
#include "adminpagewidget.h"
#include "settingspagewidget.h"
#include "ChatManager.h"

// Forward declaration для уникнення циклічних залежностей
class ChatPageWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief Головне вікно додатку Match++.
 *
 * Це основний контролер додатку, який відповідає за:
 * - Ініціалізацію та компонування всіх сторінок (Search, Profile, Matches, Settings, Admin).
 * - Навігацію між сторінками через QStackedWidget.
 * - Управління глобальними налаштуваннями (мова, тема оформлення).
 * - Зв'язок між компонентами (сигнали та слоти) та передачу даних (DatabaseManager, UserProfile).
 * - Обробку життєвого циклу сесії користувача (вхід, вихід, видалення акаунту).
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Конструктор MainWindow.
     *
     * Ініціалізує інтерфейс, створює всі сторінки, налаштовує з'єднання з базою даних,
     * завантажує профіль поточного користувача (якщо є) та встановлює початкову тему/мову.
     *
     * @param dbManager Вказівник на ініціалізований менеджер бази даних.
     * @param parent Батьківський віджет (зазвичай nullptr для головного вікна).
     */
    explicit MainWindow(DatabaseManager* dbManager, QWidget *parent = nullptr);

    /**
     * @brief Деструктор. Звільняє пам'ять, виділену під UI та дочірні віджети.
     */
    ~MainWindow();

    /**
     * @brief Змінює мову додатку.
     *
     * Завантажує відповідний файл перекладу (.qm) та оновлює тексти
     * на всіх активних віджетах.
     *
     * @param languageCode Код мови ("ua" або "en").
     */
    void switchLanguage(const QString& languageCode);

    /**
     * @brief Перемикає тему оформлення (Світла/Темна).
     *
     * Завантажує та застосовує відповідний QSS файл стилів до всього додатку.
     *
     * @param isDark true для темної теми, false для світлої.
     */
    void switchTheme(bool isDark);

    /**
     * @brief Активує та показує Панель Адміністратора.
     *
     * Викликається з налаштувань. Перемикає стек віджетів на сторінку адміністрування.
     */
    void showAdminPage();

protected:
    /**
     * @brief Обробник подій зміни стану (зокрема, зміни мови).
     * @param event Подія.
     */
    void changeEvent(QEvent *event) override;

private slots:
    // --- Слоти навігації (кнопки меню) ---
    void on_btn_Search_clicked();
    void on_btn_Profile_clicked();
    void on_btn_Matches_clicked();
    void on_btn_Settings_clicked();
    void on_btn_Exit_clicked();

    /**
     * @brief Слот, що викликається після успішного збереження/створення профілю.
     *
     * Оновлює внутрішній об'єкт m_currentProfile, активує навігацію та перемикає
     * користувача на сторінку пошуку.
     */
    void onProfileSaved();

    /**
     * @brief Слот обробки видалення акаунту.
     *
     * Очищає сесію, скидає інтерфейс до початкового стану (сторінка вітання/створення профілю).
     */
    void onAccountDeleted();

    /**
     * @brief Відкриває чат з конкретним користувачем.
     * @param targetUserId ID співрозмовника.
     */
    void onOpenChatRequested(int targetUserId);

private:
    /**
     * @brief Оновлює тексти кнопок меню, додаючи стилізовані префікси.
     *
     * Використовується для створення ефекту "дерева файлів" (наприклад, "> Профіль").
     * Викликається після перекладу інтерфейсу.
     */
    void updateMenuLabels();

    Ui::MainWindow *ui;

    // --- Сторінки додатку (дочірні віджети) ---
    WelcomePageWidget *m_welcomePage;   ///< Сторінка вітання / Входу.
    SearchPageWidget *m_searchPage;     ///< Сторінка пошуку (свайпів).
    ProfilePageWidget *m_profilePage;   ///< Сторінка редагування власного профілю.
    MatchesPageWidget *m_matchesPage;   ///< Сторінка списку метчів.
    SettingsPageWidget *m_settingsPage; ///< Сторінка налаштувань.
    AdminPageWidget* m_adminPage;       ///< Панель адміністратора.
    ChatPageWidget *m_chatPage;         ///< Віджет чату (відкривається окремо).

    // --- Системні компоненти ---
    DatabaseManager* m_dbManager;       ///< Менеджер бази даних.
    QTranslator m_translator;           ///< Об'єкт для перекладу інтерфейсу.
    ChatManager* m_chatManager;         ///< Менеджер логіки чату.

    QButtonGroup *m_navButtonGroup;     ///< Група кнопок навігації (для ексклюзивного вибору).

    bool m_userExists;                  ///< Прапорець: чи залогінений користувач.
    UserProfile m_currentProfile;       ///< Дані поточного активного користувача.
};

#endif // MAINWINDOW_H