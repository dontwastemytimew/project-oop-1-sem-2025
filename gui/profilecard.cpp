#include "profilecard.h"
#include <QHBoxLayout>
#include <QPixmap>

ProfileCard::ProfileCard(QWidget* parent) : QWidget(parent)
{
    this->setObjectName("ProfileCard");

    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setSpacing(10);
    rootLayout->setContentsMargins(20, 20, 20, 20);

    QLabel* header = new QLabel("class <span style='color:#E5C07B'>Candidate</span> {", this);
    header->setObjectName("profileHeader");
    header->setTextFormat(Qt::RichText);
    rootLayout->addWidget(header);

    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(25);
    contentLayout->setContentsMargins(20, 0, 0, 0);

    lblPhoto = new QLabel(this);
    lblPhoto->setObjectName("profilePhoto");
    lblPhoto->setAlignment(Qt::AlignCenter);
    lblPhoto->setFixedSize(280, 350);
    contentLayout->addWidget(lblPhoto);

    QWidget* infoContainer = new QWidget(this);
    infoContainer->setObjectName("profileInfoContainer");

    QVBoxLayout* infoLayout = new QVBoxLayout(infoContainer);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(12);
    infoLayout->setAlignment(Qt::AlignVCenter);

    QLabel* publicLabel = new QLabel("public:", this);
    publicLabel->setObjectName("profilePublicKeyword");
    infoLayout->addWidget(publicLabel);

    lblName = new QLabel(this);
    lblName->setObjectName("profileField");
    lblName->setWordWrap(true);
    lblName->setTextFormat(Qt::RichText);
    lblName->setIndent(20);
    infoLayout->addWidget(lblName);

    lblCity = new QLabel(this);
    lblCity->setObjectName("profileField");
    lblCity->setTextFormat(Qt::RichText);
    lblCity->setIndent(20);
    infoLayout->addWidget(lblCity);

    lblTags = new QLabel(this);
    lblTags->setObjectName("profileField");
    lblTags->setWordWrap(true);
    lblTags->setTextFormat(Qt::RichText);
    lblTags->setIndent(20);
    infoLayout->addWidget(lblTags);

    infoLayout->addSpacing(10);

    lblDescription = new QLabel(this);
    lblDescription->setObjectName("profileComment");
    lblDescription->setWordWrap(true);
    lblDescription->setIndent(20);
    infoLayout->addWidget(lblDescription);

    infoLayout->addSpacing(15);

    lblCompatibility = new QLabel(this);
    lblCompatibility->setObjectName("profileCompatibility");
    lblCompatibility->setTextFormat(Qt::RichText);
    lblCompatibility->setIndent(20);
    infoLayout->addWidget(lblCompatibility);

    contentLayout->addWidget(infoContainer, 1);
    rootLayout->addLayout(contentLayout);

    QLabel* footer = new QLabel("};", this);
    footer->setObjectName("profileFooter");
    rootLayout->addWidget(footer);
}

void ProfileCard::setProfileData(const UserProfile& profile)
{
    if (!profile.getPhotoPath().isEmpty()) {
        QPixmap pixmap(profile.getPhotoPath());
        if (!pixmap.isNull()) {
            lblPhoto->setPixmap(pixmap.scaled(280, 350, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
            m_hasPhoto = true;
        } else {
            lblPhoto->setText("<code style='color:#E06C75'>// Error: Image 404</code>");
        }
    } else {
        lblPhoto->setText("<code style='color:#5C6370'>// No Image Provided</code>");
    }

    QString typeClr   = "#C678DD";
    QString varClr    = "#E06C75";
    QString strClr    = "#98C379";
    QString numClr    = "#D19A66";

    QString nameHtml = QString(
        "<span style='color:%1'>User</span> "
        "<span style='color:%2'>candidate</span>("
        "<span style='color:%3'>\"%4\"</span>, "
        "<span style='color:%5'>%6</span>);"
    ).arg(typeClr, varClr, strClr, profile.getName(), numClr, QString::number(profile.getAge()));

    lblName->setText(nameHtml);

    QString cityHtml = QString(
        "<span style='color:%1'>String</span> location = "
        "<span style='color:%2'>\"%3\"</span>;"
    ).arg(typeClr, strClr, profile.getCity());

    lblCity->setText(cityHtml);

    QStringList tags = profile.getTags();
    QString tagsString;
    if (tags.isEmpty()) {
        tagsString = "{}";
    } else {
        tagsString = "{ ";
        for (int i = 0; i < tags.size(); ++i) {
            tagsString += QString("\"%1\"").arg(tags[i]);
            if (i < tags.size() - 1) tagsString += ", ";
        }
        tagsString += " }";
    }

    QString tagsHtml = QString(
        "<span style='color:%1'>List</span>&lt;<span style='color:%1'>String</span>&gt; interests = <span style='color:%2'>%3</span>;"
    ).arg(typeClr, strClr, tagsString);

    lblTags->setText(tagsHtml);

    if (!profile.getBio().isEmpty()) {
        lblDescription->setText(QString("/*\n * %1\n */").arg(profile.getBio()));
    } else {
        lblDescription->setText("// TODO: Add description");
    }
}

void ProfileCard::setCompatibilityPercent(int percent)
{
    m_currentPercent = percent;

    QString valColor = (percent > 70) ? "#98C379" : (percent > 40) ? "#D19A66" : "#E06C75";
    QString typeColor = "#61AFEF";
    QString varColor = "#E06C75";

    QString html = QString(
        "<span style='color:%1'>const int</span> "
        "<span style='color:%2'>match_score</span> = "
        "<span style='color:%3'>%4</span>; "
        "<span style='color:#5C6370'>// %5</span>"
    ).arg(typeColor, varColor, valColor, QString::number(percent),
          (percent > 50 ? "High compatibility" : "Low compatibility"));

    lblCompatibility->setText(html);
}

void ProfileCard::changeEvent(QEvent *event) { QWidget::changeEvent(event); }
void ProfileCard::retranslateUi() {}