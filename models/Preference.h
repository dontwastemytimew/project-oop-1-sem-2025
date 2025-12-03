#ifndef PREFERENCE_H
#define PREFERENCE_H

#include <QString>

/**
 * @brief Клас моделі даних для збереження критеріїв пошуку (вподобань).
 *
 * Цей клас інкапсулює набір фільтрів, які користувач встановлює на сторінці пошуку
 * для знаходження потенційних партнерів. Об'єкт цього класу передається в
 * DatabaseManager для формування SQL-запитів фільтрації.
 *
 * Основні параметри:
 * - Віковий діапазон.
 * - Географія (місто).
 * - Гендерна ідентичність та орієнтація.
 */
class Preference {
public:
    /**
     * @brief Конструктор Preference.
     *
     * Ініціалізує об'єкт вподобань. Значення за замовчуванням (0 або порожній рядок)
     * інтерпретуються системою як "Не важливо" або "Будь-який".
     *
     * @param minAge Мінімальний бажаний вік (0 = неважливо).
     * @param maxAge Максимальний бажаний вік (0 = неважливо).
     * @param city Бажане місто (порожній рядок = будь-яке місто).
     * @param gender Бажана стать партнера (наприклад, "Жінка", "Чоловік").
     * @param orientation Бажана орієнтація партнера (наприклад, "Гетеро", "Бісексуал").
     */
    Preference(int minAge = 0, int maxAge = 0, const QString &city = "",
        const QString &gender = "", const QString &orientation = "");

    // --- Геттери (Методи доступу) ---

    /**
     * @brief Отримує мінімальний вік для пошуку.
     * @return int Вік у роках.
     */
    int getMinAge() const;

    /**
     * @brief Отримує максимальний вік для пошуку.
     * @return int Вік у роках.
     */
    int getMaxAge() const;

    /**
     * @brief Отримує назву бажаного міста.
     * @return QString Назва міста або порожній рядок.
     */
    QString getCity() const;

    /**
     * @brief Отримує бажану стать партнера.
     * @return QString Рядок із назвою статі.
     */
    QString getGender() const;

    /**
     * @brief Отримує бажану орієнтацію партнера.
     * @return QString Рядок із назвою орієнтації.
     */
    QString getOrientation() const;

    // --- Сеттери (Методи модифікації) ---

    /**
     * @brief Встановлює мінімальний вік.
     * @param newMinAge Нове значення мінімального віку.
     */
    void setMinAge(int newMinAge);

    /**
     * @brief Встановлює максимальний вік.
     * @param newMaxAge Нове значення максимального віку.
     */
    void setMaxAge(int newMaxAge);

    /**
     * @brief Встановлює бажане місто.
     * @param newCity Назва міста.
     */
    void setCity(const QString &newCity);

    /**
     * @brief Встановлює бажану стать.
     * @param gender Стать ("Чоловік", "Жінка" тощо).
     */
    void setGender(const QString &gender);

    /**
     * @brief Встановлює бажану орієнтацію.
     * @param orientation Орієнтація.
     */
    void setOrientation(const QString &orientation);

private:
    int m_minAge;           ///< Нижня межа віку.
    int m_maxAge;           ///< Верхня межа віку.
    QString m_city;         ///< Місто пошуку.
    QString m_gender;       ///< Фільтр по статі.
    QString m_orientation;  ///< Фільтр по орієнтації.
};

#endif //PREFERENCE_H