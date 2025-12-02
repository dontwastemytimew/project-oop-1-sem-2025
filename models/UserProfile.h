#ifndef USERPROFILE_H
#define USERPROFILE_H

#include <QString>
#include <QStringList>
#include "ContactInfo.h"
#include "Preference.h"

/**
 * @brief Клас UserProfile
 *
 * Основна модель даних, що представляє профіль користувача в системі.
 * Зберігає всю інформацію про користувача, включаючи контакти, стать,
 * орієнтацію, фото та критерії пошуку.
 */
class UserProfile {
public:
    /**
     * @brief Конструктор UserProfile.
     */
    UserProfile(int id = -1, const QString &name = "", int age = 0,
                const QString &city = "", const QString &bio = "",
                const QString &gender = "", const QString &orientation = "",
                const QString &photoPath = "");

    // --- Геттери ---

    int getId() const;
    QString getName() const;
    int getAge() const;
    QString getCity() const;
    QString getBio() const;
    QString getGender() const;
    QString getOrientation() const;
    ContactInfo getContactInfo() const;

    /**
     * @brief Повертає шлях до фото профілю.
     */
    QString getPhotoPath() const;

    /**
     * @brief Повертає збережені критерії пошуку користувача.
     */
    Preference getPreference() const;

    /**
     * @brief Повертає список тегів/інтересів користувача.
     */
    QStringList getTags() const;

    // --- Сеттери ---

    void setId(int newId);
    void setName(const QString &newName);
    void setAge(int newAge);
    void setCity(const QString &newCity);
    void setBio(const QString &newBio);
    void setContactInfo(const ContactInfo &info);
    void setGender(const QString &gender);
    void setOrientation(const QString &orientation);
    void setPhotoPath(const QString &path);
    void setPreference(const Preference &prefs);

    /**
     * @brief Встановлює теги/інтереси користувача.
     */
    void setTags(const QStringList& tags);

    /**
     * @brief Перевіряє коректність введених користувачем даних.
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
    QString m_photoPath;
};

#endif // USERPROFILE_H