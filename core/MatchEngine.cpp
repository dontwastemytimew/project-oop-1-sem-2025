#include "MatchEngine.h"
#include "UserLogger.h"
#include <QtMath>

MatchEngine::MatchEngine(DatabaseManager* dbManager)
    : m_dbManager(dbManager)
{
}

bool MatchEngine::calculateMatch(const UserProfile &profile, const Preference &prefs)
{
    if (prefs.getMinAge() != 0 && profile.getAge() < prefs.getMinAge())
        return false;

    if (prefs.getMaxAge() != 0 && profile.getAge() > prefs.getMaxAge())
        return false;

    if (!prefs.getCity().isEmpty() && profile.getCity() != prefs.getCity())
        return false;

    return true;
}

/**
 * @brief Основний алгоритм сумісності (описаний у твоєму пункті 16 + 19)
 *
 * Формула:
 *  +30       — збіг міста
 *  +10       — за кожен збіг інтересів (TODO)
 *  +20       — різниця у віці < 2
 *  +0..20    — бонус за статеву сумісність (орієнтацію)
 *  -1000     — якщо гендер/орієнтація взагалі несумісні
 */
int MatchEngine::compatibilityScore(const UserProfile& p1, const UserProfile& p2) const
{
    int score = 0;

    //  1. Несумісність гендерів/орієнтації → match impossible
    if (p1.getOrientation() == "Гетеро") {
        if (p1.getGender() == p2.getGender()) {
            return -1000;
        }
    }
    else if (p1.getOrientation() == "Гей/Лесбі") {
        if (p1.getGender() != p2.getGender()) {
            return -1000;
        }
    }
    // Бі — ок, не перевіряємо

    //  2. Місто
    if (!p1.getCity().isEmpty() && p1.getCity() == p2.getCity())
        score += 30;

    //  3. Різниця у віці < 2 років
    if (qAbs(p1.getAge() - p2.getAge()) <= 2)
        score += 20;

    //  4. Теги (TODO)
    // Коли ти додаси систему тегів — тут буде:
    // for each tag in p1.tags:
    //      if p2.tags contains tag → score += 10

    //  5. Проста бонусна логіка (необов’язково, але красиво)
    // Чим ближче вік – тим краще
    int diff = qAbs(p1.getAge() - p2.getAge());
    if (diff <= 5)
        score += (10 - diff); // 10,9,8,7… бонус

    return score;
}

/**
 * @brief Повертає % сумісності (0–100)
 */
int MatchEngine::compatibilityPercent(const UserProfile& p1, const UserProfile& p2) const
{
    int score = compatibilityScore(p1, p2);

    if (score < 0)
        return 0;

    // Максимально можливо:
    // 30 (місто) +20 (вік) +10..50 (теги в майбутньому)
    // → поки візьмемо max = 60
    const int MAX_SCORE = 60;

    int percent = qFloor((score * 100.0) / MAX_SCORE);
    if (percent > 100) percent = 100;
    if (percent < 0) percent = 0;

    return percent;
}

/**
 * @brief Повертає true, якщо вони подолали базові бар’єри
 */
bool MatchEngine::isCompatible(const UserProfile& p1, const UserProfile& p2) const
{
    if (p1.getId() == p2.getId() && p1.getId() != -1)
        return false;

    int percent = compatibilityPercent(p1, p2);

    //  60% — умовний поріг сумісності
    return percent >= 60;
}
