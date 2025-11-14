#include "profilecard.h"
#include <QVBoxLayout>
#include <QPixmap>
#include <QLabel>

ProfileCard::ProfileCard(QWidget* parent)
    : QWidget(parent)
{
    lblPhoto = new QLabel(this);
    lblName = new QLabel(this);
    lblAge = new QLabel(this);
    lblCity = new QLabel(this);
    lblDescription = new QLabel(this);

    lblPhoto->setPixmap(QPixmap(":/resources/example_photo.jpg").scaled(150, 150, Qt::KeepAspectRatio));
    lblPhoto->setAlignment(Qt::AlignCenter);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(lblPhoto);
    layout->addWidget(lblName);
    layout->addWidget(lblAge);
    layout->addWidget(lblCity);
    layout->addWidget(lblDescription);
    setLayout(layout);
}

void ProfileCard::setProfileData(const UserProfile& profile)
{
    lblName->setText(tr("Ім'я: %1").arg(profile.getName()));
    lblAge->setText(tr("Вік: %1").arg(profile.getAge()));
    lblCity->setText(tr("Місто: %1").arg(profile.getCity()));
    lblDescription->setText(tr("Про себе: %1").arg(profile.getBio()));
    // (Тут можна додати логіку для фото)
}
