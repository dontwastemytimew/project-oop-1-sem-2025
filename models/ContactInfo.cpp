#include "models/ContactInfo.h"
#include <QString>


ContactInfo::ContactInfo(const QString &phone, const QString &email)
    : m_phone(phone), m_email(email)
{
}

QString ContactInfo::getPhone() const {
    return m_phone;
}

QString ContactInfo::getEmail() const {
    return m_email;
}

void ContactInfo::setPhone(const QString &newPhone) {
    m_phone = newPhone;
}

void ContactInfo::setEmail(const QString &newEmail) {
    m_email = newEmail;
}