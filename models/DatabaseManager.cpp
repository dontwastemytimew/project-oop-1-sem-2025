#include "DatabaseManager.h"
#include "UserLogger.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSettings>
#include <QFile>
#include <QDir>
#include "Preference.h"
#include <QSqlTableModel>
#include <QMap>
#include <QVariant>

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

    // USERS
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

    // LIKES
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

    // USER TAGS
    QString createTagsSql =
        "CREATE TABLE IF NOT EXISTS user_tags ("
        "user_id INTEGER NOT NULL, "
        "tag VARCHAR(50) NOT NULL, "
        "PRIMARY KEY (user_id, tag),"
        "FOREIGN KEY (user_id) REFERENCES users(id)"
        ");";

    if (!query.exec(createTagsSql)) {
        UserLogger::log(Error, "Failed to create 'user_tags' table.");
        return false;
    }

    query.exec("CREATE INDEX IF NOT EXISTS idx_tags_user ON user_tags(user_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_users_age ON users(age)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_likes_user ON likes(user_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_likes_target ON likes(target_id)");

    UserLogger::log(Info, "Tables 'users' and 'likes' verified/created successfully.");
    return true;
}


// ЛАЙКИ / МЕТЧІ
bool DatabaseManager::addLike(int userId, int targetId) {
    if (userId <= 0 || targetId <= 0 || userId == targetId) return false;

    if (!m_db.transaction()) {
        UserLogger::log(Error, "AddLike: Failed to start transaction.");
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("INSERT OR IGNORE INTO likes (user_id, target_id) VALUES (?, ?)");
    query.addBindValue(userId);
    query.addBindValue(targetId);

    if (query.exec()) {
        m_db.commit();
        return true;
    } else {
        m_db.rollback();
        UserLogger::log(Error, "addLike failed: " + query.lastError().text());
        return false;
    }
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

bool DatabaseManager::isMutualLike(int user1, int user2) const {
    return hasUserLiked(user1, user2) && hasUserLiked(user2, user1);
}

QList<int> DatabaseManager::getMutualMatchIds(int currentUserId) {
    QList<int> matchIds;
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT T1.target_id FROM likes AS T1 "
        "INNER JOIN likes AS T2 ON T1.target_id = T2.user_id "
        "WHERE T1.user_id = :currentId AND T2.target_id = :currentId"
    );
    query.bindValue(":currentId", currentUserId);

    if (query.exec()) {
        while (query.next()) {
            matchIds.append(query.value(0).toInt());
        }
    } else {
        UserLogger::log(Error, "Failed to get mutual match IDs: " + query.lastError().text());
    }

    return matchIds;
}


// SAVE PROFILE
int DatabaseManager::saveProfile(const UserProfile &profile) {

    if (profile.getName().isEmpty() || profile.getContactInfo().getEmail().isEmpty() || !profile.getContactInfo().getEmail().contains('@') || profile.getAge() < 18) {
        UserLogger::log(Warning, "Save failed: Invalid or missing required fields.");
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

// LOAD PROFILE BY EMAIL
bool DatabaseManager::loadProfileByEmail(const QString &email, UserProfile &profile) {
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE email = ?");
    query.addBindValue(email);

    if (!query.exec()) { UserLogger::log(Error, "Select by email failed: " + query.lastError().text()); return false; }
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

// UPDATE PROFILE
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
    if (profileId <= 0) return false;
    if (!m_db.transaction()) return false;

    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE id=?");
    query.addBindValue(profileId);

    if (query.exec()) {
        m_db.commit(); 
        return true;
    } else {
        m_db.rollback();
        return false;
    }
}

bool DatabaseManager::setProfileHidden(int profileId, bool isHidden) {
    if (profileId <= 0) return false;
    if (!m_db.transaction()) return false;

    QSqlQuery query;
    query.prepare("UPDATE users SET is_hidden=? WHERE id=?");
    query.addBindValue(isHidden);
    query.addBindValue(profileId);

    if (query.exec()) {
        m_db.commit(); 
        return true;
    } else {
        m_db.rollback();
        return false;
    }
}

// LOAD CURRENT USER
bool DatabaseManager::getCurrentUserProfile(UserProfile &profile) {
    QSettings settings("DatingAgency", "Match++");

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

// GET ALL PROFILES
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

// FILTER BY PREFERENCES
QList<UserProfile> DatabaseManager::getProfilesByCriteria(const Preference &prefs, int currentUserId) {
    QList<UserProfile> profiles;
    QSqlQuery query(m_db);

    // Додаємо AND id != :currentId для виключення себе
    QString sql = "SELECT * FROM users WHERE is_hidden = 0 AND id != :currentUserId";
    QMap<QString, QVariant> bind;

    // Прив'язуємо ID користувача
    bind[":currentUserId"] = currentUserId;

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

// BULK INSERT
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

int DatabaseManager::countUsers() {
    QSqlQuery query("SELECT COUNT(*) FROM users", m_db);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

// AUTOCOMPLETE МІСТ
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

bool DatabaseManager::removeLike(int userId, int targetId) {
    if (userId <= 0 || targetId <= 0) {
        UserLogger::log(Warning, "RemoveLike failed: Invalid user IDs.");
        return false;
    }

    if (!m_db.transaction()) { // <--- ТРАНЗАКЦІЯ ДОДАНО
        UserLogger::log(Error, "RemoveLike: Failed to start transaction.");
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM likes WHERE user_id=? AND target_id=?");
    query.addBindValue(userId);
    query.addBindValue(targetId);

    if (query.exec()) {
        if (m_db.commit()) {
            UserLogger::log(Info, QString("Like removed: User %1 removed like from Target %2.")
                                 .arg(userId).arg(targetId));
            return true;
        } else {
            UserLogger::log(Error, "RemoveLike: Failed to commit transaction.");
            m_db.rollback();
            return false;
        }
    } else {
        UserLogger::log(Error, "RemoveLike failed: " + query.lastError().text());
        m_db.rollback();
        return false;
    }
}


bool DatabaseManager::loadProfileById(int userId, UserProfile& profile)
{
    if (!m_db.isOpen()) {
        qDebug() << "Database not open. Cannot load profile by ID.";
        return false;
    }

    QSqlQuery query(m_db);
    // ВИПРАВЛЕНО: Таблиця users, ключ id, прибрано неіснуючий 'password'
    query.prepare("SELECT * FROM users WHERE id = :id");
    query.bindValue(":id", userId);

    if (!query.exec()) {
        qDebug() << "loadProfileById failed:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
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

    qDebug() << "loadProfileById: No profile found for ID" << userId;
    return false;
}


// для AdminPanel
QSqlTableModel* DatabaseManager::getUsersModel(QObject* parent) {
    QSqlTableModel* model = new QSqlTableModel(parent, m_db);
    model->setTable("users");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Ім'я"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Вік"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Місто"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Біо"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Телефон"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Email"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Стать"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("Орієнтація"));
    model->setHeaderData(9, Qt::Horizontal, QObject::tr("Прихований"));

    model->select();
    return model;
}

// для StatsDialog
QMap<QString, int> DatabaseManager::getGenderStatistics() {
    QMap<QString, int> stats;
    QSqlQuery query("SELECT gender, COUNT(*) FROM users WHERE is_hidden=0 GROUP BY gender", m_db);
    while (query.next()) {
        QString gender = query.value(0).toString();
        if (gender.isEmpty()) gender = "Невідомо";
        stats.insert(gender, query.value(1).toInt());
    }
    return stats;
}

// для StatsDialog
QMap<QString, int> DatabaseManager::getCityStatistics() {
    QMap<QString, int> stats;
    QSqlQuery query("SELECT city, COUNT(*) FROM users WHERE is_hidden=0 GROUP BY city ORDER BY COUNT(*) DESC LIMIT 5", m_db);
    while (query.next()) {
        stats.insert(query.value(0).toString(), query.value(1).toInt());
    }
    return stats;
}

// для StatsDialog
QMap<QString, int> DatabaseManager::getAgeStatistics() {
    QMap<QString, int> stats;
    stats["18-25"] = 0;
    stats["26-35"] = 0;
    stats["36-50"] = 0;
    stats["50+"] = 0;

    QSqlQuery query("SELECT age FROM users WHERE is_hidden=0", m_db);
    while (query.next()) {
        int age = query.value(0).toInt();
        if (age <= 25) stats["18-25"]++;
        else if (age <= 35) stats["26-35"]++;
        else if (age <= 50) stats["36-50"]++;
        else stats["50+"]++;
    }
    return stats;
}

// Реалізація getProfilesByIds: Завантажує профілі за списком ID
QList<UserProfile> DatabaseManager::getProfilesByIds(const QList<int> &ids) {
    QList<UserProfile> profiles;
    if (ids.isEmpty()) return profiles;

    QSqlQuery query;
    // Створення рядка "id IN (?, ?, ?...)" для ефективного SQL-запиту
    QString idList = "(";
    for (int i = 0; i < ids.size(); ++i) {
        idList += "?";
        if (i < ids.size() - 1) idList += ",";
    }
    idList += ")";

    query.prepare("SELECT * FROM users WHERE id IN " + idList);

    // Додаємо ID як bindValue
    for (int id : ids) {
        query.addBindValue(id);
    }

    if (query.exec()) {
        while (query.next()) {
            UserProfile profile;

            // Зчитування всіх полів
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
    } else {
        UserLogger::log(Error, "Failed to get profiles by IDs: " + query.lastError().text());
    }
    return profiles;
}

void DatabaseManager::saveCurrentUserId(int userId) {
    QSettings settings("DatingAgency", "Match++");
    settings.setValue("currentUserId", userId);
    settings.sync();
    UserLogger::log(Info, "Saved current user ID to QSettings: " + QString::number(userId));
}

int DatabaseManager::loadCurrentUserId() const {
    QSettings settings("DatingAgency", "Match++");
    int userId = settings.value("currentUserId", -1).toInt();
    UserLogger::log(Info, "Loaded current user ID from QSettings: " + QString::number(userId));
    return userId;
}

bool DatabaseManager::addTag(int userId, const QString& tag)
{
    if (userId <= 0 || tag.isEmpty()) return false;

    if (!m_db.transaction()) return false;

    QSqlQuery query;
    query.prepare("INSERT OR IGNORE INTO user_tags (user_id, tag) VALUES (?, ?)");
    query.addBindValue(userId);
    query.addBindValue(tag);

    if (!query.exec()) {
        m_db.rollback();
        UserLogger::log(Error, "Failed to add tag: " + query.lastError().text());
        return false;
    }

    return m_db.commit();
}

bool DatabaseManager::removeTag(int userId, const QString& tag)
{
    if (userId <= 0 || tag.isEmpty()) return false;

    // --- КРИТИЧНИЙ ФІКС: ДОДАНО ТРАНЗАКЦІЮ ---
    if (!m_db.transaction()) return false;

    QSqlQuery query;
    query.prepare("DELETE FROM user_tags WHERE user_id = ? AND tag = ?");
    query.addBindValue(userId);
    query.addBindValue(tag);

    if (!query.exec()) {
        m_db.rollback();
        UserLogger::log(Error, "Failed to remove tag: " + query.lastError().text());
        return false;
    }

    return m_db.commit(); // <-- COMMIT
}

QList<QString> DatabaseManager::getTagsForUser(int userId) const
{
    QList<QString> tags;
    QSqlQuery query(m_db); // Використовуйте конструктор з m_db для const-методу
    query.prepare("SELECT tag FROM user_tags WHERE user_id = ?");
    query.addBindValue(userId);

    if (!query.exec()) {
        UserLogger::log(Error, "Failed to get tags for user: " + query.lastError().text());
        return tags;
    }

    while (query.next()) {
        tags.append(query.value(0).toString());
    }
    return tags;
}

bool DatabaseManager::removeAllTags(int userId) {
    if (userId <= 0) return false;

    if (!m_db.transaction()) return false;

    QSqlQuery query;
    query.prepare("DELETE FROM user_tags WHERE user_id = ?");
    query.addBindValue(userId);

    if (query.exec()) {
        if (m_db.commit()) {
            UserLogger::log(Info, QString("All tags removed for User ID %1.").arg(userId));
            return true;
        } else {
            m_db.rollback();
            return false;
        }
    } else {
        UserLogger::log(Error, "Failed to remove all tags: " + query.lastError().text());
        m_db.rollback();
        return false;
    }
}


bool DatabaseManager::addMatch(int userId, int targetId) {
    if (isMutualLike(userId, targetId)) {
        UserLogger::log(Info, QString("MATCH recorded for U%1 and T%2.").arg(userId).arg(targetId));
        return true;
    }
    return false;
}