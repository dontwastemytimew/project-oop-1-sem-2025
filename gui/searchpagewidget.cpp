#include "searchpagewidget.h"
#include "ui_searchpagewidget.h"
#include <QMessageBox>

SearchPageWidget::SearchPageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SearchPageWidget)
{
    ui->setupUi(this);

    // кнопки
    connect(ui->btnLike, &QPushButton::clicked,
            this, &SearchPageWidget::on_Like_clicked);

    connect(ui->btnSkip, &QPushButton::clicked,
            this, &SearchPageWidget::on_Skip_clicked);
}

SearchPageWidget::~SearchPageWidget()
{
    delete ui;
}

void SearchPageWidget::setDatabaseManager(DatabaseManager* dbManager)
{
    m_dbManager = dbManager;

    // отримуємо залогіненого користувача
    m_dbManager->getCurrentUserProfile(m_currentUser);

    loadProfiles();
}

void SearchPageWidget::loadProfiles()
{
    if (!m_dbManager)
        return;

    m_profiles = m_dbManager->getAllProfiles();

    // видаляємо себе зі списку
    for (int i = 0; i < m_profiles.size(); ++i)
    {
        if (m_profiles[i].getId() == m_currentUser.getId()) {
            m_profiles.removeAt(i);
            break;
        }
    }

    m_index = 0;
    showCurrentProfile();
}

void SearchPageWidget::showCurrentProfile()
{
    if (m_profiles.isEmpty()) {
        ui->cardContainer->setCurrentWidget(ui->emptyPage);
        return;
    }

    if (m_index >= m_profiles.size())
        m_index = 0;

    // очищаємо стару картку
    QLayoutItem* item;
    while ((item = ui->cardLayout->takeAt(0)) != nullptr) {
        if (QWidget* w = item->widget())
            w->deleteLater();
        delete item;
    }

    UserProfile profile = m_profiles[m_index];

    ProfileCard* card = new ProfileCard(this);
    card->setProfileData(profile);

    ui->cardLayout->addWidget(card);
}

void SearchPageWidget::on_Skip_clicked()
{
    m_index++;
    if (m_index >= m_profiles.size())
        m_index = 0;

    showCurrentProfile();
}

void SearchPageWidget::on_Like_clicked()
{
    if (!m_dbManager || m_profiles.isEmpty())
        return;

    UserProfile target = m_profiles[m_index];
    int userId = m_currentUser.getId();
    int targetId = target.getId();

    // --- 1. Додаємо лайк ---
    m_dbManager->addLike(userId, targetId);

    // --- 2. Перевіряємо взаємний лайк (МЕТЧ) ---
    if (m_dbManager->isMutualLike(userId, targetId)) {

        // показуємо popup
        showMatchPopup(target);

        // повідомляємо MatchesPage (потрібно у 4.4)
        emit matchFound(userId, targetId);
    }

    // --- 3. Перехід до наступного профілю ---
    m_index++;
    if (m_index >= m_profiles.size())
        m_index = 0;

    showCurrentProfile();
}

void SearchPageWidget::showMatchPopup(const UserProfile& target)
{
    QMessageBox msg;
    msg.setIcon(QMessageBox::Information);
    msg.setWindowTitle("🎉 У вас метч!");
    msg.setText(
        QString("Ви та %1 вподобали один одного!\n"
                "Тепер можете знайти цього користувача у МЕТЧАХ.")
            .arg(target.getName())
        );
    msg.exec();
}
