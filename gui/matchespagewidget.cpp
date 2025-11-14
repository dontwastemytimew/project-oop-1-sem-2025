#include "matchespagewidget.h"
#include <QLabel>
#include <QVBoxLayout>

MatchesPageWidget::MatchesPageWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout();

    QLabel* label = new QLabel(tr("Сторінка МЕТЧІВ (Matches)"), this);
    mainLayout->addWidget(label);

    setLayout(mainLayout);

    setStyleSheet("background-color: #E1F5FE;"); // Світло-блакитний
}

MatchesPageWidget::~MatchesPageWidget() {}