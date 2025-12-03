#include "profilepagewidget.h"
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QFileDialog>
#include <QBuffer>
#include <QCompleter>
#include <QEvent>
#include <QMessageBox>
#include "UserLogger.h"

ProfilePageWidget::ProfilePageWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel* header = new QLabel("struct <span style='color:#9932CC'>UserProfile</span>", this);

    QFont headerFont("Consolas", 22, QFont::Bold);
    headerFont.setStyleHint(QFont::Monospace);
    header->setFont(headerFont);
    header->setTextFormat(Qt::RichText);
    header->setMargin(10);
    mainLayout->addWidget(header);

    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(30);

    QVBoxLayout* photoLayout = new QVBoxLayout();

    m_labelPhotoTitle = new QLabel(this);
    m_labelPhotoTitle->setAlignment(Qt::AlignCenter);
    photoLayout->addWidget(m_labelPhotoTitle);

    m_photoLabel = new QLabel(this);
    m_photoLabel->setFixedSize(220, 280);
    m_photoLabel->setAlignment(Qt::AlignCenter);
    m_photoLabel->setStyleSheet("border: 2px dashed #888888; border-radius: 8px;");
    m_photoLabel->setObjectName("photoPlaceholder");
    m_photoLabel->setText(tr("Немає фото"));
    photoLayout->addWidget(m_photoLabel);

    m_choosePhotoButton = new QPushButton(this);
    m_choosePhotoButton->setCursor(Qt::PointingHandCursor);
    m_choosePhotoButton->setObjectName("btnChoosePhoto");
    photoLayout->addWidget(m_choosePhotoButton);

    photoLayout->addStretch();
    contentLayout->addLayout(photoLayout);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    formLayout->setHorizontalSpacing(20);
    formLayout->setVerticalSpacing(15);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText(tr("Ваше ім'я"));

    m_ageSpinBox = new QSpinBox(this);
    m_ageSpinBox->setRange(18, 99);
    m_ageSpinBox->setFixedWidth(100);

    m_cityEdit = new QLineEdit(this);
    m_cityEdit->setPlaceholderText(tr("Київ, Львів..."));

    m_phoneEdit = new QLineEdit(this);
    m_phoneEdit->setPlaceholderText("+380...");

    m_emailEdit = new QLineEdit(this);
    m_emailEdit->setPlaceholderText("example@mail.com");

    m_genderCombo = new QComboBox(this);
    m_genderCombo->addItem("Не вказано");
    m_genderCombo->addItem("Чоловік");
    m_genderCombo->addItem("Жінка");

    m_orientationCombo = new QComboBox(this);
    m_orientationCombo->addItem("Не вказано");
    m_orientationCombo->addItem("Гетеро");
    m_orientationCombo->addItem("Гей/Лесбі");
    m_orientationCombo->addItem("Бісексуал");

    m_tagsEdit = new QLineEdit(this);
    m_tagsEdit->setPlaceholderText(tr("IT, Спорт, Кава..."));

    m_labelName = new QLabel(this);
    m_labelAge = new QLabel(this);
    m_labelCity = new QLabel(this);
    m_labelGender = new QLabel(this);
    m_labelOrientation = new QLabel(this);
    m_labelEmail = new QLabel(this);
    m_labelPhone = new QLabel(this);
    m_labelTags = new QLabel(this);

    formLayout->addRow(m_labelName, m_nameEdit);
    formLayout->addRow(m_labelAge, m_ageSpinBox);
    formLayout->addRow(m_labelCity, m_cityEdit);
    formLayout->addRow(m_labelGender, m_genderCombo);
    formLayout->addRow(m_labelOrientation, m_orientationCombo);

    formLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    formLayout->addRow(m_labelEmail, m_emailEdit);
    formLayout->addRow(m_labelPhone, m_phoneEdit);
    formLayout->addRow(m_labelTags, m_tagsEdit);

    contentLayout->addLayout(formLayout);
    mainLayout->addLayout(contentLayout);

    m_labelBio = new QLabel(this);
    QFont bioFont = m_labelBio->font();
    bioFont.setBold(true);
    m_labelBio->setFont(bioFont);
    m_labelBio->setMargin(5);
    mainLayout->addWidget(m_labelBio);

    m_bioEdit = new QTextEdit(this);
    m_bioEdit->setPlaceholderText(tr("Розкажіть трохи про себе..."));
    m_bioEdit->setMaximumHeight(100);
    mainLayout->addWidget(m_bioEdit);

    m_saveButton = new QPushButton(this);
    m_saveButton->setCursor(Qt::PointingHandCursor);
    m_saveButton->setMinimumHeight(40);
    m_saveButton->setObjectName("btnSaveProfile");

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(m_saveButton);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    retranslateUi();

    connect(m_choosePhotoButton, &QPushButton::clicked, this, &ProfilePageWidget::onChoosePhoto);
    connect(m_saveButton, &QPushButton::clicked,
            this, &ProfilePageWidget::on_btn_SaveProfile_clicked);
}

void ProfilePageWidget::retranslateUi() {
    m_labelName->setText(tr("Ім'я:"));
    m_labelAge->setText(tr("Вік:"));
    m_labelCity->setText(tr("Місто:"));
    m_labelGender->setText(tr("Стать:"));
    m_labelOrientation->setText(tr("Орієнтація:"));
    m_labelEmail->setText(tr("Email:"));
    m_labelPhone->setText(tr("Телефон:"));
    m_labelTags->setText(tr("Інтереси (Теги):"));
    m_labelBio->setText(tr("Про себе:"));
    m_labelPhotoTitle->setText(tr("Фото профілю"));

    m_choosePhotoButton->setText(tr("Обрати фото..."));
    m_saveButton->setText(tr("Зберегти профіль"));
}

void ProfilePageWidget::setDatabaseManager(DatabaseManager* dbManager) {
    m_dbManager = dbManager;
    setupCityAutocomplete();
}

void ProfilePageWidget::loadCurrentProfile() {
    if (m_currentUser.getId() == -1 && m_dbManager) {
        m_dbManager->getCurrentUserProfile(m_currentUser);
    }

    if (!m_currentUser.getPhotoPath().isEmpty()) {
        m_photoPath = m_currentUser.getPhotoPath();
        QPixmap pix(m_photoPath);
        if (!pix.isNull()) {
            m_photoLabel->setPixmap(pix.scaled(220, 280, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        } else {
            m_photoLabel->setText(tr("Помилка фото"));
        }
    } else {
        m_photoLabel->setText(tr("Немає фото"));
    }

    m_nameEdit->setText(m_currentUser.getName());
    m_ageSpinBox->setValue(m_currentUser.getAge());
    m_cityEdit->setText(m_currentUser.getCity());
    m_bioEdit->setText(m_currentUser.getBio());
    m_phoneEdit->setText(m_currentUser.getContactInfo().getPhone());
    m_emailEdit->setText(m_currentUser.getContactInfo().getEmail());
    m_genderCombo->setCurrentText(m_currentUser.getGender());
    m_orientationCombo->setCurrentText(m_currentUser.getOrientation());

    if (m_dbManager && m_currentUser.getId() != -1) {
        QList<QString> tags = m_dbManager->getTagsForUser(m_currentUser.getId());

        m_currentUser.setTags(tags);

        m_tagsEdit->setText(tags.join(", "));
    } else {
        m_tagsEdit->setText("");
    }
}

void ProfilePageWidget::setInternalProfile(const UserProfile& profile) {
    m_currentUser = profile;
    loadCurrentProfile();
}

void ProfilePageWidget::clearFields() {
    m_nameEdit->clear();
    m_ageSpinBox->setValue(18);
    m_cityEdit->clear();
    m_bioEdit->clear();
    m_phoneEdit->clear();
    m_emailEdit->clear();
    m_tagsEdit->clear();
    m_photoLabel->clear();
    m_photoLabel->setText(tr("Немає фото"));
    m_photoPath.clear();
    m_currentUser = UserProfile();
}

void ProfilePageWidget::onChoosePhoto() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Оберіть файл"), "", tr("Images (*.png *.jpg *.jpeg)"));
    if (!fileName.isEmpty()) {
        m_photoPath = fileName;
        QPixmap pix(m_photoPath);
        m_photoLabel->setPixmap(pix.scaled(220, 280, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }
}

void ProfilePageWidget::setupCityAutocomplete() {
    if (!m_dbManager) return;
    QStringList cities = m_dbManager->getAllCities();
    QCompleter* completer = new QCompleter(cities, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_cityEdit->setCompleter(completer);
}

void ProfilePageWidget::on_btn_SaveProfile_clicked() {
    QString name = m_nameEdit->text();
    QString email = m_emailEdit->text();
    int age = m_ageSpinBox->value();

    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Помилка валідації"), tr("Ім'я не може бути пустим."));
        UserLogger::log(Warning, "Validation error: Name is empty.");
        return;
    }

    if (age < 18) {
        QMessageBox::critical(this, tr("Помилка валідації"), tr("Вік має бути не менше 18 років."));
        UserLogger::log(Critical, "Validation error: Age is below 18.");
        return;
    }

    if (!email.contains('@') || !email.contains('.')) {
        QMessageBox::warning(this, tr("Помилка валідації"), tr("Введіть коректну адресу електронної пошти."));
        UserLogger::log(Warning, "Validation error: Invalid email format.");
        return;
    }

    m_currentUser.setName(name);
    m_currentUser.setAge(age);
    m_currentUser.setCity(m_cityEdit->text());
    m_currentUser.setBio(m_bioEdit->toPlainText());
    m_currentUser.setGender(m_genderCombo->currentText());
    m_currentUser.setOrientation(m_orientationCombo->currentText());
    m_currentUser.setPhotoPath(m_photoPath);

    ContactInfo info(m_phoneEdit->text(), email);
    m_currentUser.setContactInfo(info);

    int id = -1;
    bool success = false;

    if (m_currentUser.getId() == -1) {
        id = m_dbManager->saveProfile(m_currentUser);
        if (id != -1) {
            m_currentUser.setId(id);
            success = true;
        }
    } else {
        success = m_dbManager->updateProfile(m_currentUser);
        id = m_currentUser.getId();
    }

    if (id != -1 && success) {
        QString tagsString = m_tagsEdit->text();
        QStringList tags = tagsString.split(",", Qt::SkipEmptyParts);
        for (QString& tag : tags) tag = tag.trimmed();

        m_currentUser.setTags(tags);

        m_dbManager->removeAllTags(id);
        for (const QString& tag : tags) {
            m_dbManager->addTag(id, tag);
        }

        m_dbManager->saveCurrentUserId(id);

        QMessageBox::information(this, tr("Успіх"), tr("Профіль успішно збережено!"));
        UserLogger::log(Info, "Profile saved successfully. ID=" + QString::number(id));
        emit profileSaved(m_currentUser);
    } else {
        QMessageBox::critical(this, tr("Помилка БД"), tr("Не вдалося зберегти профіль у базу даних."));
        UserLogger::log(Error, "Failed to save profile.");
    }
}

void ProfilePageWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}