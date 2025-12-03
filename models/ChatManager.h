#pragma once
#include "ChatStructures.h"

// Forward declaration для уникнення циклічних залежностей
class DatabaseManager;

/**
 * @brief Менеджер чатів та повідомлень.
 *
 * Цей клас відповідає за логіку обміну повідомленнями між користувачами.
 * Він виступає прошарком між інтерфейсом чату (ChatPageWidget) та базою даних.
 *
 * Основні функції:
 * - Надсилання та збереження нових повідомлень.
 * - Завантаження історії листування для конкретної пари користувачів.
 * - Генерація автоматичних відповідей від "бота" (для симуляції спілкування з фейковими акаунтами).
 */
class ChatManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Конструктор ChatManager.
     *
     * Ініціалізує менеджер та завантажує список заготовок для відповідей бота.
     *
     * @param dbManager Вказівник на менеджер бази даних (для збереження/завантаження історії).
     * @param parent Батьківський об'єкт QObject (зазвичай MainWindow або nullptr).
     */
    explicit ChatManager(DatabaseManager* dbManager, QObject* parent = nullptr);

    /**
     * @brief Надсилає повідомлення від одного користувача іншому.
     *
     * Створює об'єкт ChatMessage з поточним часом та зберігає його в базі даних
     * через DatabaseManager::saveChatMessage.
     *
     * @param fromUserId ID відправника.
     * @param toUserId ID отримувача.
     * @param message Текст повідомлення.
     */
    void sendMessage(int fromUserId, int toUserId, const QString &message);

    /**
     * @brief Отримує історію повідомлень між двома користувачами.
     *
     * Завантажує з бази даних усі повідомлення, де один користувач є відправником,
     * а інший - отримувачем (і навпаки), відсортовані за часом.
     *
     * @param userId ID першого користувача (зазвичай, поточний користувач).
     * @param targetId ID другого користувача (співрозмовник).
     * @return QList<ChatMessage> Список повідомлень у хронологічному порядку.
     */
    QList<ChatMessage> getMessages(int userId, int targetId) const;

    /**
     * @brief Генерує випадкову відповідь від бота.
     *
     * Використовується, коли користувач пише повідомлення фейковому акаунту.
     * Обирає випадкову фразу із заздалегідь підготовленого списку botReplies.
     *
     * @return QString Текст відповіді.
     */
    QString getBotReply() const;

private:
    QList<QString> botReplies;      ///< Список заготовлених фраз для автоматичних відповідей.
    DatabaseManager* m_dbManager;   ///< Посилання на базу даних для збереження історії.
};