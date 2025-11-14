#ifndef USERLOGGER_H
#define USERLOGGER_H

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

/**
 * @brief Перерахування для рівнів логування.
 * Визначає важливість повідомлення.
 */
enum LogLevel {
    Debug,    // Детальна інформація для налагодження
    Info,     // Стандартні інформаційні повідомлення (напр., "Додаток запущено")
    Warning,  // Попередження (напр., "Файл не знайдено, але створено новий")
    Error,    // Помилка, яку можна обробити (напр., "Не вдалося зберегти профіль")
    Critical  // Критична помилка, що може вплинути на роботу (напр., "БД не підключено")
};

/**
 * @brief Статичний клас UserLogger
 *
 * Забезпечує єдиний механізм логування подій у консоль та у файл.
 * Всі методи є статичними, тому не потрібно створювати екземпляр класу.
 */
class UserLogger {
public:
    /**
     * @brief Ініціалізує логер.
     * Відкриває файл логів для запису. Цей метод має бути викликаний один раз при старті програми.
     * @param fileName Ім'я файлу логів (за замовчуванням "app_log.txt").
     */
    static void init(const QString &fileName = "app_log.txt");

    /**
     * @brief Записує повідомлення у лог.
     * Одночасно виводить повідомлення у qDebug() (консоль) та у файл логів.
     * @param level Рівень важливості повідомлення (Debug, Info, etc.).
     * @param message Текст повідомлення для запису.
     */
    static void log(LogLevel level, const QString &message);

private:
    /// Вказівник на об'єкт файлу логів.
    static QFile *logFile;

    /// Прапорець, що показує, чи був логер успішно ініціалізований.
    static bool initialized;
};

#endif //USERLOGGER_H
