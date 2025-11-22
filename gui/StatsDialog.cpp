#include "StatsDialog.h"
#include <QApplication>

StatsDialog::StatsDialog(DatabaseManager* db, QWidget *parent)
    : QDialog(parent), m_db(db)
{
    setWindowTitle(tr("Аналітика"));
    resize(900, 600);
    setObjectName("statsDialog");

    m_mainLayout = new QVBoxLayout(this);
    setupTabs();
    setLayout(m_mainLayout);

    applyChartTheme();
}

void StatsDialog::setupTabs() {
    QTabWidget* tabWidget = new QTabWidget(this);
    tabWidget->setObjectName("statsTabs");

    tabWidget->addTab(createGenderChart(), tr("Стать"));
    tabWidget->addTab(createCityChart(), tr("Міста"));
    tabWidget->addTab(createAgeChart(), tr("Вік"));

    m_mainLayout->addWidget(tabWidget);
}

void StatsDialog::applyChartTheme() {
    QString currentStyle = qApp->styleSheet();
    bool isDark = currentStyle.contains("#1e1e1e", Qt::CaseInsensitive);

    QColor textColor = isDark ? Qt::white : Qt::black;
    QColor gridColor = isDark ? QColor("#444444") : QColor("#d0d0d0");

    QList<QChartView*> views = this->findChildren<QChartView*>();

    for (QChartView* view : views) {
        QChart* chart = view->chart();
        if (!chart) continue;

        // Тема
        if (isDark) {
            chart->setTheme(QChart::ChartThemeBlueCerulean);
        } else {
            chart->setTheme(QChart::ChartThemeBlueNcs);
        }

        // Фон прозорий
        chart->setBackgroundBrush(Qt::NoBrush);

        // Заголовки
        chart->setTitleBrush(textColor);
        chart->legend()->setLabelColor(textColor);

        // Фарбуємо підписи всередині графіків
        QList<QAbstractSeries*> allSeries = chart->series();
        for (QAbstractSeries* series : allSeries) {

            if (series->type() == QAbstractSeries::SeriesTypePie) {
                QPieSeries* pieSeries = static_cast<QPieSeries*>(series);
                for (QPieSlice* slice : pieSeries->slices()) {
                    slice->setLabelColor(textColor);
                    slice->setBorderColor(isDark ? QColor("#2D2D2D") : Qt::white);
                }
            }

             if (series->type() == QAbstractSeries::SeriesTypeBar) {
                QBarSeries* barSeries = static_cast<QBarSeries*>(series);
                for (QBarSet* set : barSeries->barSets()) {
                    set->setLabelColor(textColor);
                }
            }
        }

        QList<QAbstractAxis*> axes = chart->axes();
        for (QAbstractAxis* axis : axes) {
            axis->setLabelsColor(textColor);
            axis->setTitleBrush(textColor);

            QPen gridPen = axis->gridLinePen();
            gridPen.setColor(gridColor);
            axis->setGridLinePen(gridPen);

            QPen linePen = axis->linePen();
            linePen.setColor(textColor);
            axis->setLinePen(linePen);
        }

        chart->update();
    }
}

QWidget* StatsDialog::createGenderChart() {
    QMap<QString, int> data = m_db->getGenderStatistics();

    QPieSeries *series = new QPieSeries();
    for (auto it = data.begin(); it != data.end(); ++it) {
        QPieSlice* slice = series->append(it.key(), it.value());
        QString label = QString("%1: %2").arg(it.key()).arg(it.value());
        slice->setLabel(label);

        if (it.key() == "Чоловік") slice->setBrush(QColor("#2196F3"));
        else if (it.key() == "Жінка") slice->setBrush(QColor("#E91E63"));
        else slice->setBrush(Qt::gray);

        slice->setLabelVisible();
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(tr("Розподіл користувачів за статтю"));

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    return chartView;
}

QWidget* StatsDialog::createCityChart() {
    QMap<QString, int> data = m_db->getCityStatistics();
    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.4);

    for (auto it = data.begin(); it != data.end(); ++it) {
        QPieSlice* slice = series->append(it.key(), it.value());
        slice->setLabel(QString("%1 (%2)").arg(it.key()).arg(it.value()));
        slice->setLabelVisible();
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(tr("ТОП-5 Міст"));
    chart->legend()->setAlignment(Qt::AlignRight);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    return chartView;
}

QWidget* StatsDialog::createAgeChart() {
    QMap<QString, int> data = m_db->getAgeStatistics();

    QBarSet *set = new QBarSet(tr("Користувачі"));

    QStringList categories;
    for (auto it = data.begin(); it != data.end(); ++it) {
        *set << it.value();
        categories << it.key();
    }

    QBarSeries *series = new QBarSeries();
    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(tr("Вікові групи"));
    chart->legend()->setVisible(false);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%i");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    return chartView;
}