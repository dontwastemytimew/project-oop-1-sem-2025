#ifndef PROFILECARD_H
#define PROFILECARD_H

#include <QLabel>
#include <QWidget>
#include "UserProfile.h"

/**
 * @brief ProfileCard class
 * * Віджет для відображення профілю користувача (використовується на сторінці пошуку).
 */
class ProfileCard : public QWidget
{
    Q_OBJECT

public:
    explicit ProfileCard(QWidget* parent = nullptr);

    /// Встановлює основні дані профілю (фото, імʼя, місто, вік, біо)
    void setProfileData(const UserProfile& profile);

    /// Встановлює % сумісності
    void setCompatibilityPercent(int percent);

private:
    QLabel* lblPhoto;
    QLabel* lblName;
    QLabel* lblAge;
    QLabel* lblCity;
    QLabel* lblDescription;
    QLabel* lblCompatibility;   ///< Відсоток сумісності
};

#endif