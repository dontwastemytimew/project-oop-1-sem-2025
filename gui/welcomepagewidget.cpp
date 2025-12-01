#include "welcomepagewidget.h"
#include <QVBoxLayout>
#include <QLabel>

WelcomePageWidget::WelcomePageWidget(QWidget *parent) : QWidget(parent)
{


    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 50, 50, 50);

    QLabel* titleLabel = new QLabel(tr("Вітаємо у Match++!"), this);
    titleLabel->setObjectName("welcomeTitle");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel* subtitleLabel = new QLabel(tr("Оберіть 'Профіль' для налаштування або 'Пошук', щоб почати"), this);
    subtitleLabel->setObjectName("welcomeSubtitle");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setWordWrap(true);

    mainLayout->addStretch();
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addStretch();

    setLayout(mainLayout);
}