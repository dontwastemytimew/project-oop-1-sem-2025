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
    // Перевіряємо, що email коректний (просто перевірка на '@')
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
    query.prepare("INSERT INTO users (name, age, city, bio, phone, email, gender, orientation) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(profile.getName());
    query.addBindValue(profile.getAge());
    query.addBindValue(profile.getCity());
    query.addBindValue(profile.getBio());
    query.addBindValue(profile.getContactInfo().getPhone());
    query.addBindValue(profile.getContactInfo().getEmail());
    query.addBindValue(profile.getGender());
    query.addBindValue(profile.getOrientation());

    if (query.exec()) {
        // ОТРИМУЄМО НОВИЙ ID
        int newId = query.lastInsertId().toInt();

        if (m_db.commit()) {
            UserLogger::log(Info, "Profile saved and transaction committed. New ID: " + QString::number(newId));
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

bool DatabaseManager::profileExists(const QString &email) const {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE email = ?");
    query.addBindValue(email);

    if (query.exec()) {
        if (query.next()) {
            return query.value(0).toInt() > 0;
        }
    } else {
        UserLogger::log(Error, "Failed to check if profile exists: " + query.lastError().text());
    }
    return false;
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

            ContactInfo contacts(
                query.value("phone").toString(),
                query.value("email").toString()
            );
            profile.setContactInfo(contacts);

            UserLogger::log(Info, "Profile loaded successfully for: " + profile.getName());
            return true;
        } else {
            return false;
        }
    } else {
        UserLogger::log(Error, "Failed to load profile by email: " + query.lastError().text());
        return false;
    }
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
    query.prepare("UPDATE users SET name = ?, age = ?, city = ?, bio = ?, phone = ?, email = ?, "
                  "gender = ?, orientation = ? "
                  "WHERE id = ?");

    query.addBindValue(profile.getName());
    query.addBindValue(profile.getAge());
    query.addBindValue(profile.getCity());
    query.addBindValue(profile.getBio());
    query.addBindValue(profile.getContactInfo().getPhone());
    query.addBindValue(profile.getContactInfo().getEmail());
    query.addBindValue(profile.getId());
    query.addBindValue(profile.getGender());
    query.addBindValue(profile.getOrientation());

    if (query.exec()) {
        if (m_db.commit()) {
            UserLogger::log(Info, "Profile updated successfully for: " + profile.getName());
            return true;
        } else {
            UserLogger::log(Error, "Failed to commit update transaction: " + m_db.lastError().text());
            m_db.rollback();
            return false;
        }
    } else {
        UserLogger::log(Error, "Failed to update profile: " + query.lastError().text());
        m_db.rollback();
        return false;
    }
}

bool DatabaseManager::deleteProfile(int profileId) {
    if (profileId <= 0) {
        UserLogger::log(Warning, "Cannot delete profile: Invalid ID.");
        return false;
    }

    if (!m_db.transaction()) {
        UserLogger::log(Error, "Failed to start transaction for delete: " + m_db.lastError().text());
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE id = ?");
    query.addBindValue(profileId);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            if (m_db.commit()) {
                UserLogger::log(Info, QString("Profile with ID %1 deleted successfully.").arg(profileId));
                return true;
            } else {
                UserLogger::log(Error, "Failed to commit delete transaction: " + m_db.lastError().text());
                m_db.rollback();
                return false;
            }
        } else {
            UserLogger::log(Warning, QString("No profile found with ID %1 to delete.").arg(profileId));
            m_db.rollback();
            return false;
        }
    } else {
        UserLogger::log(Error, "Failed to delete profile: " + query.lastError().text());
        m_db.rollback();
        return false;
    }
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

    // Якщо "Не важливо" або пусте -- не додаємо в запит
    if (!prefs.getGender().isEmpty() && prefs.getGender() != "Не важливо") {
        sql += " AND gender = :gender";
        bindValues[":gender"] = prefs.getGender();
    }

    if (!prefs.getOrientation().isEmpty() && prefs.getOrientation() != "Не важливо") {
        sql += " AND orientation = :orientation";
        bindValues[":orientation"] = prefs.getOrientation();
    }

    query.prepare(sql);

    for (auto it = bindValues.constBegin(); it != bindValues.constEnd(); ++it) {
        query.bindValue(it.key(), it.value());
    }

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

        ContactInfo contacts(
            query.value("phone").toString(),
            query.value("email").toString()
        );
        profile.setContactInfo(contacts);

        profiles.append(profile);
    }

    UserLogger::log(Info, QString("Found %1 profiles matching criteria.").arg(profiles.count()));
    return profiles;
}

bool DatabaseManager::setProfileHidden(int profileId, bool isHidden) {
    if (profileId <= 0) return false;

    if (!m_db.transaction()) {
        UserLogger::log(Error, "Failed to start transaction (setHidden): " + m_db.lastError().text());
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE users SET is_hidden = ? WHERE id = ?");
    query.addBindValue(isHidden);
    query.addBindValue(profileId);

    if (query.exec()) {
        m_db.commit();
        UserLogger::log(Info, QString("Profile %1 hidden status set to: %2").arg(profileId).arg(isHidden));
        return true;
    } else {
        UserLogger::log(Error, "Failed to update hidden status: " + query.lastError().text());
        m_db.rollback();
        return false;
    }
}

bool DatabaseManager::getCurrentUserProfile(UserProfile &profile) {
    QSettings settings("DatingAgency", "TitleApp");

    //Зчитуємо збережений ID користувача
    int profileId = settings.value("current_user_id", -1).toInt();

    UserLogger::log(Info, QString("DEBUG LOAD START: QSettings returned ID: %1").arg(profileId));

    if (profileId == -1 || profileId == 0) {
        UserLogger::log(Info, "No saved session ID found.");
        return false;
    }

    // Використовуємо ЗБЕРЕЖЕНИЙ ID для пошуку
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE id = ?");
    query.addBindValue(profileId);

    if (query.exec() && query.next()) {
        profile.setId(query.value("id").toInt());
        profile.setName(query.value("name").toString());
        profile.setAge(query.value("age").toInt());
        profile.setCity(query.value("city").toString());
        profile.setBio(query.value("bio").toString());
        profile.setGender(query.value("gender").toString());
        profile.setOrientation(query.value("orientation").toString());

        ContactInfo contacts(
            query.value("phone").toString(),
            query.value("email").toString()
        );
        profile.setContactInfo(contacts);
        UserLogger::log(Info, QString("Session loaded for ID: %1").arg(profileId));
        return true;
    }

    UserLogger::log(Warning, QString("Saved ID %1 not found in DB.").arg(profileId));
    return false;
}

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

QMap<QString, int> DatabaseManager::getCityStatistics() {
    QMap<QString, int> stats;
    // Топ-5 міст
    QSqlQuery query("SELECT city, COUNT(*) FROM users WHERE is_hidden=0 GROUP BY city ORDER BY COUNT(*) DESC LIMIT 5");
    while (query.next()) {
        stats.insert(query.value(0).toString(), query.value(1).toInt());
    }
    return stats;
}

QMap<QString, int> DatabaseManager::getGenderStatistics() {
    QMap<QString, int> stats;
    // Розподіл за статтю
    QSqlQuery query("SELECT gender, COUNT(*) FROM users WHERE is_hidden=0 GROUP BY gender");
    while (query.next()) {
        QString gender = query.value(0).toString();
        if (gender.isEmpty()) gender = "Невідомо";

        stats.insert(gender, query.value(1).toInt());
    }
    return stats;
}

QMap<QString, int> DatabaseManager::getAgeStatistics() {
    QMap<QString, int> stats;
    stats["18-25"] = 0;
    stats["26-35"] = 0;
    stats["36-50"] = 0;
    stats["50+"] = 0;

    QSqlQuery query("SELECT age FROM users WHERE is_hidden=0");
    while (query.next()) {
        int age = query.value(0).toInt();
        if (age <= 25) stats["18-25"]++;
        else if (age <= 35) stats["26-35"]++;
        else if (age <= 50) stats["36-50"]++;
        else stats["50+"]++;
    }
    return stats;
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

        ContactInfo contacts(
            query.value("phone").toString(),
            query.value("email").toString()
        );
        profile.setContactInfo(contacts);

        profiles.append(profile);
    }
    return profiles;
}

bool DatabaseManager::saveProfileBulk(const QList<UserProfile> &profiles) {
    if (profiles.isEmpty()) return true;

    if (!m_db.transaction()) {
        UserLogger::log(Error, "Bulk: Failed to start transaction.");
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO users (name, age, city, bio, phone, email, gender, orientation) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

    for (const UserProfile& profile : profiles) {
        query.addBindValue(profile.getName());
        query.addBindValue(profile.getAge());
        query.addBindValue(profile.getCity());
        query.addBindValue(profile.getBio());
        query.addBindValue(profile.getContactInfo().getPhone());
        query.addBindValue(profile.getContactInfo().getEmail());
        query.addBindValue(profile.getGender());
        query.addBindValue(profile.getOrientation());

        if (!query.exec()) {
            UserLogger::log(Error, "Bulk: Failed to exec query. Rolling back.");
            m_db.rollback();
            return false;
        }
    }

    if (m_db.commit()) {
        UserLogger::log(Info, QString("Bulk: Successfully inserted %1 profiles.").arg(profiles.count()));
        return true;
    } else {
        UserLogger::log(Error, "Bulk: Failed to commit transaction.");
        m_db.rollback();
        return false;
    }
}

int DatabaseManager::countUsers() {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users");

    if (query.exec()) {
        if (query.next()) {
            return query.value(0).toInt();
        }
    } else {
        UserLogger::log(Error, "Failed to count users: " + query.lastError().text());
    }
    return 0;
}