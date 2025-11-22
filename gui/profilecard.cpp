#include "profilecard.h"
#include <QHBoxLayout>
#include <QPixmap>
#include <QLabel>
#include <QFrame>

ProfileCard::ProfileCard(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QFrame* cardFrame = new QFrame(this);
    cardFrame->setObjectName("cardFrame");
    cardFrame->setFixedWidth(340);

    QVBoxLayout* cardLayout = new QVBoxLayout(cardFrame);
    cardLayout->setSpacing(5);
    cardLayout->setContentsMargins(15, 20, 15, 20);

    // ФОТО
    lblPhoto = new QLabel(this);
    lblPhoto->setObjectName("cardPhoto");
    lblPhoto->setAlignment(Qt::AlignCenter);
    lblPhoto->setFixedSize(250, 250);
    lblPhoto->setStyleSheet("background-color: #cccccc; border-radius: 10px;");

    // ІМ'Я та ВІК
    QHBoxLayout* nameAgeLayout = new QHBoxLayout();

    lblName = new QLabel(this);
    lblName->setObjectName("cardName");

    lblAge = new QLabel(this);
    lblAge->setObjectName("cardAge");

    nameAgeLayout->addWidget(lblName);
    nameAgeLayout->addWidget(lblAge);
    nameAgeLayout->addStretch();

    // МІСТО
    lblCity = new QLabel(this);
    lblCity->setObjectName("cardCity");

    // ОПИС
    lblDescription = new QLabel(this);
    lblDescription->setObjectName("cardBio");
    lblDescription->setWordWrap(true);
    lblDescription->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    cardLayout->addWidget(lblPhoto, 0, Qt::AlignCenter);
    cardLayout->addLayout(nameAgeLayout);
    cardLayout->addWidget(lblCity);
    cardLayout->addWidget(lblDescription);
    cardLayout->addStretch();

    mainLayout->addWidget(cardFrame);
    setLayout(mainLayout);
}

void ProfileCard::setProfileData(const UserProfile& profile)
{
    // ФОТО
    // Тут треба логіку: якщо у юзера є фото -> показати. Якщо ні -> заглушка.
    // Поки ставимо заглушку з іконкою
    QPixmap pixmap(":/resources/example_photo.jpg");
    if (!pixmap.isNull()) {
        lblPhoto->setPixmap(pixmap.scaled(lblPhoto->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        lblPhoto->setText("No Photo");
    }

    // ТЕКСТ
    lblName->setText(profile.getName());

    lblAge->setText(QString(", %1").arg(profile.getAge()));

    // Додаємо іконку локації
    lblCity->setText(QString("📍 %1").arg(profile.getCity()));

    lblDescription->setText(profile.getBio());

    // Якщо є стать/орієнтація, можна додати і їх
    // lblDescription->setText(QString("%1\n\n%2, %3").arg(profile.getBio()).arg(profile.getGender()).arg(profile.getOrientation()));
}