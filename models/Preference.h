#ifndef PREFERENCE_H
#define PREFERENCE_H

#include <QString>

/**
 * @brief Клас Preference
 *
 * Модель даних, що зберігає критерії пошуку (вподобання) користувача
 * для знаходження інших профілів.
 */
class Preference {
public:
    /**
     * @brief Конструктор Preference.
     * @param minAge Мінімальний бажаний вік (0 означає "неважливо").
     * @param maxAge Максимальний бажаний вік (0 означає "неважливо").
     * @param city Бажане місто (порожній рядок означає "неважливо").
     */
    Preference(int minAge = 0, int maxAge = 0, const QString &city = "");

    // --- Геттери ---
    int getMinAge() const;
    int getMaxAge() const;
    QString getCity() const;

    // --- Сеттери ---
    void setMinAge(int newMinAge);
    void setMaxAge(int newMaxAge);
    void setCity(const QString &newCity);

private:
    int m_minAge;
    int m_maxAge;
    QString m_city;
};

#endif //PREFERENCE_H
