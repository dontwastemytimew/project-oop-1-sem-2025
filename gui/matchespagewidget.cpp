#include "matchespagewidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QPixmap>
#include <QIcon>
#include <QDebug>

MatchesPageWidget::MatchesPageWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* title = new QLabel(tr("Ваші метчі"), this);
    title->setObjectName("matchesTitleLabel");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    // Список
    m_list = new QListWidget(this);
    m_list->setObjectName("matchesListWidget");
    layout->addWidget(m_list);

    setLayout(layout);

    connect(m_list, &QListWidget::itemClicked,
            this, &MatchesPageWidget::onMatchClicked);
}

MatchesPageWidget::~MatchesPageWidget() {}

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

    QList<int> matchIds = m_db->getMatches(m_currentUserId);
    QList<UserProfile> allProfiles = m_db->getAllProfiles();

    for (int matchId : matchIds)
    {
        for (const UserProfile& p : allProfiles)
        {
            if (p.getId() == matchId)
            {
                QListWidgetItem* item = new QListWidgetItem(m_list);

                item->setText(QString("%1, %2 років\n%3")
                                  .arg(p.getName())
                                  .arg(p.getAge())
                                  .arg(p.getCity()));

                // Фото
                QPixmap pix(p.getPhotoPath());
                if (pix.isNull())
                    pix.load(":/resources/default_avatar.png");

                item->setIcon(QIcon(pix));

                // Зберігаємо userId у елементі
                item->setData(Qt::UserRole, p.getId());
                item->setData(Qt::UserRole + 1, p.getName());

                m_list->addItem(item);
                break;
            }
        }
    }

    if (m_list->count() == 0)
    {
        QListWidgetItem* empty = new QListWidgetItem("Немає метчів поки що 😊");
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

    // Визначаємо ID матчу по тексту 
    QString name = item->text().split(",").first(); // перше слово — ім'я

    // Шукаємо профіль користувача по імені
    UserProfile matchProfile;
    QList<UserProfile> allProfiles = m_db->getAllProfiles();
    for (const UserProfile& p : allProfiles)
    {
        if (p.getName() == name)
        {
            matchProfile = p;
            break;
        }
    }

    // Відкриваємо чат
    ChatWindow* chat = new ChatWindow(matchProfile, m_chatManager, this);
    chat->exec(); // як діалог
}
