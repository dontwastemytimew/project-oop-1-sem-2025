#ifndef MATCHENGINE_H
#define MATCHENGINE_H

#include "Preference.h"
#include "UserProfile.h"
#include "DatabaseManager.h"
#include <QtMath>

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

private:
    DatabaseManager* m_dbManager;
};

#endif // MATCHENGINE_H
