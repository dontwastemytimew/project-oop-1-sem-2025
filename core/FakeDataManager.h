#ifndef FAKEDATAMANAGER_H
#define FAKEDATAMANAGER_H

#include "DatabaseManager.h"

class FakeDataManager {
public:
    /**
     * @brief Генерує та зберігає випадкових користувачів у БД.
     * @param dbManager Вказівник на менеджер бази даних.
     * @param count Кількість користувачів для генерації (за замовчуванням 50).
     */
    static void generateTestUsers(DatabaseManager* dbManager, int count = 50);

    /**
     * @brief Генерує СПИСОК випадкових користувачів.
     */
    static QList<UserProfile> generateList(int count);


private:
    // Допоміжні методи для генерації випадкових даних
    static QString getRandomName();
    static QString getRandomCity();
    static QString getRandomBio();
    static QString generateEmail(const QString& name);
    static QString generatePhone();
    static QString getRandomMaleName();
    static QString getRandomFemaleName();
};

#endif // FAKEDATAMANAGER_H