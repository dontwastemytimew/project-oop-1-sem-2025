#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QtCharts>
#include "DatabaseManager.h"

class StatsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatsDialog(DatabaseManager* db, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *event) override;

private:
    void setupTabs();
    void applyChartTheme();
    void retranslateUi();
    QWidget* createGenderChart();
    QWidget* createCityChart();
    QWidget* createAgeChart();
    DatabaseManager* m_db;
    QVBoxLayout* m_mainLayout;
    QTabWidget* m_tabWidget;
    QChart* m_genderChart;
    QChart* m_cityChart;
    QChart* m_ageChart;
    QBarSet* m_ageBarSet;
};

#endif // STATSDIALOG_H