#ifndef USERPROFILE_H
#define USERPROFILE_H

#include <QString>
#include <QStringList>
#include "ContactInfo.h"
#include "Preference.h"

/**
 * @brief Основна модель даних профілю користувача.
 *
 * Цей клас представляє повну інформацію про користувача системи Match++.
 * Він агрегує (містить) в собі прості дані (ім'я, вік) та складніші
 * структури (ContactInfo, Preference).
 *
 * Об'єкти цього класу використовуються для передачі даних між шаром доступу
 * до даних (DatabaseManager), логікою (MatchEngine) та інтерфейсом (GUI).
 */
class UserProfile {
public:
    /**
     * @brief Конструктор UserProfile.
     *
     * Ініціалізує новий профіль з вказаними параметрами або значеннями за замовчуванням.
     * ID за замовчуванням дорівнює -1, що означає "новий користувач, ще не збережений в БД".
     *
     * @param id Унікальний ідентифікатор (Primary Key з БД).
     * @param name Ім'я користувача.
     * @param age Вік.
     * @param city Місто проживання.
     * @param bio Короткий опис "Про себе".
     * @param gender Стать ("Чоловік"/"Жінка").
     * @param orientation Орієнтація.
     * @param photoPath Шлях до файлу фотографії.
     */
    UserProfile(int id = -1, const QString &name = "", int age = 0,
                const QString &city = "", const QString &bio = "",
                const QString &gender = "", const QString &orientation = "",
                const QString &photoPath = "");

    // --- Геттери (Методи доступу) ---

    /**
     * @brief Отримує унікальний ID користувача.
     * @return int ID або -1, якщо профіль не збережено.
     */
    int getId() const;

    /**
     * @brief Отримує ім'я користувача.
     * @return QString Ім'я.
     */
    QString getName() const;

    /**
     * @brief Отримує вік користувача.
     * @return int Вік у роках.
     */
    int getAge() const;

    /**
     * @brief Отримує назву міста.
     * @return QString Місто.
     */
    QString getCity() const;

    /**
     * @brief Отримує текст біографії.
     * @return QString Опис профілю.
     */
    QString getBio() const;

    /**
     * @brief Отримує стать користувача.
     * @return QString Рядок із назвою статі.
     */
    QString getGender() const;

    /**
     * @brief Отримує орієнтацію користувача.
     * @return QString Рядок із назвою орієнтації.
     */
    QString getOrientation() const;

    /**
     * @brief Отримує об'єкт контактної інформації.
     * @return ContactInfo Копія об'єкта з телефоном та email.
     */
    ContactInfo getContactInfo() const;

    /**
     * @brief Повертає локальний шлях до файлу фотографії.
     * @return QString Абсолютний або відносний шлях до зображення.
     */
    QString getPhotoPath() const;

    /**
     * @brief Повертає налаштування пошуку (фільтри) для цього користувача.
     *
     * Використовується MatchEngine для визначення, кого шукає цей користувач.
     * @return Preference Об'єкт з критеріями пошуку.
     */
    Preference getPreference() const;

    /**
     * @brief Повертає список інтересів (тегів).
     * @return QStringList Список рядків (наприклад, {"IT", "Спорт", "Кава"}).
     */
    QStringList getTags() const;

    // --- Сеттери (Методи модифікації) ---

    /**
     * @brief Встановлює ID користувача.
     * Зазвичай викликається DatabaseManager після збереження в БД.
     * @param newId Новий ID.
     */
    void setId(int newId);

    /**
     * @brief Встановлює ім'я.
     * @param newName Нове ім'я.
     */
    void setName(const QString &newName);

    /**
     * @brief Встановлює вік.
     * @param newAge Новий вік.
     */
    void setAge(int newAge);

    /**
     * @brief Встановлює місто.
     * @param newCity Нова назва міста.
     */
    void setCity(const QString &newCity);

    /**
     * @brief Встановлює біографію.
     * @param newBio Новий текст опису.
     */
    void setBio(const QString &newBio);

    /**
     * @brief Оновлює контактну інформацію.
     * @param info Новий об'єкт ContactInfo.
     */
    void setContactInfo(const ContactInfo &info);

    /**
     * @brief Встановлює стать.
     * @param gender Нова стать.
     */
    void setGender(const QString &gender);

    /**
     * @brief Встановлює орієнтацію.
     * @param orientation Нова орієнтація.
     */
    void setOrientation(const QString &orientation);

    /**
     * @brief Встановлює шлях до фотографії.
     * @param path Шлях до файлу.
     */
    void setPhotoPath(const QString &path);

    /**
     * @brief Оновлює критерії пошуку (вподобання).
     * @param prefs Новий об'єкт Preference.
     */
    void setPreference(const Preference &prefs);

    /**
     * @brief Встановлює список тегів.
     * @param tags Новий список інтересів.
     */
    void setTags(const QStringList& tags);

    /**
     * @brief Перевіряє, чи заповнені мінімально необхідні поля.
     *
     * Використовується перед збереженням у базу даних.
     * Перевіряє, чи не пусте ім'я, чи вік >= 18 тощо.
     *
     * @return true, якщо профіль валідний і готовий до збереження.
     */
    bool isValid() const;

private:
    int m_id;                   ///< Унікальний ID (-1, якщо не в БД).
    QString m_name;             ///< Ім'я.
    int m_age;                  ///< Вік.
    QString m_city;             ///< Місто.
    QString m_bio;              ///< Біографія.
    QString m_gender;           ///< Стать.
    QString m_orientation;      ///< Орієнтація.
    ContactInfo m_contactInfo;  ///< Об'єкт контактів (композиція).
    Preference m_preferences;   ///< Об'єкт вподобань (композиція).
    QStringList m_tags;         ///< Список тегів.
    QString m_photoPath;        ///< Шлях до фото.
};

#endif // USERPROFILE_H