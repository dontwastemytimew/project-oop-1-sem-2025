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
    /**
     * @brief Конструктор UserProfile.
     * @param id Унікальний ідентифікатор (за замовчуванням -1, що означає "новий профіль").
     * @param name Ім'я користувача.
     * @param age Вік користувача.
     * @param city Місто проживання.
     * @param bio Коротка біографія або опис.
     * @param gender Стать користувача (напр., "Чоловік", "Жінка").
     * @param orientation Сексуальна орієнтація (напр., "Гетеро", "Бісексуал").
     */
    UserProfile(int id = -1, const QString &name = "", int age = 0,
                const QString &city = "", const QString &bio = "",
                const QString &gender = "", const QString &orientation = "");

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
     * @return Об'єкт ContactInfo, що містить телефон та email.
     */
    ContactInfo getContactInfo() const;

    // --- Сеттери ---

    void setId(int newId);
    void setName(const QString &newName);
    void setAge(int newAge);
    void setCity(const QString &newCity);
    void setBio(const QString &newBio);

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
     * @brief Встановлює контактну інформацію для профілю.
     * @param info Об'єкт ContactInfo.
     */
    void setContactInfo(const ContactInfo &info);

 /**
     * @brief Повертає збережені критерії пошуку користувача.
     * @return Об'єкт Preference.
     */
 Preference getPreference() const;

 /**
  * @brief Встановлює критерії пошуку для профілю.
  * Це потрібно для логіки MatchEngine.
  * @param prefs Об'єкт Preference.
  */
 void setPreference(const Preference& prefs);

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

 Preference m_preferences;
};

#endif //USERPROFILE_H