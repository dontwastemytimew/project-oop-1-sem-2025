#include "profilepagewidget.h"
#include "UserLogger.h"
#include "UserProfile.h"
#include "ContactInfo.h"
#include <QComboBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>
#include <QPixmap>
#include <QDir>
#include <QCompleter>
#include <QCoreApplication>
#include <QFileInfo>
#include <QFrame>
#include <QStringList>
#include <QRegularExpression>
#include <QEvent>

ProfilePageWidget::ProfilePageWidget(QWidget *parent)
    : QWidget(parent), m_dbManager(nullptr)
{
    QFormLayout* formLayout = new QFormLayout();

    //СТВОРЕННЯ ФОТО-СЕКЦІЇ
    m_photoLabel = new QLabel(this);
    m_photoLabel->setFixedSize(120, 120);
    m_photoLabel->setFrameShape(QFrame::Box);
    m_photoLabel->setAlignment(Qt::AlignCenter);
    m_photoLabel->setPixmap(QPixmap());

    m_choosePhotoButton = new QPushButton(this);

    m_labelPhotoTitle = new QLabel(this);

    // Додаємо фото у форму
    formLayout->addRow(m_labelPhotoTitle, m_photoLabel);
    formLayout->addRow(new QLabel(this), m_choosePhotoButton);

    m_nameEdit = new QLineEdit(this);
    m_ageSpinBox = new QSpinBox(this);
    m_ageSpinBox->setRange(18, 99);
    m_cityEdit = new QLineEdit(this);
    m_bioEdit = new QTextEdit(this);
    m_emailEdit = new QLineEdit(this);
    m_phoneEdit = new QLineEdit(this);

    m_genderCombo = new QComboBox(this);
    m_genderCombo->addItem("0");
    m_genderCombo->addItem("1");
    m_genderCombo->addItem("2");

    m_orientationCombo = new QComboBox(this);
    m_orientationCombo->addItem("0");
    m_orientationCombo->addItem("1");
    m_orientationCombo->addItem("2");
    m_orientationCombo->addItem("3");

    m_saveButton = new QPushButton(this);
    m_tagsEdit = new QLineEdit(this);

    m_labelName = new QLabel(this);
    m_labelAge = new QLabel(this);
    m_labelCity = new QLabel(this);
    m_labelEmail = new QLabel(this);
    m_labelPhone = new QLabel(this);
    m_labelGender = new QLabel(this);
    m_labelOrientation = new QLabel(this);
    m_labelTags = new QLabel(this);
    m_labelBio = new QLabel(this);

    formLayout->addRow(m_labelName, m_nameEdit);
    formLayout->addRow(m_labelAge, m_ageSpinBox);
    formLayout->addRow(m_labelCity, m_cityEdit);
    formLayout->addRow(m_labelEmail, m_emailEdit);
    formLayout->addRow(m_labelPhone, m_phoneEdit);
    formLayout->addRow(m_labelGender, m_genderCombo);
    formLayout->addRow(m_labelOrientation, m_orientationCombo);
    formLayout->addRow(m_labelTags, m_tagsEdit);
    formLayout->addRow(m_labelBio, m_bioEdit);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(m_saveButton);
    setLayout(mainLayout);

    retranslateUi();

    connect(m_saveButton, &QPushButton::clicked, this, &ProfilePageWidget::on_btn_SaveProfile_clicked);
    connect(m_choosePhotoButton, &QPushButton::clicked, this, &ProfilePageWidget::onChoosePhoto);
    m_photoPath = "";
}

void ProfilePageWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}

void ProfilePageWidget::retranslateUi() {
    m_labelPhotoTitle->setText(tr("Фото:"));
    m_labelName->setText(tr("Ім'я:"));
    m_labelAge->setText(tr("Вік:"));
    m_labelCity->setText(tr("Місто:"));
    m_labelEmail->setText(tr("Email:"));
    m_labelPhone->setText(tr("Телефон:"));
    m_labelGender->setText(tr("Стать:"));
    m_labelOrientation->setText(tr("Орієнтація:"));
    m_labelTags->setText(tr("Інтереси (Теги):"));
    m_labelBio->setText(tr("Про себе:"));

    m_choosePhotoButton->setText(tr("Обрати фото"));

    if (m_currentUser.getId() != -1) {
        m_saveButton->setText(tr("Оновити профіль"));
    } else {
        m_saveButton->setText(tr("Зберегти профіль"));
    }

    if (m_photoPath.isEmpty()) {
        m_photoLabel->setText(tr("Фото"));
    }

    m_genderCombo->setItemText(0, tr("Не вказано"));
    m_genderCombo->setItemText(1, tr("Чоловік"));
    m_genderCombo->setItemText(2, tr("Жінка"));
    m_orientationCombo->setItemText(0, tr("Гетеро"));
    m_orientationCombo->setItemText(1, tr("Бісексуал"));
    m_orientationCombo->setItemText(2, tr("Гей/Лесбі"));
    m_orientationCombo->setItemText(3, tr("Інше"));
    m_tagsEdit->setPlaceholderText(tr("Введіть теги через кому: Спорт, Кіно, IT..."));
}

void ProfilePageWidget::setDatabaseManager(DatabaseManager* dbManager) {
    m_dbManager = dbManager;
    setupCityAutocomplete();
    loadCurrentProfile();
}

void ProfilePageWidget::setupCityAutocomplete() {
    if (!m_dbManager) return;
    QStringList cities = m_dbManager->getAllCities();
    if (cities.isEmpty()) return;

    QCompleter* completer = new QCompleter(cities, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCompletionMode(QCompleter::PopupCompletion);

    m_cityEdit->setCompleter(completer);
}

void ProfilePageWidget::loadCurrentProfile() {
    if (!m_dbManager) return;

    if (m_dbManager->getCurrentUserProfile(m_currentUser)) {
        setInternalProfile(m_currentUser);

        UserLogger::log(Info, "ProfilePage: Data loaded into UI.");
    } else {
        UserLogger::log(Info, "ProfilePage: No current user found (New Registration).");
    }
}

void ProfilePageWidget::onChoosePhoto() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Обрати фото"),
        "",
        tr("Зображення (*.png *.jpg *.jpeg)")
        );

    if (fileName.isEmpty()) return;

    QDir dir(QCoreApplication::applicationDirPath());
    if (!dir.exists("photos")) dir.mkdir("photos");

    QString targetPath = dir.filePath("photos/" + QFileInfo(fileName).fileName());
    QFile::remove(targetPath);
    QFile::copy(fileName, targetPath);

    m_photoPath = targetPath;

    QPixmap pix(targetPath);
    if (!pix.isNull()) {
        m_photoLabel->setPixmap(
            pix.scaled(m_photoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void ProfilePageWidget::on_btn_SaveProfile_clicked() {
    UserLogger::log(Info, "Save Profile button clicked.");

    if (!m_dbManager) {
        QMessageBox::critical(this, tr("Помилка"), tr("Помилка бази даних! Спробуйте перезапустити."));
        return;
    }

    // ЗБІР ТА ВАЛІДАЦІЯ ДАНИХ
    QString name = m_nameEdit->text().trimmed();
    int age = m_ageSpinBox->value();
    QString email = m_emailEdit->text();
    QString phone = m_phoneEdit->text().trimmed();
    QString city = m_cityEdit->text();
    QString bio = m_bioEdit->toPlainText();
    QString gender = m_genderCombo->currentText();
    QString orientation = m_orientationCombo->currentText();
    QString tagsString = m_tagsEdit->text().trimmed();

    // Валідація
    QRegularExpression nameRegex("^[\\p{L}][\\p{L}\\s\\-']{0,48}[\\p{L}]$");
    QRegularExpression emailRegex("^([a-zA-Z0-9_\\-\\.]+)@([a-zA-Z0-9_\\-\\.]+)\\.([a-zA-Z]{2,5})$");
    QRegularExpression phoneRegex("^\\+?[0-9\\s\\-()]{7,20}$");
    if(name.isEmpty() || email.isEmpty() || age < 18) {
        QMessageBox::warning(this, tr("Увага"), tr("Ім'я, Email та вік (мінімум 18) є обов'язковими!"));
        return;
    }
    if (!nameRegex.match(name).hasMatch()) {
        QMessageBox::warning(this, tr("Увага"), tr("Ім'я може містити лише букви, пробіли, дефіси та апострофи."));
        return;
    }
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, tr("Увага"), tr("Невірний формат електронної пошти."));
        return;
    }
    if (!phone.isEmpty() && !phoneRegex.match(phone).hasMatch()) {
        QMessageBox::warning(this, tr("Увага"), tr("Невірний формат телефону. Використовуйте тільки цифри та '+'."));
        return;
    }

    int currentId = m_currentUser.getId();
    int resultId = -1;

    UserProfile newProfile(
        currentId, name, age, city, bio, gender, orientation, m_photoPath
    );
    ContactInfo contacts(phone, email);
    newProfile.setContactInfo(contacts);

    // ЛОГІКА ЗБЕРЕЖЕННЯ
    if (currentId != -1) {
        if (m_dbManager->updateProfile(newProfile)) resultId = currentId;
    } else {
        if (m_dbManager->profileExists(email)) {
            QMessageBox::warning(this, tr("Помилка"), tr("Профіль з таким Email вже існує."));
            return;
        }
        resultId = m_dbManager->saveProfile(newProfile);
    }

    if (resultId > 0) {
        newProfile.setId(resultId);
        m_currentUser = newProfile;

        if (m_dbManager) {
            m_dbManager->removeAllTags(resultId);

            if (!tagsString.isEmpty()) {
                QStringList tags = tagsString.split(",", Qt::SkipEmptyParts);
                for (const QString& tag : tags) {
                    m_dbManager->addTag(resultId, tag.trimmed());
                }
            }
        }

        QSettings settings("DatingAgency", "Match++");
        settings.setValue("current_user_id", resultId);

        UserLogger::log(Info, QString("Profile saved/updated successfully. ID: %1").arg(resultId));
        QMessageBox::information(this, tr("Успіх"), tr("Профіль успішно збережено!"));

        retranslateUi();
        emit profileSaved();
    } else {
        UserLogger::log(Error, "Failed to execute DB operation.");
        QMessageBox::critical(this, tr("Помилка"), tr("Не вдалося виконати операцію збереження профілю."));
    }
}


void ProfilePageWidget::setInternalProfile(const UserProfile& profile) {
    m_currentUser = profile;

    m_nameEdit->setText(m_currentUser.getName());
    m_ageSpinBox->setValue(m_currentUser.getAge());
    m_cityEdit->setText(m_currentUser.getCity());
    m_bioEdit->setText(m_currentUser.getBio());
    m_emailEdit->setText(m_currentUser.getContactInfo().getEmail());
    m_phoneEdit->setText(m_currentUser.getContactInfo().getPhone());
    m_genderCombo->setCurrentText(m_currentUser.getGender());
    m_orientationCombo->setCurrentText(m_currentUser.getOrientation());

    // ВІДОБРАЖЕННЯ ФОТО
    m_photoPath = profile.getPhotoPath();

    if (!m_photoPath.isEmpty()) {
        QPixmap pix(m_photoPath);
        if (!pix.isNull()) {
            m_photoLabel->setPixmap(
                pix.scaled(m_photoLabel->size(),
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation));
        }
    } else {
        m_photoLabel->clear();
        m_photoLabel->setText(tr("Немає фото"));
    }

    retranslateUi();
    UserLogger::log(Info, "ProfilePage: Internal state updated from MainWindow.");
}

void ProfilePageWidget::clearFields() {
    m_nameEdit->clear();
    m_cityEdit->clear();
    m_bioEdit->clear();
    m_phoneEdit->clear();
    m_emailEdit->clear();

    m_ageSpinBox->setValue(18);

    m_genderCombo->setCurrentIndex(0);
    m_orientationCombo->setCurrentIndex(0);

    m_currentUser.setId(-1);
    m_photoPath = "";
    m_photoLabel->clear();

    retranslateUi();
}