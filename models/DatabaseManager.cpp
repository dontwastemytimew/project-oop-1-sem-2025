#include "DatabaseManager.h"
#include "UserLogger.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSettings>
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

    QString createSql =
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
        "photo_path TEXT,"            -- ★ ДОДАНО
        "is_hidden BOOLEAN DEFAULT 0"
        ");";

    if (query.exec(createSql)) {
        UserLogger::log(Info, "Table 'users' verified/created successfully.");
        return true;
    } else {
        UserLogger::log(Error, "Failed to create 'users' table: " + query.lastError().text());
        return false;
    }
}

int DatabaseManager::saveProfile(const UserProfile &profile) {
    if (profile.getName().isEmpty()) {
        UserLogger::log(Warning, "Save failed: Name cannot be empty.");
        return false;
    }
    if (profile.getContactInfo().getEmail().isEmpty()) {
        UserLogger::log(Warning, "Save failed: Email cannot be empty.");
        return false;
    }
    if (!profile.getContactInfo().getEmail().contains('@')) {
        UserLogger::log(Warning, "Save failed: Email is not valid.");
        return false;
    }
    if (profile.getAge() < 18) {
        UserLogger::log(Warning, "Save failed: User must be 18 or older.");
        return false;
    }

    if (!m_db.transaction()) {
        UserLogger::log(Error, "Failed to start transaction: " + m_db.lastError().text());
        return -1;
    }

    QSqlQuery query;

    query.prepare(
        "INSERT INTO users (name, age, city, bio, phone, email, gender, orientation, photo_path) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
        ); // ★ ДОДАНО photo_path у INSERT

    query.addBindValue(profile.getName());
    query.addBindValue(profile.getAge());
    query.addBindValue(profile.getCity());
    query.addBindValue(profile.getBio());
    query.addBindValue(profile.getContactInfo().getPhone());
    query.addBindValue(profile.getContactInfo().getEmail());
    query.addBindValue(profile.getGender());
    query.addBindValue(profile.getOrientation());
    query.addBindValue(profile.getPhotoPath()); // ★ ДОДАНО

    if (query.exec()) {
        int newId = query.lastInsertId().toInt();

        if (m_db.commit()) {
            UserLogger::log(Info, "Profile saved. ID = " + QString::number(newId));
            return newId;
        } else {
            UserLogger::log(Error, "Failed to commit transaction.");
            m_db.rollback();
            return -1;
        }
    } else {
        UserLogger::log(Error, "Failed to save profile: " + query.lastError().text());
        m_db.rollback();
        return -1;
    }
}

bool DatabaseManager::loadProfileByEmail(const QString &email, UserProfile &profile) {
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE email = ?");
    query.addBindValue(email);

    if (query.exec()) {
        if (query.next()) {

            profile.setId(query.value("id").toInt());
            profile.setName(query.value("name").toString());
            profile.setAge(query.value("age").toInt());
            profile.setCity(query.value("city").toString());
            profile.setBio(query.value("bio").toString());
            profile.setGender(query.value("gender").toString());
            profile.setOrientation(query.value("orientation").toString());
            profile.setPhotoPath(query.value("photo_path").toString());   // ★ ДОДАНО

            ContactInfo contacts(
                query.value("phone").toString(),
                query.value("email").toString()
                );
            profile.setContactInfo(contacts);

            UserLogger::log(Info, "Profile loaded for: " + profile.getName());
            return true;
        }
    } else {
        UserLogger::log(Error, "Failed to load profile by email: " + query.lastError().text());
    }
    return false;
}

bool DatabaseManager::updateProfile(const UserProfile &profile) {
    if (profile.getId() == -1) {
        UserLogger::log(Warning, "Cannot update profile: Invalid ID (-1).");
        return false;
    }

    if (!m_db.transaction()) {
        UserLogger::log(Error, "Failed to start transaction for update: " + m_db.lastError().text());
        return false;
    }

    QSqlQuery query;
    query.prepare(
        "UPDATE users "
        "SET name = ?, age = ?, city = ?, bio = ?, phone = ?, email = ?, "
        "gender = ?, orientation = ?, photo_path = ? "
        "WHERE id = ?"
        ); // ★ ДОДАНО photo_path = ?

    query.addBindValue(profile.getName());
    query.addBindValue(profile.getAge());
    query.addBindValue(profile.getCity());
    query.addBindValue(profile.getBio());
    query.addBindValue(profile.getContactInfo().getPhone());
    query.addBindValue(profile.getContactInfo().getEmail());
    query.addBindValue(profile.getGender());
    query.addBindValue(profile.getOrientation());
    query.addBindValue(profile.getPhotoPath()); // ★ ДОДАНО
    query.addBindValue(profile.getId());

    if (query.exec()) {
        if (m_db.commit()) {
            UserLogger::log(Info, "Profile updated successfully: " + profile.getName());
            return true;
        }
    }

    UserLogger::log(Error, "Failed to update profile: " + query.lastError().text());
    m_db.rollback();
    return false;
}

QList<UserProfile> DatabaseManager::getAllProfiles() {
    QList<UserProfile> profiles;
    QSqlQuery query(m_db);

    QString sql = "SELECT * FROM users WHERE is_hidden = 0";

    if (!query.exec(sql)) {
        UserLogger::log(Error, "Failed to get all profiles: " + query.lastError().text());
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
        profile.setPhotoPath(query.value("photo_path").toString()); // ★ ДОДАНО

        ContactInfo contacts(
            query.value("phone").toString(),
            query.value("email").toString()
            );
        profile.setContactInfo(contacts);

        profiles.append(profile);
    }

    return profiles;
}

QList<UserProfile> DatabaseManager::getProfilesByCriteria(const Preference &prefs) {
    QList<UserProfile> profiles;
    QSqlQuery query(m_db);

    QString sql = "SELECT * FROM users WHERE 1=1 AND is_hidden = 0";
    QMap<QString, QVariant> bindValues;

    if (prefs.getMinAge() > 0) {
        sql += " AND age >= :minAge";
        bindValues[":minAge"] = prefs.getMinAge();
    }
    if (prefs.getMaxAge() > 0) {
        sql += " AND age <= :maxAge";
        bindValues[":maxAge"] = prefs.getMaxAge();
    }
    if (!prefs.getCity().isEmpty()) {
        sql += " AND city LIKE :city";
        bindValues[":city"] = "%" + prefs.getCity() + "%";
    }
    if (!prefs.getGender().isEmpty() && prefs.getGender() != "Не важливо") {
        sql += " AND gender = :gender";
        bindValues[":gender"] = prefs.getGender();
    }
    if (!prefs.getOrientation().isEmpty() && prefs.getOrientation() != "Не важливо") {
        sql += " AND orientation = :orientation";
        bindValues[":orientation"] = prefs.getOrientation();
    }

    query.prepare(sql);
    for (auto it = bindValues.constBegin(); it != bindValues.constEnd(); ++it)
        query.bindValue(it.key(), it.value());

    if (!query.exec()) {
        UserLogger::log(Error, "Failed to get profiles by criteria: " + query.lastError().text());
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
        profile.setPhotoPath(query.value("photo_path").toString()); // ★ ДОДАНО

        ContactInfo contacts(
            query.value("phone").toString(),
            query.value("email").toString()
            );
        profile.setContactInfo(contacts);

        profiles.append(profile);
    }

    return profiles;
}
