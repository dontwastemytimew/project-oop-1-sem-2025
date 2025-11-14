#include "UserProfile.h"

UserProfile::UserProfile(int id, const QString &name, int age,
                         const QString &city, const QString &bio)
    : m_id(id), m_name(name), m_age(age), m_city(city), m_bio(bio)
{
    // m_contactInfo буде створено автоматично з конструктором за замовчуванням
}


int UserProfile::getId() const { return m_id; }
QString UserProfile::getName() const { return m_name; }
int UserProfile::getAge() const { return m_age; }
QString UserProfile::getCity() const { return m_city; }
QString UserProfile::getBio() const { return m_bio; }
ContactInfo UserProfile::getContactInfo() const { return m_contactInfo; }


void UserProfile::setId(int newId) { m_id = newId; }
void UserProfile::setName(const QString &newName) { m_name = newName; }
void UserProfile::setAge(int newAge) { m_age = newAge; }
void UserProfile::setCity(const QString &newCity) { m_city = newCity; }
void UserProfile::setBio(const QString &newBio) { m_bio = newBio; }
void UserProfile::setContactInfo(const ContactInfo &info) { m_contactInfo = info; }