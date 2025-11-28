#include "MatchEngine.h"
#include "UserLogger.h"
#include "UserProfile.h"
#include <QtMath>
#include <QSettings>

MatchEngine::MatchEngine(DatabaseManager* dbManager)
    : m_dbManager(dbManager)
{
}

// Видаляємо стару calculateMatch, оскільки функціонал перенесено в SQL та compatibilityScore.
// bool MatchEngine::calculateMatch(const UserProfile &profile, const Preference &prefs)
// {
//     if (prefs.getMinAge() != 0 && profile.getAge() < prefs.getMinAge())
//         return false;
//
//     if (prefs.getMaxAge() != 0 && profile.getAge() > prefs.getMaxAge())
//         return false;
//
//     if (!prefs.getCity().isEmpty() && profile.getCity() != prefs.getCity())
//         return false;
//
//     return true;
// }

/**
 * @brief Основний алгоритм сумісності
 *
 * Формула:
 * +30       — збіг міста
 * +20       — різниця у віці < 2
 * +10       — бонус за близький вік (до 5 років)
 * -1000     — якщо гендер/орієнтація несумісні
 */
int MatchEngine::compatibilityScore(const UserProfile& p1, const UserProfile& p2) const
{
    int score = 0;

    // ПЕРЕВІРКА КРИТИЧНОЇ НЕСУМІСНОСТІ (Орієнтація)
    // p1 - той, хто шукає; p2 - знайдений профіль
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
    // "Бісексуал" та "Інше" вважаються сумісними з усіма гендерами.

    // --- БАЛАНСУЮЧІ БАЛИ ---

    // Місто (+30 балів)
    if (!p1.getCity().isEmpty() && p1.getCity() == p2.getCity()) {
        score += 30;
    }

    // Різниця у віці < 2 років (+20 балів)
    int ageDiff = qAbs(p1.getAge() - p2.getAge());
    if (ageDiff <= 2) {
        score += 20;
    }

    // Проста бонусна логіка: Близькість віку (+10..1 балів)
    if (ageDiff <= 5) {
        score += (10 - ageDiff);
    }

    // 5. Теги (TODO): Коли реалізуєте, додайте +10 за збіг

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

    // Максимальний бал (30 місто + 20 вік + 10 бонус = 60)
    // Якщо буде реалізовано 5 інтересів (+50), то max = 110.
    const int MAX_SCORE = 60;

    int percent = qFloor((score * 100.0) / MAX_SCORE);

    if (percent > 100) percent = 100;

    return percent;
}

/**
 * @brief Повертає true, якщо вони подолали базові бар’єри
 *
 * Цю функцію використовує DatabaseManager для початкової SQL-фільтрації.
 */
bool MatchEngine::isCompatible(const UserProfile& p1, const UserProfile& p2) const
{
    // Не можеш бути сумісним із собою
    if (p1.getId() == p2.getId() && p1.getId() != -1)
        return false;

    // Якщо несумісність критична (-1000), то відсоток буде 0
    if (compatibilityScore(p1, p2) < 0) {
        return false;
    }

    // Перевірка сумісності за відсотком (УМОВНИЙ ПОРІГ 60%)
    int percent = compatibilityPercent(p1, p2);

    return percent >= 60;
}