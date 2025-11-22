#ifndef ADMINPAGEWIDGET_H
#define ADMINPAGEWIDGET_H

#include <QWidget>
#include "DatabaseManager.h"

class QTableView;
class QSqlTableModel;
class QPushButton;
class QLineEdit;
class QSortFilterProxyModel;
class StatsDialog;

class AdminPageWidget : public QWidget {
    Q_OBJECT
public:
    explicit AdminPageWidget(QWidget *parent = nullptr);

    void setDatabaseManager(DatabaseManager* dbManager);
    void refreshTable();

    signals:
        void backClicked();

    private slots:
        void onDeleteClicked();
    void onGenerateClicked();
    void onCustomContextMenu(const QPoint &pos);
    void onStatsClicked();

private:
    DatabaseManager* m_dbManager;
    QTableView* m_tableView;
    QSqlTableModel* m_model;
    QSortFilterProxyModel* m_proxyModel;
    void toggleHiddenStatus(int row);
    QPushButton* m_btnDelete;
    QPushButton* m_btnGenerate;
    QPushButton* m_btnBack;
    QLineEdit* m_searchField;
    QPushButton* m_btnStats;
};

#endif // ADMINPAGEWIDGET_H