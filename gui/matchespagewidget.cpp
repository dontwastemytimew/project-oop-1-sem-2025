#include "matchespagewidget.h"
#include "DatabaseManager.h" // Потрібен
#include "chatwindow.h"      // <-- ДОДАНО: Для відкриття вікна чату
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPixmap>
#include <QIcon>
#include <QDebug>
#include <QApplication> // Для Q_UNUSED, якщо не використовується

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
    // Якщо всі віджети створюються з parent=this (як у вашому випадку),
    // вони видаляться автоматично. Тут нічого не потрібно.
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

    // 1. Отримуємо ID тих, з ким є взаємний лайк (оптимізований SQL)
    QList<int> matchIds = m_db->getMutualMatchIds(m_currentUserId);

    // 2. Отримуємо повні профілі за списком ID (оптимізований SQL)
    QList<UserProfile> matchProfiles = m_db->getProfilesByIds(matchIds);

    for (const UserProfile& p : matchProfiles)
    {
        QListWidgetItem* item = new QListWidgetItem(m_list);

        item->setText(QString("%1, %2 %3\n%4")
                          .arg(p.getName())
                          .arg(p.getAge())
                          .arg(tr("років"))
                          .arg(p.getCity()));

        // Фото (якщо є)
        QPixmap pix(p.getPhotoPath());
        if (pix.isNull())
            pix.load(":/resources/default_avatar.png");

        item->setIcon(QIcon(pix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

        // Зберігаємо ID у елементі (КЛЮЧОВА ЗМІННА ДЛЯ onMatchClicked)
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

// ОПТИМІЗОВАНА ФУНКЦІЯ КЛІКУ ДЛЯ ВІДКРИТТЯ ЧАТУ
void MatchesPageWidget::onMatchClicked(QListWidgetItem* item)
{
    if (!m_db || !m_chatManager) return;

    // 1. ЗЧИТУЄМО ID ІЗ ЗБЕРЕЖЕНИХ ДАНИХ (UserRole)
    int matchId = item->data(Qt::UserRole).toInt();

    if (matchId <= 0) return;

    // 2. Завантажуємо повний профіль для чату, використовуючи loadProfileById
    UserProfile matchProfile;

    // МЕТОД LOADPROFILEBYID БУВ ДОДАНИЙ В DatabaseManager
    if (m_db->loadProfileById(matchId, matchProfile))
    {
        // 3. Відкриваємо чат (завдання №17)
        // ChatWindow* chat = new ChatWindow(matchProfile, m_chatManager, this);
        // chat->exec();
        qDebug() << "Opening chat with:" << matchProfile.getName();
        // ТУТ ПОТРІБНО ДОДАТИ РЕАЛІЗАЦІЮ ЧАТУ

    } else {
        qWarning() << "Could not load profile for match ID:" << matchId;
    }
}