#include "settingspagewidget.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QLabel>

SettingsPageWidget::SettingsPageWidget(QWidget *parent) 
    : QWidget(parent), m_mainWindow(nullptr) 
{
    QFormLayout* formLayout = new QFormLayout();
    
    m_langComboBox = new QComboBox(this);
    m_langComboBox->addItem(tr("Українська"), "ua");
    m_langComboBox->addItem(tr("English"), "en");

    formLayout->addRow(tr("Мова (Language):"), m_langComboBox);

    setLayout(formLayout);

    connect(m_langComboBox, &QComboBox::currentIndexChanged, this, &SettingsPageWidget::on_languageChanged);
}

void SettingsPageWidget::setMainWindow(MainWindow* window) {
    m_mainWindow = window;
}

void SettingsPageWidget::on_languageChanged(int index) {
    if (!m_mainWindow) return;

    QString langCode = m_langComboBox->itemData(index).toString(); // Отримуємо код ("ua" або "en")
    m_mainWindow->switchLanguage(langCode);
}