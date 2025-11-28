#include "TagManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include "UserLogger.h"

TagManager::TagManager(DatabaseManager* db) : m_db(db) { }

bool TagManager::addTag(int userId, const QString& tag)
{
    if (userId <= 0 || tag.isEmpty()) return false;

    if (!m_db->transaction()) {
        UserLogger::log(Error, "Failed to start transaction (addTag): " + m_db->lastError().text());
        return false;
    }

    QSqlQuery q;
    q.prepare("INSERT OR IGNORE INTO user_tags (user_id, tag) VALUES (?, ?)");
    q.addBindValue(userId);
    q.addBindValue(tag);

    if (!q.exec()) {
        UserLogger::log(Error, "Failed to add tag: " + q.lastError().text());
        m_db->rollback();
        return false;
    }

    if (!m_db->commit()) {
        UserLogger::log(Error, "Failed to commit addTag transaction: " + m_db->lastError().text());
        m_db->rollback();
        return false;
    }

    return true;
}

bool TagManager::removeTag(int userId, const QString& tag)
{
    if (userId <= 0 || tag.isEmpty()) return false;

    QSqlQuery q;
    q.prepare("DELETE FROM user_tags WHERE user_id = ? AND tag = ?");
    q.addBindValue(userId);
    q.addBindValue(tag);

    if (!q.exec()) {
        UserLogger::log(Error, "Failed to remove tag: " + q.lastError().text());
        return false;
    }
    return true;
}

QList<QString> TagManager::getTagsForUser(int userId) const
{
    QList<QString> tags;
    QSqlQuery q;
    q.prepare("SELECT tag FROM user_tags WHERE user_id = ?");
    q.addBindValue(userId);

    if (!q.exec()) {
        UserLogger::log(Error, "Failed to get tags for user: " + q.lastError().text());
        return tags;
    }

    while (q.next()) {
        tags.append(q.value(0).toString());
    }
    return tags;
}
