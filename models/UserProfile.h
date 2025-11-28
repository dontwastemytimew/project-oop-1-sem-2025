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
                const QString &photoPath = ""); // <-- ФУНКЦІОНАЛ ФОТО ПОВЕРНУТО

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
    QString getPhotoPath() const; // <-- ФУНКЦІОНАЛ ФОТО ПОВЕРНУТО

    /**
     * @brief Повертає збережені критерії пошуку користувача.
     * @return Об'єкт Preference.
     */
    Preference getPreference() const;

    /**
     * @brief Повертає список тегів/інтересів користувача.
     */
    QStringList getTags() const { return m_tags; }

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

    /**
     * @brief Встановлює теги/інтереси користувача.
     */
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

    QStringList m_tags; // <-- ЗМІННА ДЛЯ ТЕГІВ
    QString m_photoPath; // <-- ЗМІННА ДЛЯ ФОТОГРАФІЙ
};

#endif // USERPROFILE_H