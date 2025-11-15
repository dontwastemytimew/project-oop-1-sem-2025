#include "searchpagewidget.h"
#include "MatchEngine.h"
#include "Preference.h"
#include "UserLogger.h"
#include "profilecard.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QStackedWidget>

SearchPageWidget::SearchPageWidget(QWidget *parent)
: QWidget(parent), m_dbManager(nullptr), m_currentMatchIndex(0)
{
    // Створюємо UI для Фільтрів
    QFormLayout* filterLayout = new QFormLayout();
    m_minAgeSpin = new QSpinBox(this);
    m_maxAgeSpin = new QSpinBox(this);
    m_minAgeSpin->setRange(18, 99);
    m_maxAgeSpin->setRange(18, 99);
    m_maxAgeSpin->setValue(99);

    m_cityEdit = new QLineEdit(this);
    m_findButton = new QPushButton(tr("Знайти пару"), this);

    filterLayout->addRow(tr("Мін. вік:"), m_minAgeSpin);
    filterLayout->addRow(tr("Макс. вік:"), m_maxAgeSpin);
    filterLayout->addRow(tr("Місто:"), m_cityEdit);

    m_resultsStack = new QStackedWidget(this);
    m_resultsStack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Додаємо стартову "заглушку" в стопку
    QLabel* placeholder = new QLabel(tr("Натисніть 'Знайти пару', щоб побачити профілі"), this);
    placeholder->setAlignment(Qt::AlignCenter);
    m_resultsStack->addWidget(placeholder);

    m_likeButton = new QPushButton(tr(" Like"), this);
    m_likeButton->setIcon(QIcon(":/resources/icons/like-white.png"));
    m_likeButton->setObjectName("m_likeButton");

    m_skipButton = new QPushButton(tr(" Skip"), this);
    m_skipButton->setIcon(QIcon(":/resources/icons/skip-white.png"));
    m_skipButton->setObjectName("m_skipButton");

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_skipButton, 1);
    buttonLayout->addWidget(m_likeButton, 1);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(filterLayout);  // 1. Фільтри
    mainLayout->addWidget(m_findButton);  // 2. Кнопка "Знайти"
    mainLayout->addWidget(m_resultsStack); // 3. Область для карток
    mainLayout->addLayout(buttonLayout);   // 4. Кнопки "Like/Skip"

    setLayout(mainLayout);

    connect(m_findButton, &QPushButton::clicked, this, &SearchPageWidget::on_btn_Find_clicked);
    connect(m_likeButton, &QPushButton::clicked, this, &SearchPageWidget::on_Like_clicked);
    connect(m_skipButton, &QPushButton::clicked, this, &SearchPageWidget::on_Skip_clicked);
}

void SearchPageWidget::setDatabaseManager(DatabaseManager* dbManager) {
    m_dbManager = dbManager;
}

void SearchPageWidget::on_btn_Find_clicked() {
    if (!m_dbManager) {
        UserLogger::log(Error, "SearchPage: DatabaseManager is not set!");
        return;
    }

    // 1. Збираємо критерії з UI
    Preference prefs(m_minAgeSpin->value(), m_maxAgeSpin->value(), m_cityEdit->text());
    UserLogger::log(Info, "Starting search with criteria...");

    // 2. Отримуємо ВСІХ користувачів з БД
    QList<UserProfile> allUsers = m_dbManager->getProfilesByCriteria(Preference());

    // 3. Фільтруємо їх за допомогою MatchEngine і зберігаємо у m_currentMatches
    m_currentMatches.clear();
    for (const UserProfile& profile : allUsers) {
        if (MatchEngine::calculateMatch(profile, prefs)) {
            m_currentMatches.append(profile);
        }
    }

    // 4. Відображаємо результат
    UserLogger::log(Info, QString("Search complete! Found %1 matches.").arg(m_currentMatches.count()));

    m_currentMatchIndex = 0;

    // 5. Очищуємо стару стопку віджетів
    while(m_resultsStack->count() > 1) {
        QWidget* widget = m_resultsStack->widget(1);
        m_resultsStack->removeWidget(widget);
        widget->deleteLater();
    }

    // 6. Показуємо перший профіль
    showNextProfile();
}


void SearchPageWidget::showNextProfile() {
    if (m_currentMatchIndex >= m_currentMatches.count()) {
        UserLogger::log(Info, "No more profiles to show.");
        // Показуємо "заглушку" (вона на індексі 0)
        m_resultsStack->setCurrentIndex(0);
        return;
    }

    UserProfile profile = m_currentMatches.at(m_currentMatchIndex);

    ProfileCard* card = new ProfileCard(this);
    card->setProfileData(profile);

    int newIndex = m_resultsStack->addWidget(card);
    m_resultsStack->setCurrentIndex(newIndex);
}

void SearchPageWidget::on_Like_clicked() {
    UserLogger::log(Info, "User clicked LIKE");
    // (Тут буде логіка збереження лайку в БД)

    m_currentMatchIndex++;
    showNextProfile();
}

void SearchPageWidget::on_Skip_clicked() {
    UserLogger::log(Info, "User clicked SKIP");

    m_currentMatchIndex++;
    showNextProfile();
}