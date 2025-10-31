#include "Preference.h"

Preference::Preference(int minAge, int maxAge, const QString &city)
    : m_minAge(minAge), m_maxAge(maxAge), m_city(city)
{
}


int Preference::getMinAge() const { return m_minAge; }
int Preference::getMaxAge() const { return m_maxAge; }
QString Preference::getCity() const { return m_city; }


void Preference::setMinAge(int newMinAge) { m_minAge = newMinAge; }
void Preference::setMaxAge(int newMaxAge) { m_maxAge = newMaxAge; }
void Preference::setCity(const QString &newCity) { m_city = newCity; }