#include "MatchEngine.h"
#include "UserLogger.h"

MatchEngine::MatchEngine(DatabaseManager* dbManager)
    : m_dbManager(dbManager)
{
}

// Фільтр по вподобаннях у профілі
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

// Жорстка перевірка сумісності
bool MatchEngine::isCompatible(const UserProfile& p1, const UserProfile& p2) const
{
    if (p1.getId() != -1 && p1.getId() == p2.getId())
        return false;

    const Preference& pref = p1.getPreference();

    // Стать
    if (!pref.getGender().isEmpty() &&
        pref.getGender() != "Не важливо" &&
        pref.getGender() != p2.getGender())
        return false;

    // Орієнтація
    if (p1.getOrientation() == "Гетеро" &&
        p1.getGender() == p2.getGender())
        return false;

    if (p1.getOrientation() == "Гей/Лесбі" &&
        p1.getGender() != p2.getGender())
        return false;

    // Вік
    if ((pref.getMinAge() != 0 && p2.getAge() < pref.getMinAge()) ||
        (pref.getMaxAge() != 0 && p2.getAge() > pref.getMaxAge()))
        return false;

    // Місто
    if (!pref.getCity().isEmpty() &&
        pref.getCity() != "Не важливо" &&
        pref.getCity() != p2.getCity())
        return false;

    // Теги (має бути хоча б 1)
    int sharedTags = 0;
    for (const QString& tag : p1.getTags())
        if (p2.getTags().contains(tag))
            sharedTags++;

    if (sharedTags < 1)
        return false;

    return true;
}

// М'яка оцінка (0–100%)
int MatchEngine::computeCompatibility(const UserProfile &a, const UserProfile &b) const
{
    int score = 0;

    // 1) Теги (0–30)
    int shared = 0;
    for (const QString& t : a.getTags())
        if (b.getTags().contains(t))
            shared++;

    if (!a.getTags().isEmpty())
        score += qMin(30, shared * 10);

    // 2) Вік (0–20)
    int diffAge = abs(a.getAge() - b.getAge());
    if (diffAge < 3) score += 20;
    else if (diffAge < 6) score += 10;

    // 3) Місто (0–20)
    if (a.getCity() == b.getCity())
        score += 20;

    // 4) Орієнтація + стать (0–20)
    if (isCompatible(a, b) && isCompatible(b, a))
        score += 20;

    // 5) Біо (0–10)
    if (!a.getBio().isEmpty() && !b.getBio().isEmpty()) {
        QString w1 = a.getBio().split(" ").first();
        QString w2 = b.getBio().split(" ").first();
        if (w1 == w2)
            score += 10;
    }

    return score;
}

// Список відсортованих матчів
QList<QPair<UserProfile, int>> MatchEngine::getSortedMatches(int userId) const
{
    QList<QPair<UserProfile, int>> result;

    UserProfile me = m_dbManager->getUserById(userId);
    QList<UserProfile> all = m_dbManager->getAllUsers();

    for (const UserProfile& u : all)
    {
        if (u.getId() == me.getId())
            continue;

        if (!isCompatible(me, u))
            continue;

        int percent = computeCompatibility(me, u);
        result.append({u, percent});
    }

    std::sort(result.begin(), result.end(),
              [](const auto& a, const auto& b){
                  return a.second > b.second;
              });

    return result;
}
