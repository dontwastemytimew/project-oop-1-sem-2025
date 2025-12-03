#ifndef FAKEDATAMANAGER_H
#define FAKEDATAMANAGER_H

#include "DatabaseManager.h"
#include <QList>
#include <QString>

/**
 * @brief Клас-утиліта для генерації фейкових (тестових) даних.
 *
 * Цей клас надає набір статичних методів для створення випадкових профілів користувачів,
 * генерації реалістичних атрибутів (ім'я, місто, біографія, контакти) та
 * симуляції активності у додатку (наприклад, генерація вхідних лайків).
 *
 * @note Використовується переважно для тестування (Unit Tests) та наповнення
 * бази даних під час розробки, щоб не вводити дані вручну.
 */
class FakeDataManager {
public:
    /**
     * @brief Генерує та зберігає випадкових користувачів безпосередньо у базу даних.
     *
     * Метод створює вказану кількість профілів з випадковими параметрами
     * (стать, вік, орієнтація, фото) та зберігає їх через DatabaseManager.
     * Також для кожного створеного користувача додаються випадкові теги.
     *
     * @param dbManager Вказівник на ініціалізований менеджер бази даних.
     * @param count Кількість користувачів, яку необхідно згенерувати (за замовчуванням 50).
     */
    static void generateTestUsers(DatabaseManager* dbManager, int count = 50);

    /**
     * @brief Генерує список об'єктів UserProfile у пам'яті.
     *
     * На відміну від generateTestUsers, цей метод НЕ зберігає дані в БД.
     * Він корисний для юніт-тестів, де потрібно перевірити логіку без
     * взаємодії з реальною базою даних.
     *
     * @param count Кількість профілів для генерації.
     * @return QList<UserProfile> Список згенерованих об'єктів профілів.
     */
    static QList<UserProfile> generateList(int count);

    /**
     * @brief Створює "вхідні" лайки для вказаного користувача.
     *
     * Метод емулює ситуацію, коли інші (вже існуючі в базі) користувачі
     * лайкнули вказаного користувача. Це необхідно для тестування
     * функціоналу "Метчі" та "Хто мене лайкнув".
     *
     * @param dbManager Вказівник на менеджер бази даних.
     * @param currentUserId ID користувача, який отримає лайки.
     * @param count Максимальна кількість лайків для генерації (за замовчуванням 50).
     */
    static void seedReverseLikes(DatabaseManager* dbManager, int currentUserId, int count = 50);


private:
    // --- Допоміжні методи генерації атрибутів ---

    /**
     * @brief Повертає випадкове чоловіче або жіноче ім'я.
     * @return QString Випадкове ім'я.
     */
    static QString getRandomName();

    /**
     * @brief Повертає випадкове місто зі списку великих міст України.
     * @return QString Назва міста.
     */
    static QString getRandomCity();

    /**
     * @brief Генерує випадковий текст біографії ("Про себе").
     * @return QString Текст біографії.
     */
    static QString getRandomBio();

    /**
     * @brief Генерує фейкову електронну адресу на основі імені.
     * @param name Ім'я користувача для формування email.
     * @return QString Рядок у форматі "name_1234@example.com".
     */
    static QString generateEmail(const QString& name);

    /**
     * @brief Генерує випадковий номер телефону у форматі +380...
     * @return QString Номер телефону.
     */
    static QString generatePhone();

    /**
     * @brief Повертає випадкове чоловіче ім'я зі списку.
     * @return QString Чоловіче ім'я.
     */
    static QString getRandomMaleName();

    /**
     * @brief Повертає випадкове жіноче ім'я зі списку.
     * @return QString Жіноче ім'я.
     */
    static QString getRandomFemaleName();

    /**
     * @brief Генерує список випадкових тегів (інтересів).
     *
     * Обирає випадкову підмножину зі списку доступних тегів (наприклад, "IT", "Спорт", "Кава").
     * @return QList<QString> Список тегів.
     */
    static QList<QString> generateRandomTags();

    /**
     * @brief Додає випадкові теги до існуючих користувачів у базі даних.
     *
     * Внутрішній метод для масового оновлення тегів.
     * @param dbManager Вказівник на менеджер бази даних.
     */
    static void seedTags(DatabaseManager* dbManager);

    /**
     * @brief Отримує шлях до випадкового фото з ресурсів.
     *
     * Обирає фотографію з папки ресурсів відповідно до статі користувача.
     *
     * @param gender Стать користувача ("Чоловік" або "Жінка").
     * @return QString Шлях до ресурсу (наприклад, ":/resources/fake_photos/male/1.jpg").
     */
    static QString getRandomPhotoPath(const QString& gender);
};

#endif // FAKEDATAMANAGER_H