#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QtCharts>
#include <QDialog>
#include "DatabaseManager.h"

/**
 * @brief Діалогове вікно статистики та аналітики.
 *
 * Цей клас відповідає за візуалізацію даних про користувачів, зареєстрованих у системі.
 * Використовує модуль QtCharts для побудови графіків та діаграм.
 *
 * Статистика розділена на вкладки (Tabs):
 * - Гендерний розподіл (Pie Chart).
 * - Популярні міста (Bar/Pie Chart).
 * - Вікові групи (Bar Chart).
 */
class StatsDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор StatsDialog.
     *
     * Ініціалізує вікно, завантажує дані з бази через DatabaseManager
     * та будує всі графіки.
     *
     * @param db Вказівник на менеджер бази даних (джерело статистики).
     * @param parent Батьківський віджет.
     */
    explicit StatsDialog(DatabaseManager* db, QWidget *parent = nullptr);

protected:
    /**
     * @brief Обробник подій зміни стану (зокрема, зміни мови).
     *
     * Перемальовує заголовки графіків та назви вкладок при зміні мови додатку.
     * @param event Подія.
     */
    void changeEvent(QEvent *event) override;

private:
    /**
     * @brief Налаштовує віджет вкладок (QTabWidget).
     *
     * Створює вкладки "Гендер", "Міста", "Вік" та додає до них
     * відповідні віджети графіків.
     */
    void setupTabs();

    /**
     * @brief Застосовує тему оформлення до графіків.
     *
     * Налаштовує кольори, шрифти та згладжування (Antialiasing) для
     * покращення візуального вигляду діаграм.
     */
    void applyChartTheme();

    /**
     * @brief Оновлює тексти інтерфейсу при зміні мови.
     */
    void retranslateUi();

    /**
     * @brief Створює віджет з діаграмою гендерного розподілу.
     * @return QWidget* Віджет, що містить QChartView з круговою діаграмою (Pie Chart).
     */
    QWidget* createGenderChart();

    /**
     * @brief Створює віджет з графіком по містах.
     * @return QWidget* Віджет, що містить графік розподілу користувачів по містах.
     */
    QWidget* createCityChart();

    /**
     * @brief Створює віджет з гістограмою віку.
     *
     * Розподіляє користувачів по вікових групах (наприклад, 18-25, 26-35 тощо).
     * @return QWidget* Віджет, що містить стовпчасту діаграму (Bar Chart).
     */
    QWidget* createAgeChart();

    DatabaseManager* m_db;          ///< Посилання на базу даних для отримання статистики.
    QVBoxLayout* m_mainLayout;      ///< Головний компонувальник вікна.
    QTabWidget* m_tabWidget;        ///< Віджет вкладок для перемикання між графіками.

    // Графіки
    QChart* m_genderChart;          ///< Об'єкт графіка гендеру.
    QChart* m_cityChart;            ///< Об'єкт графіка міст.
    QChart* m_ageChart;             ///< Об'єкт графіка віку.

    QBarSet* m_ageBarSet;           ///< Набір даних для стовпчастої діаграми віку (потрібен для оновлення).
};

#endif // STATSDIALOG_H