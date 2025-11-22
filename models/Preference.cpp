#include "Preference.h"

Preference::Preference(int minAge, int maxAge, const QString &city,
                       const QString &gender, const QString &orientation)
    : m_minAge(minAge), m_maxAge(maxAge), m_city(city),
      m_gender(gender), m_orientation(orientation)
{
}

int Preference::getMinAge() const { return m_minAge; }
int Preference::getMaxAge() const { return m_maxAge; }
QString Preference::getCity() const { return m_city; }
QString Preference::getGender() const { return m_gender; }
QString Preference::getOrientation() const { return m_orientation; }

void Preference::setMinAge(int newMinAge) { m_minAge = newMinAge; }
void Preference::setMaxAge(int newMaxAge) { m_maxAge = newMaxAge; }
void Preference::setCity(const QString &newCity) { m_city = newCity; }
void Preference::setGender(const QString &gender) { m_gender = gender; }
void Preference::setOrientation(const QString &orientation) { m_orientation = orientation; }