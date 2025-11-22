#include "searchpagewidget.h"
#include "MatchEngine.h"
#include "Preference.h"
#include "UserLogger.h"
#include "profilecard.h"
#include <QFormLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QComboBox>
#include <QMessageBox>

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

    // Налаштування Статі
    m_genderCombo = new QComboBox(this);
    m_genderCombo->addItem(tr("Не важливо"));
    m_genderCombo->addItem(tr("Чоловік"));
    m_genderCombo->addItem(tr("Жінка"));

    // Налаштування Орієнтації
    m_orientationCombo = new QComboBox(this);
    m_orientationCombo->addItem(tr("Не важливо"));
    m_orientationCombo->addItem(tr("Гетеро"));
    m_orientationCombo->addItem(tr("Бісексуал"));
    m_orientationCombo->addItem(tr("Гей/Лесбі"));

    m_cityEdit = new QLineEdit(this);
    m_cityEdit->setPlaceholderText(tr("Введіть місто..."));

    m_findButton = new QPushButton(tr("Знайти пару"), this);

    filterLayout->addRow(tr("Мін. вік:"), m_minAgeSpin);
    filterLayout->addRow(tr("Макс. вік:"), m_maxAgeSpin);
    filterLayout->addRow(tr("Стать:"), m_genderCombo);
    filterLayout->addRow(tr("Орієнтація:"), m_orientationCombo);
    filterLayout->addRow(tr("Місто:"), m_cityEdit);

    m_resultsStack = new QStackedWidget(this);
    m_resultsStack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Додаємо стартову заглушку
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
    mainLayout->addLayout(filterLayout);
    mainLayout->addWidget(m_findButton);
    mainLayout->addWidget(m_resultsStack);
    mainLayout->addLayout(buttonLayout);

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

    int minAge = m_minAgeSpin->value();
    int maxAge = m_maxAgeSpin->value();
    QString city = m_cityEdit->text();

    QString gender = m_genderCombo->currentText();
    QString orientation = m_orientationCombo->currentText();

    // Створюємо Preference з усіма параметрами
    Preference prefs(minAge, maxAge, city, gender, orientation);

    UserLogger::log(Info, "Starting search with criteria: " + city + ", " + gender);

    // Отримуємо ВІДФІЛЬТРОВАНИХ користувачів прямо з БД
    m_currentMatches = m_dbManager->getProfilesByCriteria(prefs);

    UserLogger::log(Info, QString("Search complete! Found %1 matches.").arg(m_currentMatches.count()));

    m_currentMatchIndex = 0;

    // Очищуємо стару стопку віджетів
    while(m_resultsStack->count() > 1) {
        QWidget* widget = m_resultsStack->widget(1);
        m_resultsStack->removeWidget(widget);
        widget->deleteLater();
    }

    showNextProfile();
}

void SearchPageWidget::showNextProfile() {
    if (m_currentMatchIndex >= m_currentMatches.count()) {
        UserLogger::log(Info, "No more profiles to show.");
        m_resultsStack->setCurrentIndex(0);

        if (m_currentMatches.isEmpty()) {
             QMessageBox::information(this, tr("Пошук"), tr("На жаль, нікого не знайдено за цими критеріями."));
        }
        return;
    }

    UserProfile profile = m_currentMatches.at(m_currentMatchIndex);

    ProfileCard* card = new ProfileCard(this);
    card->setProfileData(profile);

    int newIndex = m_resultsStack->addWidget(card);
    m_resultsStack->setCurrentIndex(newIndex);
}

void SearchPageWidget::on_Like_clicked() {
    if (m_resultsStack->currentIndex() == 0) return;

    UserLogger::log(Info, "User clicked LIKE");

    // ТУТ У МАЙБУТНЬОМУ БУДЕ:
    // m_dbManager->addLike(currentUser.id, targetUser.id);

    m_currentMatchIndex++;
    showNextProfile();
}

void SearchPageWidget::on_Skip_clicked() {
    if (m_resultsStack->currentIndex() == 0) return;

    UserLogger::log(Info, "User clicked SKIP");

    m_currentMatchIndex++;
    showNextProfile();
}