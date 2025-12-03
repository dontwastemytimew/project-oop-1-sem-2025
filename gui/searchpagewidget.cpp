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
#include <algorithm>
#include <QEvent>

SearchPageWidget::SearchPageWidget(QWidget *parent)
: QWidget(parent)
{
    m_matchEngine = nullptr;
    m_dbManager = nullptr;
    m_currentMatchIndex = 0;

    // --- ГОЛОВНИЙ LAYOUT ---
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(40);

    // =========================================================
    // ЛІВА ПАНЕЛЬ: ФІЛЬТРИ
    // =========================================================
    QWidget* filterContainer = new QWidget(this);
    filterContainer->setFixedWidth(320);
    filterContainer->setObjectName("filterContainer"); // Для стилізації фону через QSS

    QVBoxLayout* filterLayout = new QVBoxLayout(filterContainer);
    filterLayout->setContentsMargins(10, 10, 10, 10);
    filterLayout->setSpacing(10);

    // 1. Заголовок "class Search {" (Стилізований заголовок)
    QLabel* headerLabel = new QLabel("class <span style='color:#9932CC'>Search</span> {", this);
    QFont headerFont = headerLabel->font();
    headerFont.setFamily("Consolas");
    headerFont.setPointSize(16);
    headerFont.setBold(true);
    headerLabel->setFont(headerFont);
    headerLabel->setTextFormat(Qt::RichText);
    headerLabel->setAlignment(Qt::AlignLeft);
    filterLayout->addWidget(headerLabel);

    // 2. "public:"
    QLabel* publicLabel = new QLabel("public:", this);
    QFont publicFont = publicLabel->font();
    publicFont.setFamily("Consolas");
    publicFont.setPointSize(14);
    publicFont.setBold(true);
    publicLabel->setFont(publicFont);
    // Фарбуємо в фіолетовий через stylesheet для надійності, або залишаємо як є
    publicLabel->setStyleSheet("color: #9932CC;");
    filterLayout->addWidget(publicLabel);

    // 3. Контейнер для форми (з відступом)
    QWidget* formContainer = new QWidget(this);
    QVBoxLayout* formIndentLayout = new QVBoxLayout(formContainer);
    formIndentLayout->setContentsMargins(25, 0, 0, 0); // Відступ зліва (Tab)
    formIndentLayout->setSpacing(15);

    // Форма параметрів
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setVerticalSpacing(15);
    formLayout->setHorizontalSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Ініціалізація віджетів
    m_minAgeSpin = new QSpinBox(this);
    m_minAgeSpin->setRange(18, 99);

    m_maxAgeSpin = new QSpinBox(this);
    m_maxAgeSpin->setRange(18, 99);
    m_maxAgeSpin->setValue(99);

    m_genderCombo = new QComboBox(this);
    m_genderCombo->addItem("Не важливо");
    m_genderCombo->addItem("Чоловік");
    m_genderCombo->addItem("Жінка");

    m_orientationCombo = new QComboBox(this);
    m_orientationCombo->addItem("Не важливо");
    m_orientationCombo->addItem("Гетеро");
    m_orientationCombo->addItem("Гей/Лесбі");
    m_orientationCombo->addItem("Бісексуал");

    m_cityEdit = new QLineEdit(this);
    // Плейсхолдер встановлюється в retranslateUi

    m_labelMinAge = new QLabel(this);
    m_labelMaxAge = new QLabel(this);
    m_labelGender = new QLabel(this);
    m_labelOrientation = new QLabel(this);
    m_labelCity = new QLabel(this);

    // Шрифт для лейблів (звичайний, не код)
    QFont labelFont("Segoe UI");
    labelFont.setPointSize(10);
    m_labelMinAge->setFont(labelFont);
    m_labelMaxAge->setFont(labelFont);
    m_labelGender->setFont(labelFont);
    m_labelOrientation->setFont(labelFont);
    m_labelCity->setFont(labelFont);

    formLayout->addRow(m_labelMinAge, m_minAgeSpin);
    formLayout->addRow(m_labelMaxAge, m_maxAgeSpin);
    formLayout->addRow(m_labelGender, m_genderCombo);
    formLayout->addRow(m_labelOrientation, m_orientationCombo);
    formLayout->addRow(m_labelCity, m_cityEdit);

    formIndentLayout->addLayout(formLayout);
    formIndentLayout->addSpacing(10);

    // Кнопка ПОШУКУ
    m_findButton = new QPushButton(this);
    m_findButton->setObjectName("btnFind");
    m_findButton->setCursor(Qt::PointingHandCursor);
    m_findButton->setMinimumHeight(45);

    formIndentLayout->addWidget(m_findButton);

    filterLayout->addWidget(formContainer);
    filterLayout->addStretch();

    // 4. Футер "};"
    QLabel* footerLabel = new QLabel("};", this);
    footerLabel->setFont(headerFont);
    filterLayout->addWidget(footerLabel);

    mainLayout->addWidget(filterContainer);


    // =========================================================
    // ПРАВА ПАНЕЛЬ: РЕЗУЛЬТАТИ
    // =========================================================
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(20);
    rightLayout->setAlignment(Qt::AlignCenter);

    m_resultsStack = new QStackedWidget(this);
    m_resultsStack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_placeholderLabel = new QLabel(this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setWordWrap(true);

    QFont phFont("Segoe UI", 12);
    phFont.setItalic(true);
    m_placeholderLabel->setFont(phFont);

    m_resultsStack->addWidget(m_placeholderLabel);

    rightLayout->addWidget(m_resultsStack);

    // Кнопки дій
    QHBoxLayout* actionLayout = new QHBoxLayout();
    actionLayout->setSpacing(60);
    actionLayout->setAlignment(Qt::AlignCenter);

    m_skipButton = new QPushButton(this);
    m_skipButton->setObjectName("m_skipButton");
    m_skipButton->setFixedSize(140, 60);
    m_skipButton->setCursor(Qt::PointingHandCursor);

    m_likeButton = new QPushButton(this);
    m_likeButton->setObjectName("m_likeButton");
    m_likeButton->setFixedSize(140, 60);
    m_likeButton->setCursor(Qt::PointingHandCursor);

    actionLayout->addWidget(m_skipButton);
    actionLayout->addWidget(m_likeButton);

    rightLayout->addLayout(actionLayout);

    mainLayout->addLayout(rightLayout, 1);

    setLayout(mainLayout);

    retranslateUi();

    connect(m_findButton, &QPushButton::clicked, this, &SearchPageWidget::on_btn_Find_clicked);
    connect(m_likeButton, &QPushButton::clicked, this, &SearchPageWidget::on_Like_clicked);
    connect(m_skipButton, &QPushButton::clicked, this, &SearchPageWidget::on_Skip_clicked);
}

SearchPageWidget::~SearchPageWidget() {
    delete m_matchEngine;
}

void SearchPageWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}

void SearchPageWidget::retranslateUi() {
    // Українські назви
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

    // Українські кнопки
    m_findButton->setText(tr("Знайти пару"));

    m_placeholderLabel->setText(tr("Натисніть 'Знайти пару', щоб побачити профілі\n(Або змініть критерії пошуку)"));

    m_likeButton->setText(tr("Лайк"));
    m_skipButton->setText(tr("Пропустити"));
}

void SearchPageWidget::setDatabaseManager(DatabaseManager* dbManager) {
    m_dbManager = dbManager;
    if(m_dbManager) {
        m_matchEngine = new MatchEngine(m_dbManager);
        setupCityAutocomplete();
    }
}

void SearchPageWidget::showMatchPopup(const UserProfile& target)
{
    QMessageBox msg;
    msg.setIcon(QMessageBox::Information);
    msg.setWindowTitle(tr("It's a Match!"));
    msg.setText(
        tr("Ви та <b>%1</b> вподобали один одного!<br>"
           "Тепер ви можете спілкуватися в чаті.").arg(target.getName())
        );
    msg.setStyleSheet("QLabel{color: white;} QMessageBox{background-color: #2D2D30;} QPushButton{background-color: #9932CC; color: white; font-weight: bold;}");
    msg.exec();
}

void SearchPageWidget::on_btn_Find_clicked() {
    if (!m_dbManager || m_currentUser.getId() == -1) {
        QMessageBox::critical(this, tr("Помилка"), tr("Профіль користувача не завантажено."));
        return;
    }

    QString genderData = m_genderCombo->currentData().toString();
    QString genderDB;

    if (genderData == "male") genderDB = "Чоловік";
    else if (genderData == "female") genderDB = "Жінка";
    else genderDB = "";

    QString orientData = m_orientationCombo->currentData().toString();
    QString orientDB;

    if (orientData == "hetero") orientDB = "Гетеро";
    else if (orientData == "bi") orientDB = "Бісексуал";
    else if (orientData == "gay") orientDB = "Гей/Лесбі";
    else orientDB = "";

    Preference prefs(m_minAgeSpin->value(), m_maxAgeSpin->value(), m_cityEdit->text(),
                      genderDB, orientDB);

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

    UserLogger::log(Info, QString("Search complete! Found %1 matches.").arg(m_currentMatches.count()));

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
             m_placeholderLabel->setText(tr("Нікого не знайдено за вашими критеріями.\nСпробуйте змінити параметри."));
        } else {
             m_placeholderLabel->setText(tr("Це всі профілі на сьогодні.\nЗайдіть пізніше!"));
        }

        m_likeButton->setEnabled(false);
        m_skipButton->setEnabled(false);
        return;
    }

    m_likeButton->setEnabled(true);
    m_skipButton->setEnabled(true);

    UserProfile profile = m_currentMatches.at(m_currentMatchIndex);

    ProfileCard* card = new ProfileCard(this);
    card->setProfileData(profile);

    if (m_matchEngine) {
        int compatibility = m_matchEngine->compatibilityPercent(m_currentUser, profile);
        card->setCompatibilityPercent(compatibility);
    }

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
            showMatchPopup(targetProfile);
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
        // Логіка скіпа
    }
    m_currentMatchIndex++;
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