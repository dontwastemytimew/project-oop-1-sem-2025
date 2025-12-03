#ifndef ADMINPAGEWIDGET_H
#define ADMINPAGEWIDGET_H

#include "DatabaseManager.h"

class QTableView;
class QSqlTableModel;
class QPushButton;
class QLineEdit;
class QSortFilterProxyModel;
class StatsDialog;
class QLabel;

class AdminPageWidget : public QWidget {
    Q_OBJECT
public:
    explicit AdminPageWidget(QWidget *parent = nullptr);

    void setDatabaseManager(DatabaseManager* dbManager);
    void refreshTable();

protected:
    void changeEvent(QEvent *event) override;

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
    void retranslateUi();
    QLabel* m_titleLabel;
    QLabel* m_searchIconLabel;
};

#endif // ADMINPAGEWIDGET_H