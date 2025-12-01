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
    // --- Конструктори та Базові операції ---
    DatabaseManager();
    ~DatabaseManager();
    bool openDatabase();
    void closeDatabase();
    bool createTables();

    // --- CRUD Операції ---
    int saveProfile(const UserProfile &profile);
    bool loadProfileByEmail(const QString &email, UserProfile &profile);

    /**
     * @brief Завантажує профіль за унікальним ID.
     */
    bool loadProfileById(int userId, UserProfile& profile); // <--- ПРАВИЛЬНЕ ОГОЛОШЕННЯ

    bool profileExists(const QString &email) const;
    bool updateProfile(const UserProfile &profile);
    bool deleteProfile(int profileId);
    bool setProfileHidden(int profileId, bool isHidden);

    // --- Отримання Профілів ---
    QList<UserProfile> getProfilesByCriteria(const Preference &prefs);
    QList<UserProfile> getAllProfiles();
    QList<UserProfile> getProfilesByIds(const QList<int> &ids);
    QStringList getAllCities();

    // --- Адмінка та Статистика ---
    QSqlTableModel* getUsersModel(QObject* parent = nullptr);
    QMap<QString, int> getCityStatistics();
    QMap<QString, int> getGenderStatistics();
    QMap<QString, int> getAgeStatistics();
    bool saveProfileBulk(const QList<UserProfile> &profiles);
    int countUsers();

    // --- LIKES / MATCHES / СЕСІЯ ---

    /**
     * @brief Додає лайк від одного користувача іншому.
     */
    bool addLike(int fromUserId, int toUserId);

    /**
     * @brief Видаляє лайк.
     */
    bool removeLike(int fromUserId, int toUserId);

    /**
     * @brief Перевіряє, чи користувач лайкнув цільовий профіль.
     */
    bool hasUserLiked(int userId, int targetId) const;

    /**
     * @brief Перевіряє, чи лайки є взаємними (Match).
     */
    bool isMutualLike(int user1, int user2) const;

    /**
     * @brief Отримує список ID профілів, з якими відбувся взаємний лайк (Метч).
     */
    QList<int> getMutualMatchIds(int currentUserId);

 /**
     * @brief Отримує поточний профіль (для симуляції "сесії").
     * @param profile Посилання на об'єкт, який буде заповнено.
     * @return true, якщо профіль знайдено.
     */
 bool getCurrentUserProfile(UserProfile &profile);

private:
    QSqlDatabase m_db;
    const QString DB_NAME = "dating_agency.db";
};

#endif // DATABASEMANAGER_H