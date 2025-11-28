#ifndef MATCHENGINE_H
#define MATCHENGINE_H

#include "Preference.h"
#include "UserProfile.h"
#include "DatabaseManager.h"
#include <QtMath>
#include <QList> // Для QList<QPair>
#include <algorithm> // Для std::sort

/**
 * @brief MatchEngine class
 *
 * Розрахунок сумісності профілів на основі:
 * - статі / орієнтації
 * - віку
 * - міста
 * - формули балів
 * - тегів (TODO)
 */
class MatchEngine {
public:

    explicit MatchEngine(DatabaseManager* dbManager);

    /**
     * @brief Перевіряє сумісність двох профілів.
     * @return true, якщо вони подолали базові бар'єри (наприклад, 60%).
     */
    bool isCompatible(const UserProfile& p1, const UserProfile& p2) const;

    /**
     * @brief Обчислює сумарний бал сумісності (напр., 50 балів).
     */
    int compatibilityScore(const UserProfile& p1, const UserProfile& p2) const;

    /**
     * @brief Обчислює % сумісності (0–100).
     */
    int compatibilityPercent(const UserProfile& p1, const UserProfile& p2) const;

    /**
     * @brief Повертає список сумісних профілів, відсортованих за відсотком сумісності.
     * @param userId ID користувача, для якого шукаємо пари.
     */
    QList<QPair<UserProfile, int>> getSortedMatches(int userId) const;

private:
    DatabaseManager* m_dbManager;
};

#endif // MATCHENGINE_H