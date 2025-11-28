#pragma once
#include <QString>
#include <QList>
#include "DatabaseManager.h"

/**
 * @brief Клас для роботи з тегами користувачів
 */
class TagManager
{
public:
    explicit TagManager(DatabaseManager* db);

    // Додає тег користувачу
    bool addTag(int userId, const QString& tag);

    // Видаляє тег користувача
    bool removeTag(int userId, const QString& tag);

    // Повертає всі теги користувача
    QList<QString> getTagsForUser(int userId) const;

private:
    DatabaseManager* m_db;
};
