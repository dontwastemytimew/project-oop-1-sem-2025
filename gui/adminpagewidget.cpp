#include "adminpagewidget.h"
#include <QSqlTableModel>
#include "UserLogger.h"
#include "FakeDataManager.h"
#include "StatsDialog.h"
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QHeaderView>
#include <QMessageBox>
#include <QEvent>

AdminPageWidget::AdminPageWidget(QWidget *parent)
    : QWidget(parent), m_dbManager(nullptr), m_model(nullptr), m_proxyModel(nullptr)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Верхня панель
    QHBoxLayout* topBar = new QHBoxLayout();

    m_btnBack = new QPushButton(this);
    m_btnBack->setObjectName("adminBackBtn");

    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("adminTitleLabel");
    m_titleLabel->setAlignment(Qt::AlignCenter);

    topBar->addWidget(m_btnBack);
    topBar->addStretch();
    topBar->addWidget(m_titleLabel);
    topBar->addStretch();
    topBar->addSpacing(120);

    mainLayout->addLayout(topBar);

    // Панель інструментів
    QHBoxLayout* toolbarLayout = new QHBoxLayout();

    m_btnGenerate = new QPushButton(this);
    m_btnGenerate->setObjectName("adminGenBtn");
    m_btnStats = new QPushButton(this);
    m_btnStats->setObjectName("adminStatsBtn");
    m_btnDelete = new QPushButton(this);
    m_btnDelete->setObjectName("adminDelBtn");

    m_searchIconLabel = new QLabel(this);
    m_searchIconLabel->setObjectName("searchIconLbl");

    m_searchField = new QLineEdit(this);
    m_searchField->setClearButtonEnabled(true);
    m_searchField->setObjectName("adminSearchInput");

    toolbarLayout->addWidget(m_btnGenerate);
    toolbarLayout->addWidget(m_btnStats);
    toolbarLayout->addWidget(m_btnDelete);
    toolbarLayout->addSpacing(20);
    toolbarLayout->addWidget(m_searchIconLabel);
    toolbarLayout->addWidget(m_searchField);

    mainLayout->addLayout(toolbarLayout);

    // Таблиця
    m_tableView = new QTableView(this);
    m_tableView->setObjectName("adminTable");
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSortingEnabled(true);

    m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_tableView, &QTableView::customContextMenuRequested, this, &AdminPageWidget::onCustomContextMenu);

    mainLayout->addWidget(m_tableView);
    setLayout(mainLayout);

    retranslateUi();

    connect(m_btnDelete, &QPushButton::clicked, this, &AdminPageWidget::onDeleteClicked);
    connect(m_btnGenerate, &QPushButton::clicked, this, &AdminPageWidget::onGenerateClicked);
    connect(m_btnBack, &QPushButton::clicked, this, &AdminPageWidget::backClicked);
    connect(m_btnStats, &QPushButton::clicked, this, &AdminPageWidget::onStatsClicked);

    // ЛОГІКА ПОШУКУ ЧЕРЕЗ PROXY
    connect(m_searchField, &QLineEdit::textChanged, this, [this](const QString &text){
        if (!m_proxyModel) return;
        m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        m_proxyModel->setFilterFixedString(text);
        m_proxyModel->setFilterKeyColumn(-1);
    });
}

void AdminPageWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}

void AdminPageWidget::retranslateUi() {
    m_btnBack->setText(tr(" Назад"));
    m_titleLabel->setText(tr("Панель Адміністратора"));

    m_btnGenerate->setText(tr(" +50 Фейків"));
    m_btnStats->setText(tr(" Статистика"));
    m_btnDelete->setText(tr(" Видалити"));

    m_searchField->setPlaceholderText(tr("Введіть ім'я або місто..."));

    if (m_model) {
        m_model->setHeaderData(0, Qt::Horizontal, "ID");
        m_model->setHeaderData(1, Qt::Horizontal, tr("Ім'я"));
        m_model->setHeaderData(2, Qt::Horizontal, tr("Вік"));
        m_model->setHeaderData(3, Qt::Horizontal, tr("Місто"));
        m_model->setHeaderData(5, Qt::Horizontal, tr("Стать"));
        m_model->setHeaderData(6, Qt::Horizontal, "Email");
        m_model->setHeaderData(7, Qt::Horizontal, tr("Телефон"));
        m_model->setHeaderData(8, Qt::Horizontal, tr("Орієнтація"));
        m_model->setHeaderData(9, Qt::Horizontal, tr("Прихований"));
    }
}

void AdminPageWidget::setDatabaseManager(DatabaseManager* dbManager) {
    m_dbManager = dbManager;
    refreshTable();
}

void AdminPageWidget::refreshTable() {
    if (!m_dbManager) return;

    if (m_proxyModel) {
        delete m_proxyModel;
        m_proxyModel = nullptr;
    }
    if (m_model) {
        delete m_model;
        m_model = nullptr;
    }

    // Створюємо базову SQL модель
    m_model = m_dbManager->getUsersModel(this);

    retranslateUi();

    // Створюємо Проксі-модель
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);

    // Налаштування пошуку
    m_proxyModel->setFilterKeyColumn(-1);

    // Підключаємо Проксі до Таблиці
    m_tableView->setModel(m_proxyModel);

    m_tableView->verticalHeader()->setVisible(false);

    // Приховуємо колонку Bio
    m_tableView->hideColumn(4);

    // Налаштування колонок
    QHeaderView* header = m_tableView->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(3, QHeaderView::Stretch);
    header->setSectionResizeMode(6, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(5, QHeaderView::ResizeToContents);
}

void AdminPageWidget::onDeleteClicked() {
    QModelIndexList selection = m_tableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, tr("Увага"), tr("Виберіть користувача для видалення."));
        return;
    }

    QModelIndex proxyIndex = selection.first();
    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);

    int userId = m_model->data(m_model->index(sourceIndex.row(), 0)).toInt();
    QString userName = m_model->data(m_model->index(sourceIndex.row(), 1)).toString();

    auto reply = QMessageBox::question(this, tr("Видалення"),
        tr("Ви впевнені, що хочете видалити користувача '%1' (ID: %2)?").arg(userName).arg(userId),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (m_dbManager->deleteProfile(userId)) {
            UserLogger::log(Info, "Admin deleted user ID: " + QString::number(userId));
            refreshTable();
        } else {
            QMessageBox::critical(this, tr("Помилка"), tr("Не вдалося видалити користувача."));
        }
    }
}

void AdminPageWidget::onGenerateClicked() {
    if (!m_dbManager) return;

    auto reply = QMessageBox::question(this, tr("Генерація даних"),
        tr("Це створить 50 випадкових профілів у базі даних. Продовжити?"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        FakeDataManager::generateTestUsers(m_dbManager, 50);
        refreshTable();
        QMessageBox::information(this, tr("Успіх"), tr("Успішно згенеровано 50 користувачів!"));
    }
}

void AdminPageWidget::onCustomContextMenu(const QPoint &pos) {
    QModelIndex proxyIndex = m_tableView->indexAt(pos);
    if (!proxyIndex.isValid()) return;

    QMenu contextMenu(this);
    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
    bool isHidden = m_model->data(m_model->index(sourceIndex.row(), 9)).toBool();

    QAction *copyAction = contextMenu.addAction(QIcon(":/resources/icons/copy.png"), tr("Копіювати Email"));
    contextMenu.addSeparator();

    QString banText = isHidden ? tr("Розбанити (Показати)") : tr("Забанити (Приховати)");
    QString banIconPath = isHidden ? ":/resources/icons/unhide.png" : ":/resources/icons/hide.png";
    QAction *banAction = contextMenu.addAction(QIcon(banIconPath), banText);

    contextMenu.addSeparator();
    QAction *deleteAction = contextMenu.addAction(QIcon(":/resources/icons/trash-black.png"), tr("Видалити"));

    QAction *selectedAction = contextMenu.exec(m_tableView->viewport()->mapToGlobal(pos));

    if (selectedAction == copyAction) {
        QString email = m_model->data(m_model->index(sourceIndex.row(), 6)).toString();
        QApplication::clipboard()->setText(email);
        UserLogger::log(Info, "Email copied to clipboard: " + email);
    }
    else if (selectedAction == banAction) {
        toggleHiddenStatus(sourceIndex.row());
    }
    else if (selectedAction == deleteAction) {
        onDeleteClicked();
    }
}
// test push

void AdminPageWidget::toggleHiddenStatus(int row) {
    if (!m_dbManager || !m_model) return;

    int userId = m_model->data(m_model->index(row, 0)).toInt();
    bool currentHidden = m_model->data(m_model->index(row, 9)).toBool();
    bool newHidden = !currentHidden;

    if (m_dbManager->setProfileHidden(userId, newHidden)) {
        UserLogger::log(Info, QString("Admin set user ID %1 hidden status to %2").arg(userId).arg(newHidden));
        refreshTable();
    } else {
        QMessageBox::critical(this, tr("Помилка"), tr("Не вдалося виконати операцію Бану/Розбану."));
    }
}

void AdminPageWidget::onStatsClicked() {
    if (!m_dbManager) return;
    StatsDialog dlg(m_dbManager, this);
    dlg.exec();
}