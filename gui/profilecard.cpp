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
    cardFrame->setFixedWidth(550);

    QHBoxLayout* dualColumnLayout = new QHBoxLayout(cardFrame);
    dualColumnLayout->setContentsMargins(20, 20, 20, 20);
    dualColumnLayout->setSpacing(20);

    lblPhoto = new QLabel(this);
    lblPhoto->setObjectName("cardPhoto");
    lblPhoto->setAlignment(Qt::AlignCenter);
    lblPhoto->setFixedSize(200, 200);

    QVBoxLayout* textStackLayout = new QVBoxLayout();
    textStackLayout->setSpacing(5);
    textStackLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    lblName = new QLabel(this); lblName->setObjectName("cardName");
    lblAge = new QLabel(this); lblAge->setObjectName("cardAge");
    lblCity = new QLabel(this); lblCity->setObjectName("cardCity");
    lblCompatibility = new QLabel(this); lblCompatibility->setObjectName("cardCompatibility");
    lblDescription = new QLabel(this); lblDescription->setObjectName("cardBio");
    lblDescription->setWordWrap(true);
    lblTags = new QLabel(this); lblTags->setObjectName("tagsLabel");
    lblTags->setWordWrap(true);
    lblTags->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QHBoxLayout* nameAgeLayout = new QHBoxLayout();
    nameAgeLayout->addWidget(lblName);
    nameAgeLayout->addWidget(lblAge);
    nameAgeLayout->addStretch();

    textStackLayout->addLayout(nameAgeLayout);
    textStackLayout->addWidget(lblCompatibility);
    textStackLayout->addWidget(lblCity);
    textStackLayout->addWidget(lblTags);
    textStackLayout->addWidget(lblDescription);
    textStackLayout->addStretch(1);

    dualColumnLayout->addWidget(lblPhoto);
    dualColumnLayout->addLayout(textStackLayout);

    mainLayout->addWidget(cardFrame);
    setLayout(mainLayout);
}

void ProfileCard::setProfileData(const UserProfile& profile)
{
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

    lblName->setText(profile.getName());
    lblAge->setText(QString(", %1").arg(profile.getAge()));
    lblCity->setText(QString("📍 %1").arg(profile.getCity()));
    lblDescription->setText(profile.getBio());

    QList<QString> tags = profile.getTags();
    QString tagsText;

    if (!tags.isEmpty()) {
        tagsText = "#" + tags.join(" #");
        lblTags->setVisible(true);
    } else {
        lblTags->setVisible(false);
    }

    lblTags->setText(tagsText);
}

void ProfileCard::setCompatibilityPercent(int percent)
{
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    lblCompatibility->setText(
        tr("Сумісність: %1%").arg(percent)
        );
}