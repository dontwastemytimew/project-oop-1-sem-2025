#include "welcomepagewidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QFont>
#include <QCoreApplication>

#define CODE_TEXT_FINAL \
    "// Starting Match++ core system...\n" \
    "// System integrity check: OK\n" \
    "template<typename T>\n" \
    "class Matcher {\n" \
    "  public:\n" \
    "    bool is_compatible = true;\n" \
    "    void match_check(T user) { /* ... */ }\n" \
    "};"

#define WELCOME_TEXT_HTML_FORMAT \
    "<div align='center'>" \
    "<span style='font-size: 28pt; font-weight: bold; color: #9932CC;'>%1</span><br/>" \
    "<span style='font-size: 14pt; color: #555555; margin-top: 10px;'>%2</span>" \
    "</div>"

WelcomePageWidget::WelcomePageWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 50, 50, 50);

    m_welcomeTextLabel = new QLabel(this);
    m_welcomeTextLabel->setObjectName("welcomeArea");

    m_welcomeTextLabel->setAlignment(Qt::AlignCenter);
    m_welcomeTextLabel->setWordWrap(true);

    QFont font("Consolas", 18);
    m_welcomeTextLabel->setFont(font);
    m_welcomeTextLabel->setStyleSheet("color: #9932CC;");
    m_welcomeTextLabel->setTextFormat(Qt::PlainText);

    mainLayout->addStretch();
    mainLayout->addWidget(m_welcomeTextLabel);
    mainLayout->addStretch();

    setLayout(mainLayout);

    m_fullText = QString(CODE_TEXT_FINAL);

    m_animationTimer = new QTimer(this);
    m_animationTimer->setInterval(15);

    connect(m_animationTimer, &QTimer::timeout, this, &WelcomePageWidget::updateTextAnimation);

    m_animationTimer->start();
}

        void WelcomePageWidget::updateTextAnimation() {
    if (m_currentIndex < m_fullText.length()) {
        QString currentText = m_fullText.left(m_currentIndex + 1);
        m_welcomeTextLabel->setText(currentText);
        m_currentIndex++;
    } else {
        m_animationTimer->stop();

        QString titleText = tr("Вітаємо у Match++!");
        QString subtitleText = tr("Оберіть 'Профіль' для налаштування або 'Пошук', щоб почати");

        QString finalHtml = QString(WELCOME_TEXT_HTML_FORMAT)
                                .arg(titleText)
                                .arg(subtitleText);

        m_welcomeTextLabel->setText(finalHtml);
        m_welcomeTextLabel->setTextFormat(Qt::RichText);
        m_welcomeTextLabel->setStyleSheet("");
        m_welcomeTextLabel->setAlignment(Qt::AlignCenter);
    }
}