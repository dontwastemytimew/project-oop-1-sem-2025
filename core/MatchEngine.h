#ifndef MATCHENGINE_H
#define MATCHENGINE_H

#include "Preference.h"
#include "UserProfile.h"
#include <QString>


/**
 * @brief MatchEngine class
 *
 * клас для простого алгоритму фільтрування профілів за Preference
 */
class MatchEngine {
public:
    /**
     * @brief calculateMatch
     * @param profile Профіль користувача
     * @param prefs Критерії пошуку
     * @return true, якщо профіль відповідає **всім** критеріям
     */
    static bool calculateMatch(const UserProfile &profile, const Preference &prefs);
};

#endif // MATCHENGINE_H