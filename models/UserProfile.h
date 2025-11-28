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

    /**
     * @brief Повертає стать користувача.
     * @return Рядок із назвою статі.
     */
    QString getGender() const;

    /**
     * @brief Повертає орієнтацію користувача.
     * @return Рядок із назвою орієнтації.
     */
    QString getOrientation() const;

    /**
     * @brief Повертає об'єкт контактної інформації.
     * @return Об'єкт ContactInfo.
     */
    ContactInfo getContactInfo() const;

    /**
     * @brief Повертає шлях до фото профілю.
     * @return Рядок із шляхом до фото.
     */
    QString getPhotoPath() const;

    /**
     * @brief Повертає збережені критерії пошуку користувача.
     * @return Об'єкт Preference.
     */
    Preference getPreference() const;

    // --- Сеттери ---

    void setId(int newId);
    void setName(const QString &newName);
    void setAge(int newAge);
    void setCity(const QString &newCity);
    void setBio(const QString &newBio);
    void setContactInfo(const ContactInfo &info);

    /**
     * @brief Встановлює стать користувача.
     * @param gender Нова стать.
     */
    void setGender(const QString &gender);

    /**
     * @brief Встановлює орієнтацію користувача.
     * @param orientation Нова орієнтація.
     */
    void setOrientation(const QString &orientation);

    /**
     * @brief Встановлює шлях до фото.
     * @param path Новий шлях до фото.
     */
    void setPhotoPath(const QString &path);

    /**
     * @brief Встановлює критерії пошуку профілю.
     * @param prefs Об'єкт Preference.
     */
    void setPreference(const Preference &prefs);

private:
    /// Унікальний ID користувача
    int m_id;
    /// Ім'я користувача.
    QString m_name;
    /// Вік користувача.
    int m_age;
    /// Місто користувача.
    QString m_city;
    /// Біографія користувача.
    QString m_bio;
    /// Стать користувача.
    QString m_gender;
    /// Сексуальна орієнтація.
    QString m_orientation;

    /// Вбудований об'єкт для зберігання контактів (Композиція).
    ContactInfo m_contactInfo;

    /// Збережені критерії пошуку (вік, місто, інтереси тощо)
    Preference m_preferences;

    /// Шлях до фото користувача
    QString m_photoPath;
};

#endif // USERPROFILE_H