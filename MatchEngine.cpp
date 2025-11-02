#include "MatchEngine.h"

bool MatchEngine::calculateMatch(const UserProfile &profile, const Preference &prefs)
{
    //перевірка мінімального віку
    if (prefs.getMinAge() != 0 && profile.age < prefs.getMinAge()) {
        return false;
    }

    //перевірка максимального віку
    if (prefs.getMaxAge() != 0 && profile.age > prefs.getMaxAge()) {
        return false;
    }

    //перевірка міста
    if (!prefs.getCity().isEmpty() && profile.city != prefs.getCity()) {
        return false;
    }

    //якшо профіль відповідає хоча б одному критерію
    return true;
}