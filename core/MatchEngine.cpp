#include "MatchEngine.h"
#include "UserLogger.h"

MatchEngine::MatchEngine(DatabaseManager* dbManager)
    : m_dbManager(dbManager)
{
}

bool MatchEngine::calculateMatch(const UserProfile &profile, const Preference &prefs)
{
    // Мінімальний вік
    if (prefs.getMinAge() != 0 && profile.getAge() < prefs.getMinAge()) {
        return false;
    }

    // Максимальний вік
    if (prefs.getMaxAge() != 0 && profile.getAge() > prefs.getMaxAge()) {
        return false;
    }

    // Місто
    if (!prefs.getCity().isEmpty() && profile.getCity() != prefs.getCity()) {
        return false;
    }

    return true;
}


/**
 * @brief Повноцінна перевірка сумісності двох реальних профілів.
 */
bool MatchEngine::isCompatible(const UserProfile& p1, const UserProfile& p2) const
{
    // 1) Неможливо бути сумісним із самим собою
    if (p1.getId() != -1 && p1.getId() == p2.getId())
        return false;

    const Preference& pref = p1.getPreference();

    // 2) Перевірка гендеру
    if (!pref.getGender().isEmpty() &&
        pref.getGender() != "Не важливо" &&
        pref.getGender() != p2.getGender())
    {
        return false;
    }

    // 3) Перевірка орієнтації
    // p1 орієнтований на ...
    // p2 має відповідати
    if (p1.getOrientation() == "Гетеро") {
        // p1 шукає протилежну стать
        if (p1.getGender() == p2.getGender()) return false;
    }
    else if (p1.getOrientation() == "Гей/Лесбі") {
        // p1 шукає тільки свій гендер
        if (p1.getGender() != p2.getGender()) return false;
    }
    else if (p1.getOrientation() == "Бісексуал") {
        // ок, всі підходять
    }

    // 4) Перевірка віку p2 у межах вподобань p1
    if ((pref.getMinAge() != 0 && p2.getAge() < pref.getMinAge()) ||
        (pref.getMaxAge() != 0 && p2.getAge() > pref.getMaxAge()))
    {
        return false;
    }

    // 5) Перевірка міста (не обов'язкова)
    if (!pref.getCity().isEmpty() &&
        pref.getCity() != "Не важливо" &&
        pref.getCity() != p2.getCity())
    {
        return false;
    }

// 6) Перевірка тегів (спільних інтересів)
const QStringList& tags1 = p1.getTags(); 
const QStringList& tags2 = p2.getTags();

int sharedTags = 0;
for (const QString& tag : tags1) {
    if (tags2.contains(tag)) {
        sharedTags++;
    }
}

// Вимога: хоча б один спільний тег
if (sharedTags < 1) {
    return false;
}


    return true;
}
