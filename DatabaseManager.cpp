#include "DatabaseManager.h"
#include "UserLogger.h"
#include <QSqlQuery>
#include <QSqlError>

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
        "email VARCHAR(100)"
        ");";

    if (query.exec(createSql)) {
        UserLogger::log(Info, "Table 'users' verified/created successfully.");
        return true;
    } else {
        UserLogger::log(Error, "Failed to create 'users' table: " + query.lastError().text());
        return false;
    }
}

bool DatabaseManager::saveProfile(const UserProfile &profile) {
    if (!m_db.transaction()) {
        UserLogger::log(Error, "Failed to start transaction: " + m_db.lastError().text());
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO users (name, age, city, bio, phone, email) "
                  "VALUES (?, ?, ?, ?, ?, ?)");

    query.addBindValue(profile.getName());
    query.addBindValue(profile.getAge());
    query.addBindValue(profile.getCity());
    query.addBindValue(profile.getBio());
    query.addBindValue(profile.getContactInfo().getPhone());
    query.addBindValue(profile.getContactInfo().getEmail());

    if (query.exec()) {
        if (m_db.commit()) {
            UserLogger::log(Info, "Profile saved and transaction committed for: " + profile.getName());
            return true;
        } else {
            UserLogger::log(Error, "Failed to commit transaction: " + m_db.lastError().text());
            m_db.rollback();
            return false;
        }
    } else {
        UserLogger::log(Error, "Failed to save profile: " + query.lastError().text());
        m_db.rollback();
        return false;
    }
}

bool DatabaseManager::profileExists(const QString &email) const {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE email = ?");
    query.addBindValue(email);

    if (query.exec()) {
        if (query.next()) {
            // query.value(0).toInt() повертає кількість знайдених записів.
            // Якщо вона більша за 0, значить профіль існує.
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
    query.prepare("UPDATE users SET name = ?, age = ?, city = ?, bio = ?, phone = ?, email = ? "
                  "WHERE id = ?");

    query.addBindValue(profile.getName());
    query.addBindValue(profile.getAge());
    query.addBindValue(profile.getCity());
    query.addBindValue(profile.getBio());
    query.addBindValue(profile.getContactInfo().getPhone());
    query.addBindValue(profile.getContactInfo().getEmail());
    query.addBindValue(profile.getId());

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
        // Перевіряємо, чи був насправді видалений рядок
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

