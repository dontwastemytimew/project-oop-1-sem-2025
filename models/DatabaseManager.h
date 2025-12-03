#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QMap>
#include <QStringList>
#include "UserProfile.h"
#include "Preference.h"
#include "ChatStructures.h"

// Forward declarations для уникнення циклічних залежностей.
class ChatPageWidget;
class ChatManager;

/**
 * @brief Головний клас для керування базою даних (Data Access Layer).
 *
 * Цей клас інкапсулює всю логіку взаємодії з SQLite.
 * Він відповідає за:
 * - Ініціалізацію з'єднання та створення структури таблиць.
 * - CRUD операції (створення, читання, оновлення, видалення) для профілів.
 * - Пошук та фільтрацію користувачів за критеріями (вік, місто тощо).
 * - Управління соціальними функціями (лайки, перевірка метчів).
 * - Роботу з чатом (збереження та завантаження історії).
 * - Аналітику та статистику для адмін-панелі.
 */
class DatabaseManager {
public:
    // --- Конструктори та Базові операції ---

    /**
     * @brief Конструктор DatabaseManager.
     *
     * Налаштовує драйвер QSQLITE та встановлює ім'я файлу бази даних (dating_agency.db).
     * Саме з'єднання відкривається методом openDatabase().
     */
    DatabaseManager();

    /**
     * @brief Деструктор. Закриває з'єднання з базою даних.
     */
    ~DatabaseManager();

    /**
     * @brief Відкриває з'єднання з базою даних.
     *
     * Якщо файлу не існує, він буде створений. Також викликає createTables()
     * для ініціалізації структури, якщо це перший запуск.
     * @return true, якщо з'єднання успішне.
     */
    bool openDatabase();

    /**
     * @brief Закриває поточне з'єднання з базою даних.
     */
    void closeDatabase();

    /**
     * @brief Створює необхідні таблиці та індекси, якщо вони відсутні.
     *
     * Створює таблиці: users, likes, chat_messages, user_tags.
     * @return true, якщо SQL-запити виконано успішно.
     */
    bool createTables();

    // --- CRUD Операції (Профіль) ---

    /**
     * @brief Зберігає нового користувача в базі даних.
     * @param profile Об'єкт з даними користувача.
     * @return int ID новоствореного запису або -1 у разі помилки.
     */
    int saveProfile(const UserProfile &profile);

    /**
     * @brief Завантажує профіль, шукаючи його за email.
     *
     * Використовується при авторизації або реєстрації (перевірка унікальності).
     * @param email Email для пошуку.
     * @param profile Посилання на об'єкт, куди будуть записані дані.
     * @return true, якщо профіль знайдено.
     */
    bool loadProfileByEmail(const QString &email, UserProfile &profile);

    /**
     * @brief Завантажує профіль за унікальним ID (Primary Key).
     *
     * Найшвидший спосіб отримання даних.
     * @param userId ID користувача.
     * @param profile Посилання на об'єкт для запису результату.
     * @return true, якщо профіль знайдено.
     */
    bool loadProfileById(int userId, UserProfile& profile);

    /**
     * @brief Перевіряє наявність профілю з вказаним email.
     * @return true, якщо email вже зайнятий.
     */
    bool profileExists(const QString &email) const;

    /**
     * @brief Оновлює дані існуючого профілю.
     *
     * Перезаписує всі поля в рядку таблиці, що відповідає profile.getId().
     * @param profile Оновлений об'єкт профілю.
     * @return true, якщо оновлення успішне.
     */
    bool updateProfile(const UserProfile &profile);

    /**
     * @brief Видаляє профіль користувача.
     * @param profileId ID профілю для видалення.
     * @return true, якщо видалення успішне.
     */
    bool deleteProfile(int profileId);

    /**
     * @brief Змінює видимість профілю у пошуку.
     * @param profileId ID користувача.
     * @param isHidden true - приховати (пауза), false - показувати.
     * @return true, якщо статус змінено.
     */
    bool setProfileHidden(int profileId, bool isHidden);

    // --- Отримання Профілів (Пошук) ---

    /**
     * @brief Шукає користувачів за вказаними критеріями (фільтрами).
     *
     * Виключає з результатів:
     * 1. Самого користувача (currentUserId).
     * 2. Приховані профілі (is_hidden = 1).
     *
     * @param prefs Об'єкт з налаштуваннями фільтрації (вік, місто, стать).
     * @param currentUserId ID того, хто шукає.
     * @return QList<UserProfile> Список знайдених профілів.
     */
    QList<UserProfile> getProfilesByCriteria(const Preference &prefs, int currentUserId);

    /**
     * @brief Отримує список ВСІХ профілів (для налагодження або FakeData).
     */
    QList<UserProfile> getAllProfiles();

    /**
     * @brief Завантажує список профілів за списком їх ID.
     *
     * Використовується для відображення списку "Метчів" (де ми знаємо тільки ID).
     * Виконує запит "WHERE id IN (...)".
     */
    QList<UserProfile> getProfilesByIds(const QList<int> &ids);

    /**
     * @brief Повертає список всіх унікальних міст, що є в базі.
     * Використовується для автодоповнення (QCompleter).
     */
    QStringList getAllCities();

    // --- Адмінка та Статистика ---

    /**
     * @brief Створює та налаштовує SQL-модель для QTableView.
     * Дозволяє редагувати таблицю 'users' напряму в адмінці.
     */
    QSqlTableModel* getUsersModel(QObject* parent = nullptr);

    /**
     * @brief Отримує статистику по містах (Топ міст).
     * @return Map {Назва міста -> Кількість користувачів}.
     */
    QMap<QString, int> getCityStatistics();

    /**
     * @brief Отримує статистику по статі.
     * @return Map {Стать -> Кількість}.
     */
    QMap<QString, int> getGenderStatistics();

    /**
     * @brief Отримує статистику по вікових групах.
     * @return Map {Діапазон віку -> Кількість}.
     */
    QMap<QString, int> getAgeStatistics();

    /**
     * @brief Масове збереження профілів (оптимізовано транзакцією).
     * Використовується FakeDataManager для швидкої генерації.
     */
    bool saveProfileBulk(const QList<UserProfile> &profiles);

    /**
     * @brief Повертає загальну кількість зареєстрованих користувачів.
     */
    int countUsers();

    // --- LIKES / MATCHES / СЕСІЯ ---

    /**
     * @brief Додає запис про лайк (user -> target).
     * @return true, якщо лайк успішно додано (або він вже існував).
     */
    bool addLike(int fromUserId, int toUserId);

    /**
     * @brief Видаляє лайк (дизлайк / розірвання пари).
     */
    bool removeLike(int fromUserId, int toUserId);

    /**
     * @brief Логує факт виникнення метчу (не створює окрему таблицю, просто лог).
     */
    bool addMatch(int userId, int targetId);

    /**
     * @brief Перевіряє, чи ставив userId лайк користувачу targetId.
     */
    bool hasUserLiked(int userId, int targetId) const;

    /**
     * @brief Перевіряє наявність взаємного лайку (Метчу).
     * @return true, якщо user1 лайкнув user2, І user2 лайкнув user1.
     */
    bool isMutualLike(int user1, int user2) const;

    /**
     * @brief Знаходить ID всіх користувачів, з якими у currentUserId є метч.
     *
     * Виконує SQL JOIN таблиці лайків самої на себе.
     * @return QList<int> Список ID "друзів".
     */
    QList<int> getMutualMatchIds(int currentUserId);

    /**
     * @brief Завантажує профіль поточного активного користувача.
     *
     * Використовує збережений ID в QSettings для визначення "хто я".
     */
    bool getCurrentUserProfile(UserProfile &profile);

    /**
     * @brief Зберігає ID залогіненого користувача в налаштуваннях (QSettings).
     * Емуляція сесії між запусками програми.
     */
    void saveCurrentUserId(int userId);

    /**
     * @brief Зчитує збережений ID користувача з налаштувань.
     */
    int loadCurrentUserId() const;

    // --- Робота з тегами ---

    /**
     * @brief Додає тег до профілю.
     */
    bool addTag(int userId, const QString& tag);

    /**
     * @brief Видаляє конкретний тег у користувача.
     */
    bool removeTag(int userId, const QString& tag);

    /**
     * @brief Отримує список всіх тегів користувача.
     */
    QList<QString> getTagsForUser(int userId) const;

    /**
     * @brief Видаляє ВСІ теги користувача (перед перезаписом).
     */
    bool removeAllTags(int userId);

    // --- Чат ---

    /**
     * @brief Зберігає нове повідомлення в таблиці chat_messages.
     */
    bool saveChatMessage(const ChatMessage& message);

    /**
     * @brief Завантажує повну історію листування між двома користувачами.
     * Сортує повідомлення за часом.
     */
    QList<ChatMessage> loadChatHistory(int user1Id, int user2Id);

    /**
     * @brief Отримує час останнього повідомлення в діалозі.
     * Може використовуватись для сортування списку чатів.
     */
    QDateTime getChatLastTimestamp(int user1Id, int user2Id) const;

private:
    QSqlDatabase m_db;                  ///< Об'єкт підключення до бази даних Qt.
    const QString DB_NAME = "dating_agency.db"; ///< Ім'я файлу бази даних SQLite.
};

#endif // DATABASEMANAGER_H