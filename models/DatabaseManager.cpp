#include "DatabaseManager.h"
#include "UserLogger.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSettings>
#include <QFile>
#include <QDir>
#include "Preference.h"

DatabaseManager::DatabaseManager() {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(DB_NAME);
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

bool DatabaseManager::openDatabase() {
    if (m_db.open()) {
        UserLogger::log(Info, "Database connection successful.");
        return createTables();
    } else {
        UserLogger::log(Critical, "Failed to connect to database: " + m_db.lastError().text());
        return false;
    }
}

void DatabaseManager::closeDatabase() {
    if (m_db.isOpen()) {
        m_db.close();
        UserLogger::log(Info, "Database connection closed.");
    }
}

bool DatabaseManager::createTables() {
    QSqlQuery query;

    //
    // ---- USERS ----
    //
    QString createUsersSql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name VARCHAR(100) NOT NULL, "
        "age INTEGER, "
        "city VARCHAR(100), "
        "bio TEXT, "
        "phone VARCHAR(20), "
        "email VARCHAR(100),"
        "gender VARCHAR(20),"
        "orientation VARCHAR(20),"
        "photo_path TEXT,"
        "is_hidden BOOLEAN DEFAULT 0"
        ");";

    if (!query.exec(createUsersSql)) {
        UserLogger::log(Error, "Failed to create 'users' table: " + query.lastError().text());
        return false;
    }

    //
    // ---- LIKES (пункт 4.1) ----
    //
    QString createLikesSql =
        "CREATE TABLE IF NOT EXISTS likes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER NOT NULL, "
        "target_id INTEGER NOT NULL, "
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "UNIQUE(user_id, target_id)"
        ");";

    if (!query.exec(createLikesSql)) {
        UserLogger::log(Error, "Failed to create 'likes' table: " + query.lastError().text());
        return false;
    }

    // Індекси для швидкої роботи
    query.exec("CREATE INDEX IF NOT EXISTS idx_likes_user ON likes(user_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_likes_target ON likes(target_id)");

    UserLogger::log(Info, "Tables 'users' and 'likes' verified/created successfully.");
    return true;
}

//
// ---- SAVE PROFILE ----
//
int DatabaseManager::saveProfile(const UserProfile &profile) {

    if (profile.getName().isEmpty()) {
        UserLogger::log(Warning, "Save failed: Name cannot be empty.");
        return -1;
    }
    if (profile.getContactInfo().getEmail().isEmpty()) {
        UserLogger::log(Warning, "Save failed: Email cannot be empty.");
        return -1;
    }
    if (!profile.getContactInfo().getEmail().contains('@')) {
        UserLogger::log(Warning, "Save failed: Invalid email.");
        return -1;
    }
    if (profile.getAge() < 18) {
        UserLogger::log(Warning, "Save failed: Age must be >=18.");
        return -1;
    }

    if (!m_db.transaction()) {
        UserLogger::log(Error, "Transaction start failed: " + m_db.lastError().text());
        return -1;
    }

    QSqlQuery query;

    query.prepare(
        "INSERT INTO users (name, age, city, bio, phone, email, gender, orientation, photo_path) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
        );

    query.addBindValue(profile.getName());
    query.addBindValue(profile.getAge());
    query.addBindValue(profile.getCity());
    query.addBindValue(profile.getBio());
    query.addBindValue(profile.getContactInfo().getPhone());
    query.addBindValue(profile.getContactInfo().getEmail());
    query.addBindValue(profile.getGender());
    query.addBindValue(profile.getOrientation());
    query.addBindValue(profile.getPhotoPath());

    if (!query.exec()) {
        UserLogger::log(Error, "Failed to save profile: " + query.lastError().text());
        m_db.rollback();
        return -1;
    }

    int newId = query.lastInsertId().toInt();

    if (!m_db.commit()) {
        UserLogger::log(Error, "Failed to commit transaction.");
        m_db.rollback();
        return -1;
    }

    UserLogger::log(Info, "Profile saved. ID = " + QString::number(newId));
    return newId;
}

//
// ---- LOAD PROFILE BY EMAIL ----
//
bool DatabaseManager::loadProfileByEmail(const QString &email, UserProfile &profile) {

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE email = ?");
    query.addBindValue(email);

    if (!query.exec()) {
        UserLogger::log(Error, "Select by email failed: " + query.lastError().text());
        return false;
    }

    if (!query.next())
        return false;

    profile.setId(query.value("id").toInt());
    profile.setName(query.value("name").toString());
    profile.setAge(query.value("age").toInt());
    profile.setCity(query.value("city").toString());
    profile.setBio(query.value("bio").toString());
    profile.setGender(query.value("gender").toString());
    profile.setOrientation(query.value("orientation").toString());
    profile.setPhotoPath(query.value("photo_path").toString());

    ContactInfo contacts(
        query.value("phone").toString(),
        query.value("email").toString()
        );
    profile.setContactInfo(contacts);

    UserLogger::log(Info, "Profile loaded for: " + profile.getName());
    return true;
}

bool DatabaseManager::profileExists(const QString &email) const {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE email=?");
    query.addBindValue(email);

    if (!query.exec()) return false;

    return query.next();
}

//
// ---- UPDATE PROFILE ----
//
bool DatabaseManager::updateProfile(const UserProfile &profile) {

    if (profile.getId() == -1) {
        UserLogger::log(Warning, "Update failed: ID = -1.");
        return false;
    }

    if (!m_db.transaction()) {
        UserLogger::log(Error, "Failed to start update transaction.");
        return false;
    }

    QSqlQuery query;

    query.prepare(
        "UPDATE users SET "
        "name=?, age=?, city=?, bio=?, phone=?, email=?, gender=?, orientation=?, photo_path=? "
        "WHERE id=?"
        );

    query.addBindValue(profile.getName());
    query.addBindValue(profile.getAge());
    query.addBindValue(profile.getCity());
    query.addBindValue(profile.getBio());
    query.addBindValue(profile.getContactInfo().getPhone());
    query.addBindValue(profile.getContactInfo().getEmail());
    query.addBindValue(profile.getGender());
    query.addBindValue(profile.getOrientation());
    query.addBindValue(profile.getPhotoPath());
    query.addBindValue(profile.getId());

    if (!query.exec()) {
        UserLogger::log(Error, "Failed to update profile: " + query.lastError().text());
        m_db.rollback();
        return false;
    }

    if (!m_db.commit()) {
        UserLogger::log(Error, "Commit failed during update.");
        m_db.rollback();
        return false;
    }

    UserLogger::log(Info, "Profile updated successfully.");
    return true;
}

bool DatabaseManager::deleteProfile(int profileId) {
    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE id=?");
    query.addBindValue(profileId);

    return query.exec();
}

bool DatabaseManager::setProfileHidden(int profileId, bool isHidden) {
    QSqlQuery query;
    query.prepare("UPDATE users SET is_hidden=? WHERE id=?");
    query.addBindValue(isHidden);
    query.addBindValue(profileId);
    return query.exec();
}

//
// ---- LOAD CURRENT USER ----
//
bool DatabaseManager::getCurrentUserProfile(UserProfile &profile) {

    QSettings settings("DatingAgency", "TitleApp");

    int id = settings.value("current_user_id", -1).toInt();
    if (id == -1) return false;

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE id=?");
    query.addBindValue(id);

    if (!query.exec()) return false;
    if (!query.next()) return false;

    profile.setId(query.value("id").toInt());
    profile.setName(query.value("name").toString());
    profile.setAge(query.value("age").toInt());
    profile.setCity(query.value("city").toString());
    profile.setBio(query.value("bio").toString());
    profile.setGender(query.value("gender").toString());
    profile.setOrientation(query.value("orientation").toString());
    profile.setPhotoPath(query.value("photo_path").toString());

    profile.setContactInfo(ContactInfo(
        query.value("phone").toString(),
        query.value("email").toString()
        ));

    return true;
}

//
// ---- GET ALL PROFILES ----
//
QList<UserProfile> DatabaseManager::getAllProfiles() {
    QList<UserProfile> profiles;

    QSqlQuery query("SELECT * FROM users WHERE is_hidden=0", m_db);
    if (!query.exec()) {
        UserLogger::log(Error, "Failed to select all profiles: " + query.lastError().text());
        return profiles;
    }

    while (query.next()) {

        UserProfile profile;

        profile.setId(query.value("id").toInt());
        profile.setName(query.value("name").toString());
        profile.setAge(query.value("age").toInt());
        profile.setCity(query.value("city").toString());
        profile.setBio(query.value("bio").toString());
        profile.setGender(query.value("gender").toString());
        profile.setOrientation(query.value("orientation").toString());
        profile.setPhotoPath(query.value("photo_path").toString());

        profile.setContactInfo(ContactInfo(
            query.value("phone").toString(),
            query.value("email").toString()
            ));

        profiles.append(profile);
    }

    return profiles;
}

//
// ---- FILTER BY PREFERENCES ----
//
QList<UserProfile> DatabaseManager::getProfilesByCriteria(const Preference &prefs) {
    QList<UserProfile> profiles;
    QSqlQuery query(m_db);

    QString sql = "SELECT * FROM users WHERE is_hidden = 0";
    QMap<QString, QVariant> bind;

    if (prefs.getMinAge() > 0) {
        sql += " AND age >= :minAge";
        bind[":minAge"] = prefs.getMinAge();
    }
    if (prefs.getMaxAge() > 0) {
        sql += " AND age <= :maxAge";
        bind[":maxAge"] = prefs.getMaxAge();
    }
    if (!prefs.getCity().isEmpty()) {
        sql += " AND city LIKE :city";
        bind[":city"] = "%" + prefs.getCity() + "%";
    }
    if (!prefs.getGender().isEmpty() && prefs.getGender() != "Не важливо") {
        sql += " AND gender = :gender";
        bind[":gender"] = prefs.getGender();
    }
    if (!prefs.getOrientation().isEmpty() && prefs.getOrientation() != "Не важливо") {
        sql += " AND orientation = :orientation";
        bind[":orientation"] = prefs.getOrientation();
    }

    query.prepare(sql);

    for (auto it = bind.begin(); it != bind.end(); ++it)
        query.bindValue(it.key(), it.value());

    if (!query.exec()) {
        UserLogger::log(Error, "Failed to filter profiles: " + query.lastError().text());
        return profiles;
    }

    while (query.next()) {
        UserProfile profile;

        profile.setId(query.value("id").toInt());
        profile.setName(query.value("name").toString());
        profile.setAge(query.value("age").toInt());
        profile.setCity(query.value("city").toString());
        profile.setBio(query.value("bio").toString());
        profile.setGender(query.value("gender").toString());
        profile.setOrientation(query.value("orientation").toString());
        profile.setPhotoPath(query.value("photo_path").toString());

        profile.setContactInfo(ContactInfo(
            query.value("phone").toString(),
            query.value("email").toString()
            ));

        profiles.append(profile);
    }

    return profiles;
}

int DatabaseManager::countUsers() {
    QSqlQuery query("SELECT COUNT(*) FROM users", m_db);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

//
// ---- BULK INSERT ----
//
bool DatabaseManager::saveProfileBulk(const QList<UserProfile> &profiles) {

    if (!m_db.transaction())
        return false;

    QSqlQuery query;

    query.prepare(
        "INSERT INTO users (name, age, city, bio, phone, email, gender, orientation, photo_path) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
        );

    for (const UserProfile &p : profiles) {

        query.addBindValue(p.getName());
        query.addBindValue(p.getAge());
        query.addBindValue(p.getCity());
        query.addBindValue(p.getBio());
        query.addBindValue(p.getContactInfo().getPhone());
        query.addBindValue(p.getContactInfo().getEmail());
        query.addBindValue(p.getGender());
        query.addBindValue(p.getOrientation());
        query.addBindValue(p.getPhotoPath());

        if (!query.exec()) {
            UserLogger::log(Error, "saveProfileBulk FAIL: " + query.lastError().text());
            m_db.rollback();
            return false;
        }
    }

    return m_db.commit();
}

//
// ---- AUTOCOMPLETE МІСТ ----
//
QStringList DatabaseManager::getAllCities() {
    QStringList cities;
    QSqlQuery query(m_db);

    QString sql =
        "SELECT DISTINCT city FROM users "
        "WHERE city IS NOT NULL AND city <> '' "
        "ORDER BY city ASC";

    if (!query.exec(sql)) {
        UserLogger::log(Error, "Failed to load cities: " + query.lastError().text());
        return cities;
    }

    while (query.next()) {
        cities.append(query.value(0).toString());
    }

    return cities;
}

//
// =====================================================================
// ★★★ LIKES / MATCHES
// =====================================================================
//

bool DatabaseManager::addLike(int userId, int targetId) {
    if (userId <= 0 || targetId <= 0 || userId == targetId)
        return false;

    QSqlQuery query(m_db);

    query.prepare("INSERT OR IGNORE INTO likes (user_id, target_id) VALUES (?, ?)");
    query.addBindValue(userId);
    query.addBindValue(targetId);

    if (!query.exec()) {
        UserLogger::log(Error, "addLike failed: " + query.lastError().text());
        return false;
    }

    return true;
}

bool DatabaseManager::removeLike(int userId, int targetId) {
    QSqlQuery query(m_db);

    query.prepare("DELETE FROM likes WHERE user_id=? AND target_id=?");
    query.addBindValue(userId);
    query.addBindValue(targetId);

    return query.exec();
}

bool DatabaseManager::hasUserLiked(int userId, int targetId) const {
    QSqlQuery query(m_db);

    query.prepare("SELECT 1 FROM likes WHERE user_id=? AND target_id=?");
    query.addBindValue(userId);
    query.addBindValue(targetId);

    if (!query.exec())
        return false;

    return query.next();
}

bool DatabaseManager::isMutualLike(int userId, int targetId) const {
    if (!hasUserLiked(userId, targetId))
        return false;

    if (!hasUserLiked(targetId, userId))
        return false;

    return true;
}

QList<int> DatabaseManager::getMatches(int userId) const {
    QList<int> result;
    QSqlQuery query(m_db);

    QString sql =
        "SELECT target_id "
        "FROM likes "
        "WHERE user_id=? "
        "AND target_id IN (SELECT user_id FROM likes WHERE target_id=?)";

    query.prepare(sql);
    query.addBindValue(userId);
    query.addBindValue(userId);

    if (!query.exec())
        return result;

    while (query.next()) {
        result.append(query.value(0).toInt());
    }

    return result;
}
