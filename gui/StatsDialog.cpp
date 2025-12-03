#include "StatsDialog.h"
#include <QApplication>
#include <QEvent>

StatsDialog::StatsDialog(DatabaseManager* db, QWidget *parent)
    : QDialog(parent), m_db(db),
      m_tabWidget(nullptr),
      m_genderChart(nullptr),
      m_cityChart(nullptr),
      m_ageChart(nullptr),
      m_ageBarSet(nullptr)
{
    resize(900, 600);
    setObjectName("statsDialog");

    m_mainLayout = new QVBoxLayout(this);
    setupTabs();
    setLayout(m_mainLayout);

    applyChartTheme();

    retranslateUi();
}

void StatsDialog::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QDialog::changeEvent(event);
}

void StatsDialog::retranslateUi() {
    setWindowTitle(tr("Аналітика"));

    if (m_tabWidget) {
        m_tabWidget->setTabText(0, tr("Стать"));
        m_tabWidget->setTabText(1, tr("Міста"));
        m_tabWidget->setTabText(2, tr("Вік"));
    }

    if (m_genderChart) m_genderChart->setTitle(tr("Розподіл користувачів за статтю"));
    if (m_cityChart) m_cityChart->setTitle(tr("ТОП-5 Міст"));
    if (m_ageChart) m_ageChart->setTitle(tr("Вікові групи"));

    if (m_ageBarSet) m_ageBarSet->setLabel(tr("Користувачі"));
}

void StatsDialog::setupTabs() {
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setObjectName("statsTabs");

    m_tabWidget->addTab(createGenderChart(), "");
    m_tabWidget->addTab(createCityChart(), "");
    m_tabWidget->addTab(createAgeChart(), "");

    m_mainLayout->addWidget(m_tabWidget);
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

        if (isDark) {
            chart->setTheme(QChart::ChartThemeBlueCerulean);
        } else {
            chart->setTheme(QChart::ChartThemeBlueNcs);
        }

        chart->setBackgroundBrush(Qt::NoBrush);
        chart->setTitleBrush(textColor);
        chart->legend()->setLabelColor(textColor);

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

    m_genderChart = new QChart();
    m_genderChart->addSeries(series);

    QChartView *chartView = new QChartView(m_genderChart);
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

    m_cityChart = new QChart();
    m_cityChart->addSeries(series);
    m_cityChart->legend()->setAlignment(Qt::AlignRight);

    QChartView *chartView = new QChartView(m_cityChart);
    chartView->setRenderHint(QPainter::Antialiasing);
    return chartView;
}

QWidget* StatsDialog::createAgeChart() {
    QMap<QString, int> data = m_db->getAgeStatistics();

    m_ageBarSet = new QBarSet("");

    QStringList categories;
    for (auto it = data.begin(); it != data.end(); ++it) {
        *m_ageBarSet << it.value();
        categories << it.key();
    }

    QBarSeries *series = new QBarSeries();
    series->append(m_ageBarSet);

    m_ageChart = new QChart();
    m_ageChart->addSeries(series);
    m_ageChart->legend()->setVisible(false);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    m_ageChart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%i");
    m_ageChart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(m_ageChart);
    chartView->setRenderHint(QPainter::Antialiasing);
    return chartView;
}