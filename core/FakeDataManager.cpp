#include "FakeDataManager.h"
#include "UserProfile.h"
#include "UserLogger.h"
#include <QRandomGenerator>
#include <QVector>
#include <QDir>

void FakeDataManager::generateTestUsers(DatabaseManager* dbManager, int count) {
    if (!dbManager) return;

    UserLogger::log(Info, QString("Starting generation of %1 fake users...").arg(count));

    int successCount = 0;

    for (int i = 0; i < count; ++i) {
        // Визначаємо стать (0 або 1)
        bool isMale = QRandomGenerator::global()->bounded(2) == 0;
        QString gender = isMale ? "Чоловік" : "Жінка";

        // Обираємо ім'я відповідно до статі
        QString name = isMale ? getRandomMaleName() : getRandomFemaleName();

        // Генерація віку, міста, біо
        int age = QRandomGenerator::global()->bounded(18, 60);
        QString city = getRandomCity();
        QString bio = getRandomBio();

        // Визначаємо орієнтацію
        int orientRand = QRandomGenerator::global()->bounded(100);
        QString orientation;
        if (orientRand < 85) orientation = "Гетеро";
        else if (orientRand < 95) orientation = "Бісексуал";
        else orientation = "Гей/Лесбі";

        // Створюємо контактні дані
        QString phone = generatePhone();
        QString email = generateEmail(name);

        QString photoPath = getRandomPhotoPath(gender);
        UserProfile profile(-1, name, age, city, bio, gender, orientation, photoPath);
        ContactInfo contacts(phone, email);
        profile.setContactInfo(contacts);

        int newId = dbManager->saveProfile(profile);

        if (newId > 0) {

            QList<QString> tags = generateRandomTags();
            for (const QString& tag : tags) {
                dbManager->addTag(newId, tag);
            }

            if (QRandomGenerator::global()->bounded(100) < 50) {
                dbManager->addLike(newId, 1);
            }
            successCount++;
        }
    }
    UserLogger::log(Info, QString("Successfully generated %1 out of %2 users.").arg(successCount).arg(count));
}

QString FakeDataManager::getRandomMaleName() {
    static const QVector<QString> names = {
        "Олександр", "Андрій", "Дмитро", "Максим", "Іван", "Артем",
        "Сергій", "Владислав", "Богдан", "Євген", "Микола", "Володимир"
    };
    int index = QRandomGenerator::global()->bounded(names.size());
    return names[index];
}

QString FakeDataManager::getRandomFemaleName() {
    static const QVector<QString> names = {
        "Марія", "Олена", "Анна", "Юлія", "Вікторія", "Наталія",
        "Тетяна", "Дарина", "Софія", "Ірина", "Катерина", "Оксана", "Крістіна"
    };
    int index = QRandomGenerator::global()->bounded(names.size());
    return names[index];
}

QString FakeDataManager::getRandomName() {
    return getRandomMaleName();
}

QString FakeDataManager::getRandomCity() {
    static const QVector<QString> cities = {
        "Київ", "Львів", "Одеса", "Харків", "Дніпро", "Вінниця", "Запоріжжя",
        "Івано-Франківськ", "Тернопіль", "Полтава", "Чернігів", "Луцьк"
    };
    int index = QRandomGenerator::global()->bounded(cities.size());
    return cities[index];
}

QString FakeDataManager::getRandomBio() {
    static const QVector<QString> bios = {
        "Люблю подорожі та каву.", "Шукаю серйозні стосунки.", "Програміст, люблю котів.",
        "Захоплююсь спортом і музикою.", "Люблю читати книги в дощ.", "Мрію об'їхати весь світ.",
        "Фанатію від кіно та серіалів.", "Люблю готувати смачну їжу.", "Ціную щирість та гумор.",
        "Граю на гітарі, шукаю музу."
    };
    int index = QRandomGenerator::global()->bounded(bios.size());
    return bios[index];
}

QString FakeDataManager::generateEmail(const QString& name) {
    int randomNum = QRandomGenerator::global()->bounded(1000, 9999);
    QString latinName = "user";
    return QString("%1_%2@example.com").arg(latinName).arg(randomNum);
}

QString FakeDataManager::generatePhone() {
    QString phone = "+380";
    for (int i = 0; i < 9; ++i) {
        phone.append(QString::number(QRandomGenerator::global()->bounded(0, 10)));
    }
    return phone;
}

QList<UserProfile> FakeDataManager::generateList(int count) {
    QList<UserProfile> profiles;
    profiles.reserve(count);

    for (int i = 0; i < count; ++i) {
        bool isMale = QRandomGenerator::global()->bounded(2) == 0;
        QString gender = isMale ? "Чоловік" : "Жінка";
        QString name = isMale ? getRandomMaleName() : getRandomFemaleName();

        int age = QRandomGenerator::global()->bounded(18, 60);
        QString city = getRandomCity();
        QString bio = getRandomBio();

        // Орієнтація
        int orientRand = QRandomGenerator::global()->bounded(100);
        QString orientation;
        if (orientRand < 85) orientation = "Гетеро";
        else if (orientRand < 95) orientation = "Бісексуал";
        else orientation = "Гей/Лесбі";

        // Контакти
        quint32 uniqueID = QRandomGenerator::global()->generate();
        QString phone = generatePhone();
        QString email = QString("test_%1@bench.com").arg(uniqueID);
        QString photoPath = getRandomPhotoPath(gender);
        UserProfile profile(-1, name, age, city, bio, gender, orientation, photoPath);
        ContactInfo contacts(phone, email);
        profile.setContactInfo(contacts);

        profiles.append(profile);
    }

    return profiles;
}

QList<QString> FakeDataManager::generateRandomTags() {
    static const QVector<QString> availableTags = {
        "Кіно", "IT", "Подорожі", "Спорт", "Кулінарія",
        "Коти", "Собаки", "Книги", "Музика", "Велосипед",
        "Настільні ігри", "Кава"
    };

    QList<QString> tags;
    int numTags = QRandomGenerator::global()->bounded(2, 6); // 2 to 5 tags

    QList<int> indices;
    while (indices.size() < numTags) {
        int index = QRandomGenerator::global()->bounded(availableTags.size());
        if (!indices.contains(index)) {
            indices.append(index);
            tags.append(availableTags[index]);
        }
    }
    return tags;
}

void FakeDataManager::seedTags(DatabaseManager* dbManager) {
    if (!dbManager) return;

    QList<UserProfile> allUsers = dbManager->getAllProfiles();

    for (const auto& user : allUsers) {
        QList<QString> tags = generateRandomTags();

        for (const QString& tag : tags) {
            dbManager->addTag(user.getId(), tag);
        }
    }
    UserLogger::log(Info, "Tags successfully seeded for all generated users.");
}

void FakeDataManager::seedReverseLikes(DatabaseManager* dbManager, int currentUserId, int count) {
    if (!dbManager || currentUserId <= 0) return;

    QList<UserProfile> allUsers = dbManager->getAllProfiles();

    int profilesToProcess = qMin(count, allUsers.size());

    for (int i = 0; i < profilesToProcess; ++i) {
        int targetId = allUsers[i].getId();

        if (targetId == currentUserId || dbManager->hasUserLiked(targetId, currentUserId)) continue;

        // 50% шанс зворотного лайку
        if (QRandomGenerator::global()->bounded(100) < 50) {
            // Згенерований профіль (targetId) лайкає нас (currentUserId)
            dbManager->addLike(targetId, currentUserId);
        }
    }
    UserLogger::log(Info, QString("Seeding complete. %1 profiles checked for reverse like.").arg(profilesToProcess));
}

QString FakeDataManager::getRandomPhotoPath(const QString& gender) {
    QString resourcePath;

    if (gender == "Чоловік" || gender == "Male") {
        resourcePath = ":/resources/fake_photos/male";
    } else {
        resourcePath = ":/resources/fake_photos/female";
    }

    QDir directory(resourcePath);
    QStringList images = directory.entryList(QStringList() << "*.jpg" << "*.png" << "*.jpeg", QDir::Files);

    if (images.isEmpty()) {
        return "";
    }

    int index = QRandomGenerator::global()->bounded(images.size());
    return resourcePath + "/" + images[index];
}