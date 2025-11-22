#include "MatchEngine.h"
#include "UserLogger.h"
#include "UserProfile.h"
#include "DatabaseManager.h"

MatchEngine::MatchEngine(DatabaseManager* dbManager)
    : m_dbManager(dbManager)
{
}

bool MatchEngine::calculateMatch(const UserProfile &profile, const Preference &prefs)
{
    //перевірка мінімального віку
    if (prefs.getMinAge() != 0 && profile.getAge() < prefs.getMinAge()) {
        UserLogger::log(Debug, "Match failed: User is too young.");
        return false;
    }

    //перевірка максимального віку
    if (prefs.getMaxAge() != 0 && profile.getAge() > prefs.getMaxAge()) {
        UserLogger::log(Debug, "Match failed: User is too old.");
        return false;
    }

    //перевірка міста
    if (!prefs.getCity().isEmpty() && profile.getCity() != prefs.getCity()) {
        UserLogger::log(Debug, "Match failed: City does not match.");
        return false;
    }

    //якщо профіль відповідає хоча б одному критерію
    UserLogger::log(Debug, "Match successful!");
    return true;
}

// Заглушка, яка виконує кілька перевірок для імітації роботи алгоритму
bool MatchEngine::isCompatible(const UserProfile& p1, const UserProfile& p2) const {
    // Не можеш бути сумісним із собою
    if (p1.getId() == p2.getId() && p1.getId() != -1) return false;

    // Якщо користувач p1 шукає інший гендер, ніж p2
    // p1.getPreference().getGender() - кого шукає p1
    // p2.getGender() - хто є p2
    if (!p1.getPreference().getGender().isEmpty() &&
        p1.getPreference().getGender() != "Не важливо" &&
        p1.getPreference().getGender() != p2.getGender()) {
        return false;
        }

    // Перевірка віку
    if (p2.getAge() < p1.getPreference().getMinAge() || p2.getAge() > p1.getPreference().getMaxAge()) {
        return false;
    }

    // Додаткова важка операція для навантаження CPU
    double score = (double)p1.getAge() * 0.5 + (double)p2.getAge() * 0.5;

    // Якщо пройшли всі фільтри, вважаємо сумісним
    return score > 20.0;
}