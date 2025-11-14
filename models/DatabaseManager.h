#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QVariant>
#include "UserProfile.h"
#include "Preference.h"

/**
 * @brief Клас DatabaseManager
 *
 * Відповідає за всі операції з базою даних SQLite (з'єднання,
 * створення таблиць та повний CRUD для профілів користувачів).
 * Це єдиний клас у програмі, який безпосередньо працює з SQL.
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
     * @brief (Create) Зберігає новий профіль у базі даних.
     * Використовує транзакцію для безпечного збереження.
     * @param profile Об'єкт UserProfile для збереження.
     * @return true, якщо збереження пройшло успішно.
     */
    bool saveProfile(const UserProfile &profile);

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
     * @brief (Read) Повертає список профілів, що відповідають критеріям.
     * @param prefs Об'єкт Preference з критеріями пошуку.
     * @return Список об'єктів UserProfile.
     */
 QList<UserProfile> getProfilesByCriteria(const Preference &prefs);


private:
    /// Об'єкт з'єднання з базою даних Qt.
    QSqlDatabase m_db;

    /// Ім'я файлу бази даних.
    const QString DB_NAME = "dating_agency.db";
};

#endif //DATABASEMANAGER_H
