#ifndef USERPROFILE_H
#define USERPROFILE_H

#include "ContactInfo.h"
#include "Preference.h"

/**
 * @brief Клас UserProfile
 *
 * Основна модель даних, що представляє профіль користувача в системі.
 * Зберігає всю інформацію про користувача, включаючи його контакти, стать та орієнтацію.
 */
class UserProfile {
public:

    UserProfile(int id = -1, const QString &name = "", int age = 0,
                const QString &city = "", const QString &bio = "",
                const QString &gender = "", const QString &orientation = "");

    // --- Геттери ---
    int getId() const;
    QString getName() const;
    int getAge() const;
    QString getCity() const;
    QString getBio() const;
    QString getGender() const;
    QString getOrientation() const;
    ContactInfo getContactInfo() const;
    Preference getPreference() const;

    QStringList getTags() const { return m_tags; }

    // --- Сеттери ---
    void setId(int newId);
    void setName(const QString &newName);
    void setAge(int newAge);
    void setCity(const QString &newCity);
    void setBio(const QString &newBio);
    void setGender(const QString &gender);
    void setOrientation(const QString &orientation);
    void setContactInfo(const ContactInfo &info);
    void setPreference(const Preference& prefs);

    void setTags(const QStringList& tags) { m_tags = tags; }

    /**
     * @brief Перевіряє коректність введених користувачем даних.
     * @return true, якщо дані валідні
     */
    bool isValid() const;

private:
    int m_id;
    QString m_name;
    int m_age;
    QString m_city;
    QString m_bio;
    QString m_gender;
    QString m_orientation;
    ContactInfo m_contactInfo;
    Preference m_preferences;
    QStringList m_tags;
};

#endif // USERPROFILE_H
