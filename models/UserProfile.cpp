#include "UserProfile.h"
#include "Preference.h"
#include "ContactInfo.h"
#include <QRegularExpression>
#include <QtMath>

UserProfile::UserProfile(int id, const QString &name, int age,
                         const QString &city, const QString &bio,
                         const QString &gender, const QString &orientation,
                         const QString &photoPath)
    : m_id(id), m_name(name), m_age(age), m_city(city), m_bio(bio),
    m_gender(gender), m_orientation(orientation),
    m_preferences(), m_photoPath(photoPath),
    m_contactInfo()
{
    // m_tags буде ініціалізовано за замовчуванням
}

// --- Геттери ---

int UserProfile::getId() const { return m_id; }
QString UserProfile::getName() const { return m_name; }
int UserProfile::getAge() const { return m_age; }
QString UserProfile::getCity() const { return m_city; }
QString UserProfile::getBio() const { return m_bio; }
QString UserProfile::getGender() const { return m_gender; }
QString UserProfile::getOrientation() const { return m_orientation; }
ContactInfo UserProfile::getContactInfo() const { return m_contactInfo; }
QString UserProfile::getPhotoPath() const { return m_photoPath; }
Preference UserProfile::getPreference() const { return m_preferences; }

// --- Сеттери ---
void UserProfile::setId(int newId) { m_id = newId; }
void UserProfile::setName(const QString &newName) { m_name = newName; }
void UserProfile::setAge(int newAge) { m_age = newAge; }
void UserProfile::setCity(const QString &newCity) { m_city = newCity; }
void UserProfile::setBio(const QString &newBio) { m_bio = newBio; }
void UserProfile::setGender(const QString &gender) { m_gender = gender; }
void UserProfile::setOrientation(const QString &orientation) { m_orientation = orientation; }
void UserProfile::setContactInfo(const ContactInfo &info) { m_contactInfo = info; }
void UserProfile::setPhotoPath(const QString &path) { m_photoPath = path; }
void UserProfile::setPreference(const Preference &prefs) { m_preferences = prefs; }


// --- ВАЛІДАЦІЯ ПРОФІЛЮ ---
bool UserProfile::isValid() const
{
    // 1. Перевірка імені
    if (m_name.trimmed().length() < 2)
        return false;

    QRegularExpression nameRegex("^[A-Za-zА-Яа-яІіЇїЄє' -]+$");
    if (!nameRegex.match(m_name.trimmed()).hasMatch())
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
    QRegularExpression phoneRegex("^\\+?[0-9]{10,15}$");
    if (!m_contactInfo.getPhone().isEmpty() &&
        !phoneRegex.match(m_contactInfo.getPhone()).hasMatch())
    {
        return false;
    }

    return true;
}