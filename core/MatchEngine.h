#ifndef MATCHENGINE_H
#define MATCHENGINE_H

#include "Preference.h"
#include "UserProfile.h"
#include "DatabaseManager.h"

/**
 * @brief MatchEngine class
 *
 * Розрахунок сумісності профілів на основі:
 *  - статі / орієнтації
 *  - віку
 *  - міста
 *  - тегів (TODO)
 *  - формули балів
 */
class MatchEngine {
public:

    explicit MatchEngine(DatabaseManager* dbManager);

    /// Проста фільтрація профілів за Preference
    static bool calculateMatch(const UserProfile &profile, const Preference &prefs);

    /// Повертає true, якщо профілі п1 і п2 відповідають один одному
    bool isCompatible(const UserProfile& p1, const UserProfile& p2) const;

    /// Обчислює сумарний бал сумісності
    int compatibilityScore(const UserProfile& p1, const UserProfile& p2) const;

    /// Обчислює % сумісності (0–100)
    int compatibilityPercent(const UserProfile& p1, const UserProfile& p2) const;

private:
    DatabaseManager* m_dbManager;
};

#endif // MATCHENGINE_H
