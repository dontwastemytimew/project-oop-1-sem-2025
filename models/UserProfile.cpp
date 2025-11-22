#include "UserProfile.h"
#include "Preference.h"

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
Preference UserProfile::getPreference() const { return m_preferences;}
void UserProfile::setPreference(const Preference& prefs) { m_preferences = prefs; }