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
    DatabaseManager();
    ~DatabaseManager();

    bool openDatabase();
    void closeDatabase();
    bool createTables();

    int saveProfile(const UserProfile &profile);

    bool loadProfileByEmail(const QString &email, UserProfile &profile);
    bool profileExists(const QString &email) const;
    bool updateProfile(const UserProfile &profile);
    bool deleteProfile(int profileId);
    bool setProfileHidden(int profileId, bool isHidden);
    bool getCurrentUserProfile(UserProfile &profile);

    QList<UserProfile> getProfilesByCriteria(const Preference &prefs);
    QList<UserProfile> getAllProfiles();

    QSqlTableModel* getUsersModel(QObject* parent = nullptr);

    QMap<QString, int> getCityStatistics();
    QMap<QString, int> getGenderStatistics();
    QMap<QString, int> getAgeStatistics();

    bool saveProfileBulk(const QList<UserProfile> &profiles);
    int countUsers();

    QStringList getAllCities();

    //
    // ★★★ LIKES / MATCHES
    //
    bool addLike(int userId, int targetId);
    bool removeLike(int userId, int targetId);

    bool hasUserLiked(int userId, int targetId) const;
    bool isMutualLike(int userId, int targetId) const;

    QList<int> getMatches(int userId) const;

private:
    QSqlDatabase m_db;
    const QString DB_NAME = "dating_agency.db";
};

#endif // DATABASEMANAGER_H
