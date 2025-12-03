#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include "UserProfile.h"
#include "ChatManager.h"

/**
 * @brief Віджет сторінки чату (діалогу).
 *
 * Цей клас реалізує інтерфейс для листування з конкретним користувачем ("Метчем").
 * Він відображає історію повідомлень, поле для введення нового тексту та
 * кнопку відправки.
 *
 * Також містить логіку симуляції "живої" розмови (отримання відповідей від бота).
 */
class ChatPageWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор ChatPageWidget.
     *
     * Налаштовує розмітку (layout), створює віджети для відображення тексту
     * та вводу, а також підключає сигнали до слотів (кнопка Send, натискання Enter).
     *
     * @param matchProfile Профіль співрозмовника.
     * @param chatManager Вказівник на менеджер чатів.
     * @param currentUserId ID поточного користувача (відправника).
     * @param parent Батьківський віджет.
     */
    explicit ChatPageWidget(const UserProfile& matchProfile, ChatManager* chatManager, int currentUserId, QWidget *parent = nullptr);

    /**
     * @brief Оновлює контекст чату для нового співрозмовника.
     *
     * Цей метод викликається, коли користувач перемикається між різними чатами
     * у списку метчів. Він очищає поле вводу та область повідомлень,
     * оновлює заголовок (ім'я співрозмовника) та завантажує історію з БД.
     *
     * @param matchProfile Профіль нового співрозмовника.
     * @param currentUserId ID поточного користувача (щоб переконатися в коректності сесії).
     */
    void setMatchProfile(const UserProfile& matchProfile, int currentUserId);

private slots:
    /**
     * @brief Слот відправки повідомлення.
     *
     * Викликається при натисканні кнопки "Надіслати" або клавіші Enter.
     * 1. Зчитує текст з поля вводу.
     * 2. Передає його в ChatManager для збереження в БД.
     * 3. Додає повідомлення у вікно чату (візуально).
     * 4. Запускає таймер для емуляції відповіді бота (через кілька секунд).
     */
    void sendMessage();

    /**
     * @brief Слот генерації відповіді від бота.
     *
     * Викликається за таймером після того, як користувач щось написав.
     * Отримує випадкову фразу з ChatManager та додає її у вікно чату
     * від імені співрозмовника.
     */
    void botReply();

private:
    /**
     * @brief Завантажує історію повідомлень з бази даних.
     *
     * Використовує ChatManager::getMessages для отримання списку повідомлень
     * між currentUserId та matchProfile.id, форматує їх та виводить у m_chatArea.
     */
    void loadHistory();

private:
    UserProfile m_matchProfile;     ///< Профіль користувача, з яким ведеться розмова.
    ChatManager* m_chatManager;     ///< Менеджер для роботи з повідомленнями.

    QTextEdit* m_chatArea;          ///< Область відображення історії (тільки для читання).
    QLineEdit* m_inputField;        ///< Поле для введення нового повідомлення.
    QPushButton* m_sendButton;      ///< Кнопка відправки.
    int m_currentUserId;            ///< ID поточного користувача ("Я").
};

#endif // CHATWINDOW_H