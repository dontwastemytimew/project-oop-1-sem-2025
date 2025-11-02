#ifndef MATCHENGINE_H
#define MATCHENGINE_H

#include "Preference.h"
#include <QString>

/**
 * @brief UserProfile struct
 *
 * структура для демонстрацї профілю користувача
 */
struct UserProfile {
    QString name;
    int age;
    QString city;
};

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
     * @return true, якщо профіль відповідає хоча б одному критерію
     */
    static bool calculateMatch(const UserProfile &profile, const Preference &prefs);
};

#endif // MATCHENGINE_H