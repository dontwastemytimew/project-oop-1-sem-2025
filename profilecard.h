#ifndef PROFILECARD_H
#define PROFILECARD_H

#include <QWidget>

class ProfileCard : public QWidget
{
    Q_OBJECT

public:
    explicit ProfileCard(QWidget* parent = nullptr);
    void setProfileData(const QString& name, int age, const QString& city, const QString& desc);

private:
    QLabel* lblPhoto;
    QLabel* lblName;
    QLabel* lblAge;
    QLabel* lblCity;
    QLabel* lblDescription;
};

#endif
