#ifndef USERPROFILE_H
#define USERPROFILE_H

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
     * @param id Унікальний ідентифікатор (за замовчуванням -1, що означає "новий профіль").
     * @param name Ім'я користувача.
     * @param age Вік користувача.
     * @param city Місто проживання.
     * @param bio Коротка біографія або опис.
     * @param gender Стать користувача (напр., "Чоловік", "Жінка").
     * @param orientation Орієнтація (напр., "Гетеро", "Бісексуал").
     * @param photoPath Шлях до фото користувача (за замовчуванням порожній).
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
     * @return Рядок із шляхом до фото.
     */
    QString getPhotoPath() const;

    /**
     * @brief Повертає об'єкт налаштувань пошуку.
     */
    Preference getPreference() const;

    // --- Сеттери ---

    void setId(int newId);
    void setName(const QString &newName);
    void setAge(int newAge);
    void setCity(const QString &newCity);
    void setBio(const QString &newBio);
    void setGender(const QString &gender);
    void setOrientation(const QString &orientation);
    void setContactInfo(const ContactInfo &info);

    /**
     * @brief Встановлює шлях до фото.
     * @param path Новий шлях до фото.
     */
    void setPhotoPath(const QString &path);

    /**
     * @brief Встановлює критерії пошуку профілю.
     */
    void setPreference(const Preference &prefs);

private:
    int m_id;
    QString m_name;
    int m_age;
    QString m_city;
    QString m_bio;
    QString m_gender;
    QString m_orientation;

    ContactInfo m_contactInfo;

    /// Збережені критерії пошуку (вік, місто, інтереси тощо)
    Preference m_preferences;

    /// Шлях до фото користувача
    QString m_photoPath;
};

#endif // USERPROFILE_H
