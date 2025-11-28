#ifndef LIKESMANAGER_H
#define LIKESMANAGER_H

#include "DatabaseManager.h"
#include <QList>

/**
 * @brief Клас LikesManager
 * 
 * відповідає за логіку "лайків" та пошуку взаємних симпатій
 */
class LikesManager {
public:
    LikesManager(DatabaseManager* db);

    // лайки
    bool addLike(int userId, int targetId);
    bool isMutualLike(int userId, int targetId) const;
    
    // метчі
    QList<UserProfile> getMatchesForUser(int userId);

private:
    DatabaseManager* m_db;
};

#endif // LIKESMANAGER_H
