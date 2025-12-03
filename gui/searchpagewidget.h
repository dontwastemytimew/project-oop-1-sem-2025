#ifndef SEARCHPAGEWIDGET_H
#define SEARCHPAGEWIDGET_H

#include <QSpinBox>
#include <QComboBox>
#include <QStackedWidget>
#include <QLabel>
#include "DatabaseManager.h"
#include "UserProfile.h"
#include "MatchEngine.h"

/**
 * @brief Віджет сторінки пошуку ("Свайпи").
 *
 * Цей клас реалізує основний функціонал знайомств:
 * - Налаштування фільтрів пошуку (вік, місто, стать, орієнтація).
 * - Відображення карток користувачів, що відповідають критеріям.
 * - Логіку "Лайк" (вподобати) та "Скіп" (пропустити).
 * - Виявлення "Метчів" (взаємних лайків) та показ повідомлення про успіх.
 */
class SearchPageWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор SearchPageWidget.
     * @param parent Батьківський віджет.
     */
    explicit SearchPageWidget(QWidget *parent = nullptr);

    /**
     * @brief Деструктор. Звільняє ресурси (MatchEngine).
     */
    ~SearchPageWidget();

    /**
     * @brief Встановлює залежність від менеджера бази даних.
     *
     * Також ініціалізує MatchEngine, оскільки йому потрібен доступ до БД.
     * @param dbManager Вказівник на DatabaseManager.
     */
    void setDatabaseManager(DatabaseManager* dbManager);

    /**
     * @brief Встановлює профіль поточного авторизованого користувача.
     *
     * Це необхідно для:
     * 1. Виключення самого себе з результатів пошуку.
     * 2. Розрахунку сумісності між "мною" та кандидатами.
     * 3. Запису лайків від мого імені.
     *
     * @param profile Профіль поточного користувача.
     */
    void setCurrentUser(const UserProfile& profile);

signals:
    /**
     * @brief Сигнал про успішний "Метч" (Взаємний лайк).
     *
     * Емітується, коли поточний користувач лайкає когось, хто вже лайкнув його раніше.
     * MainWindow перехоплює цей сигнал, щоб оновити сторінку "Метчі" в реальному часі.
     *
     * @param currentUserId ID поточного користувача.
     * @param targetId ID користувача, з яким стався метч.
     */
    void matchFound(int currentUserId, int targetId);

protected:
    /**
     * @brief Обробник подій зміни стану (зокрема, зміни мови).
     * @param event Подія.
     */
    void changeEvent(QEvent *event) override;

private slots:
    /**
     * @brief Слот натискання кнопки "Знайти пару".
     *
     * Збирає критерії з полів фільтрації (Preference), звертається до DatabaseManager
     * для отримання списку кандидатів та запускає показ першого профілю.
     */
    void on_btn_Find_clicked();

    /**
     * @brief Слот натискання кнопки "Like".
     *
     * 1. Додає запис про лайк у базу даних.
     * 2. Перевіряє через DatabaseManager::isMutualLike, чи є цей лайк взаємним.
     * 3. Якщо метч - показує вітання (showMatchPopup) та емітує сигнал matchFound.
     * 4. Переходить до наступного профілю.
     */
    void on_Like_clicked();

    /**
     * @brief Слот натискання кнопки "Skip".
     *
     * Просто переходить до наступного профілю у списку без запису лайка.
     */
    void on_Skip_clicked();

private:
    /**
     * @brief Відображає наступний профіль зі списку знайдених.
     *
     * Збільшує індекс m_currentMatchIndex. Створює та налаштовує віджет ProfileCard
     * для відображення даних кандидата. Якщо список закінчився, показує повідомлення про це.
     */
    void showNextProfile();

    /**
     * @brief Налаштовує автодоповнення для поля введення міста.
     */
    void setupCityAutocomplete();

    /**
     * @brief Показує діалогове вікно "It's a Match!".
     * @param target Профіль користувача, з яким стався метч.
     */
    void showMatchPopup(const UserProfile& target);

    /**
     * @brief Оновлює тексти інтерфейсу при зміні мови.
     */
    void retranslateUi();

    DatabaseManager* m_dbManager;   ///< Доступ до бази даних.
    MatchEngine* m_matchEngine;     ///< Логіка розрахунку сумісності.
    UserProfile m_currentUser;      ///< Поточний користувач.

    QList<UserProfile> m_currentMatches; ///< Список завантажених кандидатів.
    int m_currentMatchIndex;             ///< Індекс поточного профілю, що переглядається.

    // --- Елементи UI (Фільтри) ---
    QSpinBox* m_minAgeSpin;         ///< Мін. вік.
    QSpinBox* m_maxAgeSpin;         ///< Макс. вік.
    QComboBox* m_genderCombo;       ///< Бажана стать.
    QComboBox* m_orientationCombo;  ///< Бажана орієнтація.
    QLineEdit* m_cityEdit;          ///< Бажане місто.
    QPushButton* m_findButton;      ///< Кнопка запуску пошуку.

    // --- Елементи UI (Лейбли) ---
    QLabel* m_labelMinAge;
    QLabel* m_labelMaxAge;
    QLabel* m_labelGender;
    QLabel* m_labelOrientation;
    QLabel* m_labelCity;
    QLabel* m_placeholderLabel;     ///< Текст, що показується, коли немає результатів або пошук не запущено.

    // --- Елементи UI (Результати) ---
    QStackedWidget* m_resultsStack; ///< Контейнер для перемикання між "Карткою" та "Заглушкою".
    QPushButton* m_likeButton;      ///< Кнопка Лайк.
    QPushButton* m_skipButton;      ///< Кнопка Скіп.
};

#endif // SEARCHPAGEWIDGET_H