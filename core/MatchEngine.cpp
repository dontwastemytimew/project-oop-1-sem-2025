#include "MatchEngine.h"
#include "UserLogger.h"

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