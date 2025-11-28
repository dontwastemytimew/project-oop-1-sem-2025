#include "profilepagewidget.h"
#include "ui_ProfilePageWidget.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QPixmap>
#include <QDir>
#include <QSettings>
#include <QCompleter>

ProfilePageWidget::ProfilePageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProfilePageWidget)
{
    ui->setupUi(this);

    // Кнопка вибору фото
    connect(ui->choosePhotoButton, &QPushButton::clicked,
            this, &ProfilePageWidget::onChoosePhoto);

    // Кнопка збереження
    connect(ui->saveButton, &QPushButton::clicked,
            this, &ProfilePageWidget::on_btn_SaveProfile_clicked);

    ui->photoLabel->setPixmap(QPixmap()); // очищення фото
}

ProfilePageWidget::~ProfilePageWidget() {
    delete ui;
}

void ProfilePageWidget::setDatabaseManager(DatabaseManager* dbManager) {
    m_dbManager = dbManager;

    // ★ AUTOCOMPLETE міст
    setupCityAutocomplete();

    // Завантажити поточного користувача
    loadCurrentProfile();
}

void ProfilePageWidget::setupCityAutocomplete() {
    if (!m_dbManager)
        return;

    QStringList cities = m_dbManager->getAllCities();
    if (cities.isEmpty())
        return;

    QCompleter* completer = new QCompleter(cities, this);

    completer->setCaseSensitivity(Qt::CaseInsensitive);  // регістр неважливий
    completer->setFilterMode(Qt::MatchContains);         // містить у середині
    completer->setCompletionMode(QCompleter::PopupCompletion);

    ui->cityEdit->setCompleter(completer);
}

void ProfilePageWidget::loadCurrentProfile() {
    if (!m_dbManager)
        return;

    if (m_dbManager->getCurrentUserProfile(m_currentUser)) {
        ui->nameEdit->setText(m_currentUser.getName());
        ui->ageSpinBox->setValue(m_currentUser.getAge());
        ui->cityEdit->setText(m_currentUser.getCity());
        ui->bioEdit->setText(m_currentUser.getBio());
        ui->emailEdit->setText(m_currentUser.getContactInfo().getEmail());
        ui->phoneEdit->setText(m_currentUser.getContactInfo().getPhone());
        ui->genderCombo->setCurrentText(m_currentUser.getGender());
        ui->orientationCombo->setCurrentText(m_currentUser.getOrientation());

        m_photoPath = m_currentUser.getPhotoPath();

        if (!m_photoPath.isEmpty()) {
            QPixmap pix(m_photoPath);
            if (!pix.isNull()) {
                ui->photoLabel->setPixmap(
                    pix.scaled(ui->photoLabel->size(),
                               Qt::KeepAspectRatio,
                               Qt::SmoothTransformation));
            }
        }

        ui->saveButton->setText(tr("Оновити профіль"));
    }
}

void ProfilePageWidget::onChoosePhoto() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Обрати фото"),
        "",
        tr("Зображення (*.png *.jpg *.jpeg)")
        );

    if (fileName.isEmpty())
        return;

    QDir dir(QCoreApplication::applicationDirPath());
    if (!dir.exists("photos"))
        dir.mkdir("photos");

    QString targetPath = dir.filePath("photos/" + QFileInfo(fileName).fileName());
    QFile::remove(targetPath);
    QFile::copy(fileName, targetPath);

    m_photoPath = targetPath;

    QPixmap pix(targetPath);
    if (!pix.isNull()) {
        ui->photoLabel->setPixmap(
            pix.scaled(ui->photoLabel->size(),
                       Qt::KeepAspectRatio,
                       Qt::SmoothTransformation));
    }
}

void ProfilePageWidget::on_btn_SaveProfile_clicked() {

    if (!m_dbManager) {
        QMessageBox::critical(this, tr("Помилка"), tr("Немає зʼєднання з базою."));
        return;
    }

    QString name = ui->nameEdit->text();
    int age = ui->ageSpinBox->value();
    QString city = ui->cityEdit->text();
    QString bio = ui->bioEdit->toPlainText();
    QString email = ui->emailEdit->text();
    QString phone = ui->phoneEdit->text();
    QString gender = ui->genderCombo->currentText();
    QString orientation = ui->orientationCombo->currentText();

    if (name.isEmpty() || email.isEmpty() || age < 18) {
        QMessageBox::warning(this, tr("Помилка"), tr("Невірно введені дані."));
        return;
    }

    int currentId = m_currentUser.getId();
    int resultId = -1;

    UserProfile newProfile(
        currentId,
        name,
        age,
        city,
        bio,
        gender,
        orientation,
        m_photoPath  // ★ Зберігаємо шлях до фото
        );

    newProfile.setContactInfo(ContactInfo(phone, email));
    newProfile.setPhotoPath(m_photoPath);

    if (currentId != -1) {
        if (m_dbManager->updateProfile(newProfile))
            resultId = currentId;
    } else {
        if (m_dbManager->profileExists(email)) {
            QMessageBox::warning(this, tr("Помилка"), tr("Такий email вже існує."));
            return;
        }
        resultId = m_dbManager->saveProfile(newProfile);
    }

    if (resultId > 0) {
        newProfile.setId(resultId);
        m_currentUser = newProfile;

        QSettings settings("DatingAgency", "TitleApp");
        settings.setValue("current_user_id", resultId);

        QMessageBox::information(this, tr("Успіх"), tr("Профіль збережено!"));
        ui->saveButton->setText(tr("Оновити профіль"));

        emit profileSaved();
    } else {
        QMessageBox::critical(this, tr("Помилка"), tr("Не вдалося зберегти профіль."));
    }
}

void ProfilePageWidget::setInternalProfile(const UserProfile& profile) {

    m_currentUser = profile;

    ui->nameEdit->setText(profile.getName());
    ui->ageSpinBox->setValue(profile.getAge());
    ui->cityEdit->setText(profile.getCity());
    ui->bioEdit->setText(profile.getBio());
    ui->emailEdit->setText(profile.getContactInfo().getEmail());
    ui->phoneEdit->setText(profile.getContactInfo().getPhone());
    ui->genderCombo->setCurrentText(profile.getGender());
    ui->orientationCombo->setCurrentText(profile.getOrientation());

    m_photoPath = profile.getPhotoPath();

    if (!m_photoPath.isEmpty()) {
        QPixmap pix(m_photoPath);
        ui->photoLabel->setPixmap(
            pix.scaled(ui->photoLabel->size(),
                       Qt::KeepAspectRatio,
                       Qt::SmoothTransformation));
    }

    ui->saveButton->setText(tr("Оновити профіль"));
}
