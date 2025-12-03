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
#include <QLineEdit>
#include <QEvent>

MatchesPageWidget::MatchesPageWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("matchesTitleLabel");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_titleLabel);

    m_searchField = new QLineEdit(this);
    layout->addWidget(m_searchField);

    m_list = new QListWidget(this);
    m_list->setObjectName("matchesListWidget");
    layout->addWidget(m_list);

    setLayout(layout);

    retranslateUi();

    connect(m_list, &QListWidget::itemClicked,
            this, &MatchesPageWidget::onMatchClicked);

    connect(m_searchField, &QLineEdit::textChanged,
            this, &MatchesPageWidget::onSearchTextChanged);
}

MatchesPageWidget::~MatchesPageWidget() {
}

void MatchesPageWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}

void MatchesPageWidget::retranslateUi() {
    m_titleLabel->setText(tr("Мої Метчі та Чати"));
    m_searchField->setPlaceholderText(tr("Введіть ім'я метча..."));
    reloadMatches();
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

    QList<int> matchIds = m_db->getMutualMatchIds(m_currentUserId);
    QList<UserProfile> matchProfiles = m_db->getProfilesByIds(matchIds);
    QList<QPair<UserProfile, QDateTime>> sortableMatches;

    for (const UserProfile& p : matchProfiles)
    {
        QDateTime lastTime = m_db->getChatLastTimestamp(m_currentUserId, p.getId());
        sortableMatches.append({p, lastTime});
    }

    std::sort(sortableMatches.begin(), sortableMatches.end(),
              [](const QPair<UserProfile, QDateTime>& a, const QPair<UserProfile, QDateTime>& b){
                  if (!a.second.isValid() && b.second.isValid()) return false;
                  if (a.second.isValid() && !b.second.isValid()) return true;
                  return a.second > b.second;
              });

    for (const auto& pair : sortableMatches)
    {
        const UserProfile& p = pair.first;
        QListWidgetItem* item = new QListWidgetItem(m_list);

        item->setText(QString("%1, %2 %3\n%4")
                          .arg(p.getName())
                          .arg(p.getAge())
                          .arg(tr("років"))
                          .arg(p.getCity()));

        QPixmap pix(p.getPhotoPath());
        if (pix.isNull())
            pix.load(":/resources/icons/default_avatar.png");

        item->setIcon(QIcon(pix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
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

void MatchesPageWidget::onMatchClicked(QListWidgetItem *item) {
    int targetId = item->data(Qt::UserRole).toInt();
    emit openChatRequested(targetId);
}

void MatchesPageWidget::onSearchTextChanged(const QString &text) {
    for(int i = 0; i < m_list->count(); ++i) {
        QListWidgetItem* item = m_list->item(i);
        bool matches = item->text().contains(text, Qt::CaseInsensitive);
        item->setHidden(!matches);
    }
}