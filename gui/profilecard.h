#ifndef PROFILECARD_H
#define PROFILECARD_H

#include <QWidget>
#include <QLabel>
#include "UserProfile.h"

class ProfileCard : public QWidget
{
    Q_OBJECT

public:
    explicit ProfileCard(QWidget* parent = nullptr);
    void setProfileData(const UserProfile& profile);

private:
    QLabel* lblPhoto;
    QLabel* lblName;
    QLabel* lblAge;
    QLabel* lblCity;
    QLabel* lblDescription;
};

#endif
