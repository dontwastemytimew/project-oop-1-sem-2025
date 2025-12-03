#ifndef PROFILEPAGEWIDGET_H
#define PROFILEPAGEWIDGET_H

#include "DatabaseManager.h"
#include "UserProfile.h"
#include <QWidget>

// Forward declarations UI класів
class QLineEdit;
class QSpinBox;
class QTextEdit;
class QPushButton;
class QComboBox;
class QLabel;

/**
 * @brief Віджет сторінки "Мій Профіль" (Редагування).
 *
 * Цей клас надає інтерфейс для створення нового або редагування існуючого
 * профілю користувача. Він містить форму з полями для введення особистих даних
 * (ім'я, вік, місто, біографія, теги) та функціонал для завантаження фото.
 *
 * Основні можливості:
 * - Валідація вводу (обов'язкові поля, перевірка віку).
 * - Автодоповнення для поля "Місто".
 * - Вибір фотографії з файлової системи.
 * - Збереження або оновлення даних у базі через DatabaseManager.
 */
class ProfilePageWidget : public QWidget {
    Q_OBJECT
public:
    /**
     * @brief Конструктор ProfilePageWidget.
     * @param parent Батьківський віджет.
     */
    explicit ProfilePageWidget(QWidget *parent = nullptr);

    /**
     * @brief Встановлює залежність від менеджера бази даних.
     * @param dbManager Вказівник на DatabaseManager.
     */
    void setDatabaseManager(DatabaseManager* dbManager);

    /**
     * @brief Завантажує дані поточного користувача з бази даних у поля форми.
     *
     * Використовує збережений ID користувача (з QSettings) для отримання
     * актуальної інформації та заповнення віджетів (QLineEdit, QComboBox тощо).
     */
    void loadCurrentProfile();

    /**
     * @brief Встановлює об'єкт профілю вручну.
     *
     * Використовується для ініціалізації віджета даними, які вже були завантажені
     * в MainWindow, щоб уникнути зайвого запиту до БД.
     * @param profile Об'єкт профілю користувача.
     */
    void setInternalProfile(const UserProfile& profile);

    /**
     * @brief Очищає всі поля форми та скидає шлях до фото.
     *
     * Використовується після видалення профілю або при виході з акаунту,
     * щоб підготувати форму для нового користувача.
     */
    void clearFields();

protected:
    /**
     * @brief Обробник подій зміни стану (зокрема, зміни мови).
     * @param event Подія.
     */
    void changeEvent(QEvent *event) override;

signals:
    /**
     * @brief Сигнал про успішне збереження профілю.
     *
     * Емітується після того, як дані пройшли валідацію та були успішно
     * записані в базу даних. MainWindow використовує цей сигнал для навігації.
     */
 void profileSaved(const UserProfile& savedProfile);

private slots:
    /**
     * @brief Слот обробки натискання кнопки "Зберегти".
     *
     * Збирає дані з усіх полів введення, формує об'єкт UserProfile,
     * виконує перевірку коректності (email, вік > 18) та викликає
     * DatabaseManager::saveProfile або updateProfile.
     */
    void on_btn_SaveProfile_clicked();

    /**
     * @brief Слот для вибору фотографії профілю.
     *
     * Відкриває системний діалог вибору файлу (QFileDialog), дозволяє обрати
     * зображення, зберігає шлях до нього у m_photoPath та відображає прев'ю.
     */
    void onChoosePhoto();

private:
    DatabaseManager* m_dbManager;   ///< Посилання на базу даних.
    UserProfile m_currentUser;      ///< Локальна копія даних поточного користувача.
    QString m_photoPath;            ///< Шлях до обраного файлу фотографії.

    // --- Поля введення даних ---
    QLineEdit* m_nameEdit;          ///< Поле введення імені.
    QSpinBox* m_ageSpinBox;         ///< Спінбокс для вибору віку.
    QLineEdit* m_cityEdit;          ///< Поле введення міста (з автодоповненням).
    QTextEdit* m_bioEdit;           ///< Текстова область для біографії.
    QLineEdit* m_phoneEdit;         ///< Поле введення телефону.
    QLineEdit* m_emailEdit;         ///< Поле введення Email.
    QComboBox* m_genderCombo;       ///< Вибір статі.
    QComboBox* m_orientationCombo;  ///< Вибір орієнтації.
    QLineEdit* m_tagsEdit;          ///< Поле для введення тегів (розділених комами).

    // --- Елементи керування ---
    QLabel* m_photoLabel;           ///< Лейбл для попереднього перегляду фото.
    QPushButton* m_choosePhotoButton; ///< Кнопка виклику діалогу вибору фото.
    QPushButton* m_saveButton;      ///< Кнопка підтвердження змін.

    /**
     * @brief Налаштовує QCompleter для поля введення міста.
     *
     * Завантажує список унікальних міст з бази даних для підказок при введенні.
     */
    void setupCityAutocomplete();

    /**
     * @brief Оновлює тексти інтерфейсу (лейбли) при зміні мови.
     */
    void retranslateUi();

    // --- Лейбли (текстові підписи до полів) ---
    QLabel* m_labelPhotoTitle;
    QLabel* m_labelName;
    QLabel* m_labelAge;
    QLabel* m_labelCity;
    QLabel* m_labelEmail;
    QLabel* m_labelPhone;
    QLabel* m_labelGender;
    QLabel* m_labelOrientation;
    QLabel* m_labelTags;
    QLabel* m_labelBio;
};

#endif // PROFILEPAGEWIDGET_H