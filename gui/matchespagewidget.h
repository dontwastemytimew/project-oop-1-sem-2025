#ifndef MATCHESPAGEWIDGET_H
#define MATCHESPAGEWIDGET_H

#include <QListWidget>
#include <QLabel>
#include "DatabaseManager.h"
#include "ChatManager.h"

/**
 * @brief Віджет сторінки "Метчі" (Взаємні лайки).
 *
 * Цей клас відповідає за відображення списку користувачів, з якими у
 * поточного користувача виникла взаємна симпатія ("Метч").
 *
 * Основні можливості:
 * - Відображення списку контактів з іменами та фото.
 * - Пошук/фільтрація серед метчів за іменем.
 * - Перехід до чату при кліку на користувача.
 * - Автоматичне оновлення списку при виникненні нового метчу.
 */
class MatchesPageWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор MatchesPageWidget.
     * @param parent Батьківський віджет.
     */
    explicit MatchesPageWidget(QWidget *parent = nullptr);

    /**
     * @brief Деструктор.
     */
    ~MatchesPageWidget();

    /**
     * @brief Встановлює залежність від менеджера бази даних.
     * @param db Вказівник на DatabaseManager.
     */
    void setDatabaseManager(DatabaseManager* db);

    /**
     * @brief Встановлює ID поточного авторизованого користувача.
     *
     * Необхідно для завантаження саме його списку метчів.
     * @param id ID користувача.
     */
    void setCurrentUserId(int id);

    /**
     * @brief Завантажує та відображає список метчів з бази даних.
     *
     * Метод очищає поточний список (QListWidget), робить запит до БД
     * через DatabaseManager::getSortedMatches (або getMutualMatchIds)
     * та створює елементи списку (QListWidgetItem) для кожного партнера.
     */
    void reloadMatches();

    /**
     * @brief Встановлює залежність від менеджера чатів.
     * @param chatManager Вказівник на ChatManager (може використовуватись для отримання останнього повідомлення).
     */
    void setChatManager(ChatManager* chatManager);

public slots:
    /**
     * @brief Слот, що реагує на створення нового метчу в реальному часі.
     *
     * Зазвичай підключається до сигналу з SearchPageWidget.
     * Якщо userId або targetId співпадають з поточним користувачем, список оновлюється.
     *
     * @param userId ID користувача, який поставив лайк.
     * @param targetId ID користувача, який отримав лайк (і відповів взаємністю).
     */
    void onMatchCreated(int userId, int targetId);

signals:
    /**
     * @brief Сигнал запиту на відкриття чату.
     *
     * Емітується, коли користувач обирає когось зі списку метчів.
     * Обробляється в MainWindow для перемикання на сторінку чату.
     *
     * @param targetUserId ID співрозмовника.
     */
    void openChatRequested(int targetUserId);

protected:
    /**
     * @brief Обробник подій зміни стану (зокрема, зміни мови).
     * @param event Подія.
     */
    void changeEvent(QEvent *event) override;

private slots:
    /**
     * @brief Обробка кліку на елемент списку.
     *
     * Отримує ID користувача, збережений у QListWidgetItem (Qt::UserRole),
     * та емітує сигнал openChatRequested.
     *
     * @param item Елемент списку, на який натиснули.
     */
    void onMatchClicked(QListWidgetItem* item);

    /**
     * @brief Обробка зміни тексту в полі пошуку.
     *
     * Фільтрує відображені елементи списку, приховуючи ті, що не містять
     * введеного тексту в імені.
     *
     * @param text Введений текст пошуку.
     */
    void onSearchTextChanged(const QString &text);

private:
    /**
     * @brief Оновлює тексти інтерфейсу при зміні мови.
     */
    void retranslateUi();

    int m_currentUserId = -1;           ///< ID поточного користувача.
    DatabaseManager* m_db = nullptr;    ///< Посилання на базу даних.
    ChatManager* m_chatManager = nullptr; ///< Посилання на менеджер чатів.

    // Елементи інтерфейсу
    QLabel* m_titleLabel;       ///< Заголовок сторінки.
    QLineEdit* m_searchField;   ///< Поле для пошуку серед контактів.
    QListWidget* m_list;        ///< Список для відображення карток користувачів.
};

#endif // MATCHESPAGEWIDGET_H