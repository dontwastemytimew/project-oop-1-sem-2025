#ifndef MATCHENGINE_H
#define MATCHENGINE_H

#include "Preference.h"
#include "UserProfile.h"
#include "DatabaseManager.h"

/**
 * @brief MatchEngine class
 *
 * Клас, що відповідає за алгоритм перевірки сумісності профілів
 * та базове фільтрування за налаштуваннями користувача (Preference).
 */
class MatchEngine {
public:

    explicit MatchEngine(DatabaseManager* dbManager);

    /**
     * @brief calculateMatch
     * @param profile Профіль користувача
     * @param prefs Критерії пошуку
     * @return true, якщо профіль відповідає усім критеріям
     */
    static bool calculateMatch(const UserProfile &profile, const Preference &prefs);

    /**
     * @brief isCompatible
     * Перевіряє сумісність двох реальних профілів.
     * @param p1 Перший профіль (користувач, що шукає)
     * @param p2 Другий профіль (кандидат)
     * @return true, якщо профілі сумісні
     */
    bool isCompatible(const UserProfile& p1, const UserProfile& p2) const;

    /**
     * @brief computeCompatibility
     * Обчислює відсоток сумісності між двома профілями (0–100).
     * Використовується тільки після проходження isCompatible().
     */
    int computeCompatibility(const UserProfile &a, const UserProfile &b) const;

    /**
     * @brief getSortedMatches
     * Повертає список сумісних профілів, відсортованих за відсотком сумісності.
     * @param userId ID користувача, для якого шукаємо пари
     */
    QList<QPair<UserProfile, int>> getSortedMatches(int userId) const;

private:
    DatabaseManager* m_dbManager;
};

#endif // MATCHENGINE_H
