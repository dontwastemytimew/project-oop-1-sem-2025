#include "matchespagewidget.h"
#include "ui_matchespagewidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QPixmap>

MatchesPageWidget::MatchesPageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MatchesPageWidget)
{
    ui->setupUi(this);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* title = new QLabel("💞 Ваші метчі", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; font-weight: bold; margin: 10px;");
    layout->addWidget(title);

    // Список
    m_list = new QListWidget(this);
    m_list->setIconSize(QSize(64, 64));
    m_list->setStyleSheet("font-size: 16px;");
    layout->addWidget(m_list);

    setLayout(layout);
}

MatchesPageWidget::~MatchesPageWidget()
{
    delete ui;
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

    // отримуємо ID матчів
    QList<int> matchIds = m_db->getMatches(m_currentUserId);

    // для кожного ID — повний профіль
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

    reloadMatches();   //  автоматично оновити список
}
