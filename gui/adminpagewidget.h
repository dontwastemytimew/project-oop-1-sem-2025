#ifndef ADMINPAGEWIDGET_H
#define ADMINPAGEWIDGET_H

#include "DatabaseManager.h"
#include <QWidget>

// Forward declarations для зменшення часу компіляції
class QTableView;
class QSqlTableModel;
class QPushButton;
class QLineEdit;
class QSortFilterProxyModel;
class StatsDialog;
class QLabel;

/**
 * @brief Віджет Панелі Адміністратора.
 *
 * Цей клас надає інтерфейс для управління базою даних користувачів.
 * Основні можливості:
 * - Перегляд усіх користувачів у вигляді таблиці (QTableView).
 * - Сортування та фільтрація (пошук) користувачів за ім'ям або містом.
 * - Видалення обраних акаунтів.
 * - Генерація фейкових користувачів для тестування (через FakeDataManager).
 * - Перегляд статистики (гендерний розподіл, вік, міста).
 * - Приховування/відображення профілів через контекстне меню.
 */
class AdminPageWidget : public QWidget {
    Q_OBJECT
public:
    /**
     * @brief Конструктор AdminPageWidget.
     * @param parent Батьківський віджет (зазвичай MainWindow).
     */
    explicit AdminPageWidget(QWidget *parent = nullptr);

    /**
     * @brief Ініціалізує менеджер бази даних та налаштовує моделі таблиці.
     *
     * Метод створює QSqlTableModel, прив'язує його до таблиці користувачів
     * та налаштовує QSortFilterProxyModel для можливості пошуку/фільтрації.
     *
     * @param dbManager Вказівник на менеджер бази даних.
     */
    void setDatabaseManager(DatabaseManager* dbManager);

    /**
     * @brief Оновлює дані в таблиці.
     *
     * Виконує повторний запит до бази даних (select()) для відображення
     * актуальної інформації (наприклад, після генерації нових користувачів).
     */
    void refreshTable();

protected:
    /**
     * @brief Обробник подій зміни стану віджета.
     *
     * Перевизначений для підтримки динамічної зміни мови (QEvent::LanguageChange).
     * Викликає retranslateUi() при зміні мови додатку.
     *
     * @param event Подія, що сталася.
     */
    void changeEvent(QEvent *event) override;

signals:
    /**
     * @brief Сигнал, що емітується при натисканні кнопки "Назад".
     * Використовується для навігації (повернення до налаштувань або головного меню).
     */
    void backClicked();

private slots:
    /**
     * @brief Слот видалення обраного користувача.
     *
     * Викликається при натисканні кнопки "Delete". Видаляє виділений рядок
     * з бази даних після підтвердження у діалоговому вікні.
     */
    void onDeleteClicked();

    /**
     * @brief Слот генерації тестових даних.
     *
     * Викликає FakeDataManager для створення нових випадкових профілів
     * та оновлює таблицю.
     */
    void onGenerateClicked();

    /**
     * @brief Виклик контекстного меню таблиці (правий клік).
     *
     * Дозволяє виконати дії над конкретним рядком, наприклад, "Приховати/Показати" профіль.
     * @param pos Позиція курсора миші.
     */
    void onCustomContextMenu(const QPoint &pos);

    /**
     * @brief Відкриває діалогове вікно статистики.
     */
    void onStatsClicked();

private:
    DatabaseManager* m_dbManager;           ///< Посилання на бекенд бази даних.
    QTableView* m_tableView;                ///< Таблиця для відображення даних.
    QSqlTableModel* m_model;                ///< SQL модель для прямого доступу до таблиці 'users'.
    QSortFilterProxyModel* m_proxyModel;    ///< Проксі-модель для фільтрації та сортування даних.

    /**
     * @brief Перемикає статус видимості профілю (is_hidden).
     * @param row Номер рядка в проксі-моделі, для якого змінюється статус.
     */
    void toggleHiddenStatus(int row);

    // Елементи інтерфейсу
    QPushButton* m_btnDelete;       ///< Кнопка видалення.
    QPushButton* m_btnGenerate;     ///< Кнопка генерації фейків.
    QPushButton* m_btnBack;         ///< Кнопка навігації назад.
    QLineEdit* m_searchField;       ///< Поле вводу для пошуку/фільтрації.
    QPushButton* m_btnStats;        ///< Кнопка відкриття статистики.
    QLabel* m_titleLabel;           ///< Заголовок сторінки.
    QLabel* m_searchIconLabel;      ///< Іконка пошуку.

    /**
     * @brief Оновлює тексти інтерфейсу при зміні мови.
     */
    void retranslateUi();
};

#endif // ADMINPAGEWIDGET_H