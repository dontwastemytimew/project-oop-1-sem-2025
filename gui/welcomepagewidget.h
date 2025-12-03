#ifndef WELCOMEPAGEWIDGET_H
#define WELCOMEPAGEWIDGET_H

#include <QTimer>
#include <QLabel>
#include <QWidget>

/**
 * @brief Віджет привітальної сторінки (Welcome Screen).
 *
 * Це перша сторінка, яку бачить користувач при запуску додатка.
 * Головна особливість — анімація тексту в стилі "друкарської машинки" (Typewriter Effect),
 * що імітує введення коду або команд у терміналі, підкреслюючи IT-тематику Match++.
 */
class WelcomePageWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор WelcomePageWidget.
     *
     * Налаштовує розмітку, шрифти (моноширинні) та ініціалізує таймер
     * для запуску текстової анімації.
     *
     * @param parent Батьківський віджет.
     */
    explicit WelcomePageWidget(QWidget *parent = nullptr);

protected:
    /**
     * @brief Обробник подій зміни стану (зокрема, зміни мови).
     *
     * При зміні мови перезапускає анімацію з новим перекладеним текстом.
     * @param event Подія.
     */
    void changeEvent(QEvent *event) override;

    private slots:
        /**
         * @brief Слот для оновлення тексту анімації.
         *
         * Викликається за сигналом таймера (наприклад, кожні 100 мс).
         * Додає наступний символ з повного рядка `m_fullText` до лейбла на екрані.
         * Коли текст надруковано повністю, зупиняє таймер.
         */
        void updateTextAnimation();

private:
    /**
     * @brief Оновлює тексти інтерфейсу при зміні мови та скидає анімацію.
     */
    void retranslateUi();

    QLabel* m_welcomeTextLabel; ///< Лейбл для відображення анімованого тексту.
    QTimer* m_animationTimer;   ///< Таймер, що керує швидкістю "друку".
    QString m_fullText;         ///< Повний рядок тексту, який потрібно відобразити (наприклад, "Welcome to Match++...").
    int m_currentIndex = 0;     ///< Індекс поточного символу, який друкується.
    bool m_animationFinished;   ///< Прапорець, що вказує, чи завершилась анімація (щоб не запускати повторно без потреби).
};

#endif //WELCOMEPAGEWIDGET_H