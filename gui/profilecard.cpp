#include "profilecard.h"
#include <QHBoxLayout>
#include <QPixmap>
#include <QLabel>
#include <QFrame>
#include <QFile>

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

    // ВІДСОТОК СУМІСНОСТІ
    lblCompatibility = new QLabel(this);
    lblCompatibility->setObjectName("cardCompatibility");

    // БІО
    lblDescription = new QLabel(this);
    lblDescription->setObjectName("cardBio");
    lblDescription->setWordWrap(true);
    lblDescription->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    cardLayout->addWidget(lblPhoto, 0, Qt::AlignCenter);
    cardLayout->addLayout(nameAgeLayout);
    cardLayout->addWidget(lblCompatibility);
    cardLayout->addWidget(lblCity);
    cardLayout->addWidget(lblDescription);
    cardLayout->addStretch();

    mainLayout->addWidget(cardFrame);
    setLayout(mainLayout);
}

void ProfileCard::setProfileData(const UserProfile& profile)
{
    // ФOТО ПРОФІЛЮ
    QString photoPath = profile.getPhotoPath();
    QPixmap pixmap;

    if (!photoPath.isEmpty() && QFile::exists(photoPath)) {
        pixmap.load(photoPath);
    }

    if (pixmap.isNull()) {
        pixmap.load(":/resources/icons/default_avatar.png");
    }

    if (pixmap.isNull()) {
        lblPhoto->setText(tr("Немає фото"));
    } else {
        lblPhoto->setPixmap(
            pixmap.scaled(
                lblPhoto->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                )
            );
    }

    // ТЕКСТ
    lblName->setText(profile.getName());
    lblAge->setText(QString(", %1").arg(profile.getAge()));

    lblCity->setText(QString("📍 %1").arg(profile.getCity()));

    lblDescription->setText(profile.getBio());
}

void ProfileCard::setCompatibilityPercent(int percent)
{
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    lblCompatibility->setText(
        tr("Сумісність: %1%").arg(percent)
        );
}