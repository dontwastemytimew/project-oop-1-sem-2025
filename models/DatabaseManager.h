#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QMap>
#include <QStringList>

#include "UserProfile.h"
#include "Preference.h"

/**
 * @brief Клас DatabaseManager
 *
 * Відповідає за всі операції з базою даних SQLite:
 * - з'єднання
 * - створення таблиць
 * - CRUD операції
 * - статистика
 * - лайки/метчі
 */
class DatabaseManager {
public:
    /**
     * @brief Конструктор. Налаштовує з'єднання QSQLDatabase.
     */
    DatabaseManager();

    /**
     * @brief Деструктор. Автоматично викликає closeDatabase().
     */
    ~DatabaseManager();

    /**
     * @brief Відкриває з'єднання з БД та створює таблиці, якщо їх немає.
     * @return true, якщо підключення та ініціалізація успішні.
     */
    bool openDatabase();

    /**
     * @brief Закриває з'єднання з базою даних.
     */
    void closeDatabase();

    /**
     * @brief Створює необхідні таблиці (напр., 'users'), якщо вони ще не існують.
     * @return true, якщо таблиці успішно створені або вже існують.
     */
    bool createTables();

    // --- CRUD Операції ---

    /**
     * @brief Зберігає новий профіль.
     * @return Унікальний ID нового профілю, або -1 у разі помилки.
     */
    int saveProfile(const UserProfile &profile);

    /**
     * @brief (Read) Завантажує профіль з БД за унікальним email.
     * @param email Email користувача для пошуку.
     * @param profile Посилання на об'єкт UserProfile, який буде заповнено даними (якщо знайдено).
     * @return true, якщо профіль знайдено та успішно завантажено.
     */
    bool loadProfileByEmail(const QString &email, UserProfile &profile);

    /**
     * @brief (Read) Перевіряє, чи існує профіль з таким email.
     * @param email Email для перевірки.
     * @return true, якщо профіль вже існує.
     */
    bool profileExists(const QString &email) const;

    /**
     * @brief (Update) Оновлює існуючий профіль у базі даних.
     * Ідентифікує профіль за його унікальним ID.
     * @param profile Об'єкт UserProfile з оновленими даними (ID має бути коректним).
     * @return true, якщо оновлення пройшло успішно.
     */
    bool updateProfile(const UserProfile &profile);

    /**
     * @brief (Delete) Видаляє профіль з бази даних за його ID.
     * @param profileId Унікальний ID профілю для видалення.
     * @return true, якщо видалення пройшло успішно.
     */
    bool deleteProfile(int profileId);

    /**
     * @brief Встановлює статус "прихований" для профілю.
     * @param profileId ID профілю.
     * @param isHidden true, щоб приховати, false, щоб показати.
     * @return true, якщо операція успішна.
     */
    bool setProfileHidden(int profileId, bool isHidden);

    /**
     * @brief Отримує поточний профіль (для симуляції "сесії").
     * @param profile Посилання на об'єкт, який буде заповнено.
     * @return true, якщо профіль знайдено.
     */
    bool getCurrentUserProfile(UserProfile &profile);

    /**
     * @brief (Read) Повертає список профілів, що відповідають критеріям.
     * @param prefs Об'єкт Preference з критеріями пошуку.
     * @return Список об'єктів UserProfile.
     */
    QList<UserProfile> getProfilesByCriteria(const Preference &prefs);

    /**
     * @brief Отримує список усіх профілів (використовується для тестів).
     */
    QList<UserProfile> getAllProfiles();

    /**
     * @brief Створює та повертає модель таблиці для перегляду всіх користувачів.
     * @param parent Батьківський об'єкт для управління пам'яттю.
     */
    QSqlTableModel* getUsersModel(QObject* parent = nullptr);


    /**
     * @brief Отримує статистику кількості користувачів по містах (Top-5).
     * @return QMap, де ключ - назва міста, значення - кількість користувачів.
     */
    QMap<QString, int> getCityStatistics();

    /**
     * @brief Отримує статистику розподілу користувачів за статтю.
     * @return QMap, де ключ - стать (Чоловік/Жінка), значення - кількість.
     */
    QMap<QString, int> getGenderStatistics();

    /**
     * @brief Отримує статистику розподілу користувачів за віковими групами.
     * @return QMap, де ключ - вікова група (напр. "18-25"), значення - кількість.
     */
    QMap<QString, int> getAgeStatistics();

    /**
     * @brief Швидкісний інсерт списку профілів в одній транзакції.
     */
    bool saveProfileBulk(const QList<UserProfile> &profiles);

    /**
     * @brief Рахує загальну кількість користувачів у таблиці.
     * @return Кількість користувачів.
     */
    int countUsers();

    /**
     * @brief Отримує список усіх унікальних міст з профілів.
     * @return QStringList унікальних назв міст.
     */
    QStringList getAllCities(); // <--- МЕТОД З ОБОХ БЛОКІВ

    
    /**
     * @brief Додає лайк від одного користувача іншому.
     * @return true, якщо операція успішна.
     */
    bool addLike(int userId, int targetId);

    /**
     * @brief Видаляє лайк.
     * @return true, якщо операція успішна.
     */
    bool removeLike(int userId, int targetId);

    /**
     * @brief Перевіряє, чи користувач лайкнув цільовий профіль.
     */
    bool hasUserLiked(int userId, int targetId) const;

    /**
     * @brief Перевіряє, чи лайки є взаємними (Match).
     */
    bool isMutualLike(int userId, int targetId) const;

    /**
     * @brief Отримує список ID профілів, які лайкнув поточний користувач.
     * @param userId ID поточного користувача.
     * @return Список ID цільових профілів.
     */
    QList<int> getMatches(int userId) const;

    /**
     * @brief Отримує список ID профілів, з якими відбувся взаємний лайк (Метч).
     * @param currentUserId ID поточного користувача.
     * @return Список ID, які лайкнули нас у відповідь.
     */
    QList<int> getMutualMatchIds(int currentUserId);

    /**
     * @brief Отримує повні профілі за списком їхніх ID.
     * @param ids Список ID для пошуку.
     * @return Список об'єктів UserProfile.
     */
    QList<UserProfile> getProfilesByIds(const QList<int> &ids);


private:
    QSqlDatabase m_db;
    const QString DB_NAME = "dating_agency.db";
};

#endif // DATABASEMANAGER_H