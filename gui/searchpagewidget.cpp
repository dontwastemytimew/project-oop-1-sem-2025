#include "searchpagewidget.h"
#include "MatchEngine.h"
#include "Preference.h"
#include "UserLogger.h"
#include "profilecard.h"
#include <QComboBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QMessageBox>
#include <QVector>
#include <QCompleter>
#include <QStringList>
#include <algorithm>
#include <QEvent>

SearchPageWidget::SearchPageWidget(QWidget *parent)
: QWidget(parent)
{
    // Створюємо UI для Фільтрів
    QFormLayout* filterLayout = new QFormLayout();

    m_minAgeSpin = new QSpinBox(this);
    m_maxAgeSpin = new QSpinBox(this);
    m_minAgeSpin->setRange(18, 99);
    m_maxAgeSpin->setRange(18, 99);
    m_maxAgeSpin->setValue(99);

    m_genderCombo = new QComboBox(this);
    m_genderCombo->addItem("", "any");
    m_genderCombo->addItem("", "male");
    m_genderCombo->addItem("", "female");

    m_orientationCombo = new QComboBox(this);
    m_orientationCombo->addItem("", "any");
    m_orientationCombo->addItem("", "hetero");
    m_orientationCombo->addItem("", "bi");
    m_orientationCombo->addItem("", "gay");

    m_cityEdit = new QLineEdit(this);
    m_findButton = new QPushButton(this);

    m_labelMinAge = new QLabel(this);
    m_labelMaxAge = new QLabel(this);
    m_labelGender = new QLabel(this);
    m_labelOrientation = new QLabel(this);
    m_labelCity = new QLabel(this);

    filterLayout->addRow(m_labelMinAge, m_minAgeSpin);
    filterLayout->addRow(m_labelMaxAge, m_maxAgeSpin);
    filterLayout->addRow(m_labelGender, m_genderCombo);
    filterLayout->addRow(m_labelOrientation, m_orientationCombo);
    filterLayout->addRow(m_labelCity, m_cityEdit);

    m_resultsStack = new QStackedWidget(this);
    m_resultsStack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_placeholderLabel = new QLabel(this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_resultsStack->addWidget(m_placeholderLabel);

    m_likeButton = new QPushButton(this);
    m_likeButton->setObjectName("m_likeButton");

    m_skipButton = new QPushButton(this);
    m_skipButton->setObjectName("m_skipButton");

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_skipButton, 1);
    buttonLayout->addWidget(m_likeButton, 1);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(filterLayout);
    mainLayout->addWidget(m_findButton);

    QHBoxLayout* cardCenterLayout = new QHBoxLayout();
    cardCenterLayout->addStretch();
    cardCenterLayout->addWidget(m_resultsStack);
    cardCenterLayout->addStretch();
    mainLayout->addLayout(cardCenterLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    retranslateUi();

    connect(m_findButton, &QPushButton::clicked, this, &SearchPageWidget::on_btn_Find_clicked);
    connect(m_likeButton, &QPushButton::clicked, this, &SearchPageWidget::on_Like_clicked);
    connect(m_skipButton, &QPushButton::clicked, this, &SearchPageWidget::on_Skip_clicked);
}

SearchPageWidget::~SearchPageWidget() {
}


void SearchPageWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}

void SearchPageWidget::retranslateUi() {
    m_labelMinAge->setText(tr("Мін. вік:"));
    m_labelMaxAge->setText(tr("Макс. вік:"));
    m_labelGender->setText(tr("Стать:"));
    m_labelOrientation->setText(tr("Орієнтація:"));
    m_labelCity->setText(tr("Місто:"));

    m_genderCombo->setItemText(0, tr("Не важливо"));
    m_genderCombo->setItemText(1, tr("Чоловік"));
    m_genderCombo->setItemText(2, tr("Жінка"));

    m_orientationCombo->setItemText(0, tr("Не важливо"));
    m_orientationCombo->setItemText(1, tr("Гетеро"));
    m_orientationCombo->setItemText(2, tr("Бісексуал"));
    m_orientationCombo->setItemText(3, tr("Гей/Лесбі"));

    m_cityEdit->setPlaceholderText(tr("Введіть місто..."));
    m_findButton->setText(tr("Знайти пару"));
    m_placeholderLabel->setText(tr("Натисніть 'Знайти пару', щоб побачити профілі"));
    m_likeButton->setText(tr(" Like"));
    m_skipButton->setText(tr(" Skip"));
}

void SearchPageWidget::setDatabaseManager(DatabaseManager* dbManager) {
    m_dbManager = dbManager;

    m_matchEngine = new MatchEngine(m_dbManager);

    setupCityAutocomplete();
}

void SearchPageWidget::showMatchPopup(const UserProfile& target)
{
    QMessageBox msg;
    msg.setIcon(QMessageBox::Information);
    msg.setWindowTitle(tr("У вас метч!"));
    msg.setText(
        tr("Ви та %1 вподобали один одного!\n"
           "Тепер можете знайти цього користувача у МЕТЧАХ.").arg(target.getName())
        );
    msg.exec();
}

// ЛОГІКА ПОШУКУ
void SearchPageWidget::on_btn_Find_clicked() {
    if (!m_dbManager || m_currentUser.getId() == -1) {
        QMessageBox::critical(this, tr("Профіль не завантажено"), tr("Профіль не завантажено. Створіть профіль."));
        return;
    }

    Preference prefs(m_minAgeSpin->value(), m_maxAgeSpin->value(), m_cityEdit->text(),
                      m_genderCombo->currentText(), m_orientationCombo->currentText());

    QList<UserProfile> dbResults = m_dbManager->getProfilesByCriteria(prefs, m_currentUser.getId());

    QList<QPair<UserProfile, int>> ratedMatches;

    for (UserProfile profile : dbResults) {

        QList<QString> tags = m_dbManager->getTagsForUser(profile.getId());
        profile.setTags(tags);

        if (m_matchEngine->isCompatible(m_currentUser, profile)) {
            int compatibility = m_matchEngine->compatibilityPercent(m_currentUser, profile);
            ratedMatches.append({profile, compatibility});
        }
    }

    std::sort(ratedMatches.begin(), ratedMatches.end(),
              [](const QPair<UserProfile, int>& a, const QPair<UserProfile, int>& b){
                  return a.second > b.second;
              });

    m_currentMatches.clear();
    for (const auto& pair : ratedMatches) {
        m_currentMatches.append(pair.first);
    }

    UserLogger::log(Info, QString("Search complete! Found %1 matches, sorted by score.").arg(m_currentMatches.count()));

    m_currentMatchIndex = 0;

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

    int compatibility = m_matchEngine->compatibilityPercent(m_currentUser, profile);
    card->setCompatibilityPercent(compatibility);

    int newIndex = m_resultsStack->addWidget(card);
    m_resultsStack->setCurrentIndex(newIndex);
}

void SearchPageWidget::on_Like_clicked() {
    if (m_currentMatches.isEmpty() || m_currentMatchIndex >= m_currentMatches.count()) {
        return;
    }

    UserProfile targetProfile = m_currentMatches.at(m_currentMatchIndex);
    int currentUserId = m_currentUser.getId();
    int targetId = targetProfile.getId();

    m_dbManager->addLike(currentUserId, targetId);

    if (m_dbManager->hasUserLiked(targetId, currentUserId)) {

        if (m_dbManager->addMatch(currentUserId, targetId)) {
            QMessageBox::information(this, tr("Ура! Метч!"),
                                     tr("Ви сподобалися одне одному! Тепер ви можете спілкуватися."));
            emit matchFound(currentUserId, targetId);
        } else {
            UserLogger::log(Error, "Failed to record mutual match in DB.");
        }
    }

    m_currentMatchIndex++;
    showNextProfile();
}

void SearchPageWidget::on_Skip_clicked() {
    if (m_resultsStack->currentIndex() == 0) return;

    UserLogger::log(Info, "User clicked SKIP");

    if (m_currentMatches.size() > m_currentMatchIndex) {
        m_currentMatches.removeAt(m_currentMatchIndex);
    }

    showNextProfile();
}

void SearchPageWidget::setCurrentUser(const UserProfile& profile) {
    m_currentUser = profile;
}

void SearchPageWidget::setupCityAutocomplete() {
    if (!m_dbManager) return;
    QStringList cities = m_dbManager->getAllCities();
    if (cities.isEmpty()) return;
    QCompleter* completer = new QCompleter(cities, this);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_cityEdit->setCompleter(completer);
}
