#include "profilepagewidget.h"
#include "ui_ProfilePageWidget.h"

#include "UserLogger.h"
#include "UserProfile.h"
#include "ContactInfo.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QPixmap>

ProfilePageWidget::ProfilePageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProfilePageWidget)
    , m_dbManager(nullptr)
{
    ui->setupUi(this);

    // Кнопка вибору фото
    connect(ui->choosePhotoButton, &QPushButton::clicked,
            this, &ProfilePageWidget::onChoosePhoto);

    // Кнопка збереження профілю
    connect(ui->saveButton, &QPushButton::clicked,
            this, &ProfilePageWidget::on_btn_SaveProfile_clicked);

    // Початкове порожнє фото
    ui->photoLabel->setPixmap(QPixmap());
}

ProfilePageWidget::~ProfilePageWidget() {
    delete ui;
}

void ProfilePageWidget::setDatabaseManager(DatabaseManager* dbManager) {
    m_dbManager = dbManager;
    loadCurrentProfile();
}

void ProfilePageWidget::loadCurrentProfile() {
    if (!m_dbManager) return;

    if (m_dbManager->getCurrentUserProfile(m_currentUser)) {

        ui->nameEdit->setText(m_currentUser.getName());
        ui->ageSpinBox->setValue(m_currentUser.getAge());
        ui->cityEdit->setText(m_currentUser.getCity());
        ui->bioEdit->setText(m_currentUser.getBio());
        ui->emailEdit->setText(m_currentUser.getContactInfo().getEmail());
        ui->phoneEdit->setText(m_currentUser.getContactInfo().getPhone());
        ui->genderCombo->setCurrentText(m_currentUser.getGender());
        ui->orientationCombo->setCurrentText(m_currentUser.getOrientation());

        // ★ Завантажити фото
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

    // ★ Копіюємо фото у директорію програми
    QDir dir(QCoreApplication::applicationDirPath());
    if (!dir.exists("photos"))
        dir.mkdir("photos");

    QString targetPath =
        dir.filePath("photos/" + QFileInfo(fileName).fileName());

    QFile::remove(targetPath); // якщо існує — стерти
    QFile::copy(fileName, targetPath);

    m_photoPath = targetPath;

    // ★ Показ у превʼю
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
        QMessageBox::critical(this, tr("Помилка"), tr("Помилка бази даних!"));
        return;
    }

    // Отримуємо дані з UI
    QString name = ui->nameEdit->text();
    int age = ui->ageSpinBox->value();
    QString city = ui->cityEdit->text();
    QString bio = ui->bioEdit->toPlainText();
    QString email = ui->emailEdit->text();
    QString phone = ui->phoneEdit->text();
    QString gender = ui->genderCombo->currentText();
    QString orientation = ui->orientationCombo->currentText();

    if (name.isEmpty() || email.isEmpty() || age < 18) {
        QMessageBox::warning(this, tr("Увага"), tr("Ім'я, Email та вік (мінімум 18) є обов'язковими!"));
        return;
    }

    int currentId = m_currentUser.getId();
    int resultId = -1;

    // Створюємо профіль
    UserProfile newProfile(
        currentId, name, age, city, bio, gender, orientation, m_photoPath
        );

    newProfile.setPhotoPath(m_photoPath);
    newProfile.setContactInfo(ContactInfo(phone, email));

    if (currentId != -1) {
        // Оновлення
        if (m_dbManager->updateProfile(newProfile)) {
            resultId = currentId;
        }
    } else {
        // Новий профіль
        if (m_dbManager->profileExists(email)) {
            QMessageBox::warning(this, tr("Помилка"), tr("Профіль з таким Email вже існує."));
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
