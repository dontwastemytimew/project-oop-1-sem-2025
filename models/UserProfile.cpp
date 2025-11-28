#include "UserProfile.h"
#include "Preference.h"
#include <QRegExp>

UserProfile::UserProfile(int id, const QString &name, int age,
                         const QString &city, const QString &bio,
                         const QString &gender, const QString &orientation)
    : m_id(id), m_name(name), m_age(age), m_city(city), m_bio(bio),
      m_gender(gender), m_orientation(orientation), m_preferences()
{
    // m_contactInfo буде створено автоматично конструктором за замовчуванням
}

int UserProfile::getId() const { return m_id; }
QString UserProfile::getName() const { return m_name; }
int UserProfile::getAge() const { return m_age; }
QString UserProfile::getCity() const { return m_city; }
QString UserProfile::getBio() const { return m_bio; }
QString UserProfile::getGender() const { return m_gender; }
QString UserProfile::getOrientation() const { return m_orientation; }
ContactInfo UserProfile::getContactInfo() const { return m_contactInfo; }

void UserProfile::setId(int newId) { m_id = newId; }
void UserProfile::setName(const QString &newName) { m_name = newName; }
void UserProfile::setAge(int newAge) { m_age = newAge; }
void UserProfile::setCity(const QString &newCity) { m_city = newCity; }
void UserProfile::setBio(const QString &newBio) { m_bio = newBio; }
void UserProfile::setGender(const QString &gender) { m_gender = gender; }
void UserProfile::setOrientation(const QString &orientation) { m_orientation = orientation; }
void UserProfile::setContactInfo(const ContactInfo &info) { m_contactInfo = info; }
Preference UserProfile::getPreference() const { return m_preferences; }
void UserProfile::setPreference(const Preference& prefs) { m_preferences = prefs; }


// ВАЛІДАЦІЯ ПРОФІЛЮ

bool UserProfile::isValid() const
{
    // 1. Перевірка імені
    if (m_name.trimmed().length() < 2)
        return false;

    QRegExp nameRegex("^[A-Za-zА-Яа-яІіЇїЄє' -]+$");
    if (!nameRegex.exactMatch(m_name.trimmed()))
        return false;

    // 2. Вік
    if (m_age < 18 || m_age > 99)
        return false;

    // 3. Місто
    if (m_city.trimmed().length() < 2)
        return false;

    // 4. Біо
    if (m_bio.trimmed().length() < 10)
        return false;

    // 5. Телефон
    QRegExp phoneRegex("^\\+?[0-9]{10,15}$");
    if (!m_contactInfo.phone().isEmpty() &&
        !phoneRegex.exactMatch(m_contactInfo.phone()))
    {
        return false;
    }

    return true;
}
