#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QtCharts>
#include "DatabaseManager.h"

class StatsDialog : public QDialog {
    Q_OBJECT
public:
    explicit StatsDialog(DatabaseManager* db, QWidget *parent = nullptr);

private:
    void setupTabs();
    QWidget* createGenderChart();
    QWidget* createCityChart();
    QWidget* createAgeChart();
    void applyChartTheme();
    DatabaseManager* m_db;
    QVBoxLayout* m_mainLayout;
};

#endif // STATSDIALOG_H