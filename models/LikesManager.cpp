#include "LikesManager.h"
#include "UserLogger.h"
#include <QSqlQuery>
#include <QSqlError>

LikesManager::LikesManager(DatabaseManager* db)
    : m_db(db) {}

// Додаємо лайк від userId до targetId
bool LikesManager::addLike(int userId, int targetId) {
    if (!m_db || userId <= 0 || targetId <= 0) return false;

    if (!m_db->m_db.transaction()) {
        UserLogger::log(Error, "Failed to start transaction (addLike): " + m_db->m_db.lastError().text());
        return false;
    }

    QSqlQuery q(m_db->m_db);
    q.prepare("INSERT OR IGNORE INTO likes (user_id, target_id) VALUES (?, ?)");
    q.addBindValue(userId);
    q.addBindValue(targetId);

    if (!q.exec()) {
        UserLogger::log(Error, "Failed to add like: " + q.lastError().text());
        m_db->m_db.rollback();
        return false;
    }

    if (!m_db->m_db.commit()) {
        UserLogger::log(Error, "Failed to commit addLike transaction: " + m_db->m_db.lastError().text());
        m_db->m_db.rollback();
        return false;
    }

    return true;
}

// Перевірка взаємного лайку
bool LikesManager::isMutualLike(int userId, int targetId) const {
    if (!m_db) return false;

    QSqlQuery q(m_db->m_db);
    q.prepare("SELECT COUNT(*) FROM likes WHERE (user_id = ? AND target_id = ?) OR (user_id = ? AND target_id = ?)");
    q.addBindValue(userId);
    q.addBindValue(targetId);
    q.addBindValue(targetId);
    q.addBindValue(userId);

    if (!q.exec()) {
        UserLogger::log(Error, "Failed to check mutual like: " + q.lastError().text());
        return false;
    }

    if (q.next()) {
        return q.value(0).toInt() >= 2; // обидва напрямки є
    }

    return false;
}

// Повертає список взаємних лайків (метчів)
QList<UserProfile> LikesManager::getMatchesForUser(int userId) {
    QList<UserProfile> results;
    if (!m_db) return results;

    QSqlQuery q(m_db->m_db);
    QString sql = R"sql(
        SELECT u.* FROM users u
        JOIN likes l1 ON l1.target_id = u.id
        JOIN likes l2 ON l2.user_id = u.id
        WHERE l1.user_id = ? AND l2.target_id = ?
    )sql";

    q.prepare(sql);
    q.addBindValue(userId);
    q.addBindValue(userId);

    if (!q.exec()) {
        UserLogger::log(Error, "Failed to get matches for user: " + q.lastError().text());
        return results;
    }

    while (q.next()) {
        UserProfile p;
        p.setId(q.value("id").toInt());
        p.setName(q.value("name").toString());
        p.setAge(q.value("age").toInt());
        p.setCity(q.value("city").toString());
        p.setBio(q.value("bio").toString());
        ContactInfo c(q.value("phone").toString(), q.value("email").toString());
        p.setContactInfo(c);
        p.setGender(q.value("gender").toString());
        p.setOrientation(q.value("orientation").toString());
        results.append(p);
    }

    return results;
}
