#ifndef USERPROFILE_H
#define USERPROFILE_H

#include <QString>
#include "ContactInfo.h" // Включення класу ContactInfo

/**
 * @brief Клас UserProfile
 *
 * Основна модель даних, що представляє профіль користувача в системі.
 * Зберігає всю інформацію про користувача, включаючи його контакти.
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
     */
    UserProfile(int id = -1, const QString &name = "", int age = 0,
                const QString &city = "", const QString &bio = "");

    // --- Геттери ---

    int getId() const;
    QString getName() const;
    int getAge() const;
    QString getCity() const;
    QString getBio() const;

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
     * @brief Встановлює контактну інформацію для профілю.
     * @param info Об'єкт ContactInfo.
     */
    void setContactInfo(const ContactInfo &info);

private:
    /// Унікальний ID користувача (зазвичай з БД).
    int m_id;

    /// Ім'я користувача.
    QString m_name;

    /// Вік користувача.
    int m_age;

    /// Місто користувача.
    QString m_city;

    /// Біографія користувача.
    QString m_bio;

    /// Вбудований об'єкт для зберігання контактів (Композиція).
    ContactInfo m_contactInfo;
};

#endif //USERPROFILE_H
