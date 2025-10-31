#ifndef CONTACTINFO_H
#define CONTACTINFO_H

#include <QString>

/**
 * @brief Клас ContactInfo
 *
 * Модель даних, що інкапсулює (зберігає) контактну інформацію користувача.
 * Використовується як частина класу UserProfile (Композиція).
 */
class ContactInfo {
public:
    /**
     * @brief Конструктор ContactInfo.
     * @param phone Номер телефону (за замовчуванням порожній).
     * @param email Адреса електронної пошти (за замовчуванням порожній).
     */
    ContactInfo(const QString &phone = "", const QString &email = "");

    // --- Геттери (Методи доступу) ---

    /**
     * @brief Повертає номер телефону.
     * @return QString з номером телефону.
     */
    QString getPhone() const;

    /**
     * @brief Повертає адресу email.
     * @return QString з адресою email.
     */
    QString getEmail() const;

    // --- Сеттери (Методи модифікації) ---

    /**
     * @brief Встановлює номер телефону.
     * @param newPhone Новий номер телефону.
     */
    void setPhone(const QString &newPhone);

    /**
     * @brief Встановлює адресу email.
     * @param newEmail Нова адреса email.
     */
    void setEmail(const QString &newEmail);

private:
    /// Приватне поле для зберігання номера телефону.
    QString m_phone;

    /// Приватне поле для зберігання адреси email.
    QString m_email;
};

#endif //CONTACTINFO_H
