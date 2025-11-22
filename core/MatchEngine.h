#ifndef MATCHENGINE_H
#define MATCHENGINE_H

#include "Preference.h"
#include "UserProfile.h"
#include "DatabaseManager.h"

/**
 * @brief MatchEngine class
 *
 * клас для простого алгоритму фільтрування профілів за Preference
 */
class MatchEngine {
public:

 explicit MatchEngine(DatabaseManager* dbManager);

    /**
     * @brief calculateMatch
     * @param profile Профіль користувача
     * @param prefs Критерії пошуку
     * @return true, якщо профіль відповідає **всім** критеріям
     */
    static bool calculateMatch(const UserProfile &profile, const Preference &prefs);

    /**
     * @brief Перевіряє сумісність двох профілів.
     * @param p1 Перший профіль.
     * @param p2 Другий профіль.
     * @return true, якщо вони сумісні.
     */
    bool isCompatible(const UserProfile& p1, const UserProfile& p2) const;

 private:
 DatabaseManager* m_dbManager;
};



#endif // MATCHENGINE_H