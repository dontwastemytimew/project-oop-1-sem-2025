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

    // ★ ВІДСОТОК СУМІСНОСТІ
    lblCompatibility = new QLabel(this);
    lblCompatibility->setObjectName("cardCompatibility");
    lblCompatibility->setAlignment(Qt::AlignLeft);
    lblCompatibility->setStyleSheet(
        "font-size: 16px; "
        "font-weight: bold; "
        "color: #e6399b;"   // рожевий/сердечний
        );

    // БІО
    lblDescription = new QLabel(this);
    lblDescription->setObjectName("cardBio");
    lblDescription->setWordWrap(true);
    lblDescription->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Додавання в layout
    cardLayout->addWidget(lblPhoto, 0, Qt::AlignCenter);
    cardLayout->addLayout(nameAgeLayout);
    cardLayout->addWidget(lblCity);

    cardLayout->addWidget(lblCompatibility);   // ★ новий елемент

    cardLayout->addWidget(lblDescription);
    cardLayout->addStretch();

    mainLayout->addWidget(cardFrame);
    setLayout(mainLayout);
}

void ProfileCard::setProfileData(const UserProfile& profile)
{
    // ★ ФOТО ПРОФІЛЮ
    QString photoPath = profile.getPhotoPath();
    QPixmap pixmap;

    if (!photoPath.isEmpty() && QFile::exists(photoPath)) {
        pixmap.load(photoPath);
    }

    if (pixmap.isNull()) {
        pixmap.load(":/resources/default_avatar.png");
    }

    if (pixmap.isNull()) {
        lblPhoto->setText("No Photo");
    } else {
        lblPhoto->setPixmap(
            pixmap.scaled(
                lblPhoto->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                )
            );
    }

    // ★ ІМ’Я + ВІК
    lblName->setText(profile.getName());
    lblAge->setText(QString(", %1").arg(profile.getAge()));

    // ★ МІСТО
    lblCity->setText(QString("📍 %1").arg(profile.getCity()));

    // ★ БІО
    lblDescription->setText(profile.getBio());
}

//
// ★ НОВИЙ МЕТОД — встановлення % сумісності
//
void ProfileCard::setCompatibilityPercent(int percent)
{
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    lblCompatibility->setText(
        QString("❤️ Ви підходите на %1%").arg(percent)
        );
}
