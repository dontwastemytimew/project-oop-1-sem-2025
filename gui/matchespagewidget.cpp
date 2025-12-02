#include "matchespagewidget.h"
#include "DatabaseManager.h"
#include "ChatPageWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPixmap>
#include <QIcon>
#include <QDebug>
#include <QApplication>

MatchesPageWidget::MatchesPageWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* title = new QLabel(tr("Ваші метчі"), this);
    title->setObjectName("matchesTitleLabel");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    m_list = new QListWidget(this);
    m_list->setObjectName("matchesListWidget");
    layout->addWidget(m_list);

    setLayout(layout);

    connect(m_list, &QListWidget::itemClicked,
            this, &MatchesPageWidget::onMatchClicked);
}

MatchesPageWidget::~MatchesPageWidget() {
}

void MatchesPageWidget::setDatabaseManager(DatabaseManager* db)
{
    m_db = db;
}

void MatchesPageWidget::setCurrentUserId(int id)
{
    m_currentUserId = id;
    reloadMatches();
}

void MatchesPageWidget::reloadMatches()
{
    if (!m_db || m_currentUserId <= 0)
        return;

    m_list->clear();

    // 1. Отримуємо ID тих, з ким є взаємний лайк
    QList<int> matchIds = m_db->getMutualMatchIds(m_currentUserId);

    // 2. Отримуємо повні профілі за списком ID
    QList<UserProfile> matchProfiles = m_db->getProfilesByIds(matchIds);

    for (const UserProfile& p : matchProfiles)
    {
        QListWidgetItem* item = new QListWidgetItem(m_list);

        item->setText(QString("%1, %2 %3\n%4")
                          .arg(p.getName())
                          .arg(p.getAge())
                          .arg(tr("років"))
                          .arg(p.getCity()));

        // Фото
        QPixmap pix(p.getPhotoPath());
        if (pix.isNull())
            pix.load(":/resources/default_avatar.png");

        item->setIcon(QIcon(pix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

        // Зберігаємо ID у елементі
        item->setData(Qt::UserRole, p.getId());

        m_list->addItem(item);
    }

    if (m_list->count() == 0)
    {
        QListWidgetItem* empty = new QListWidgetItem(tr("Немає метчів поки що"));
        empty->setFlags(Qt::NoItemFlags);
        m_list->addItem(empty);
    }
}

void MatchesPageWidget::onMatchCreated(int userId, int targetId)
{
    Q_UNUSED(userId)
    Q_UNUSED(targetId)

    reloadMatches();
}

void MatchesPageWidget::setChatManager(ChatManager* chatManager)
{
    m_chatManager = chatManager;
}

void MatchesPageWidget::onMatchClicked(QListWidgetItem* item)
{
    if (!m_db || !m_chatManager) return;

    int matchId = item->data(Qt::UserRole).toInt();

    if (matchId <= 0) return;

    qDebug() << "Requesting chat page for match ID:" << matchId;

    emit openChatRequested(matchId);
}