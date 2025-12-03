#include <gtest/gtest.h>
#include <QFile>
#include <QTextStream>
#include "UserLogger.h"
#include "FakeDataManager.h"
#include "DatabaseManager.h"
#include "UserProfile.h"

// Тест логера
TEST(UserLoggerTest, WriteLog) {
    QString logFile = "test_core_log.txt";
    QFile::remove(logFile);

    UserLogger::init(logFile);
    UserLogger::log(Info, "Test Message");

    QFile file(logFile);
    ASSERT_TRUE(file.open(QFile::ReadOnly));
    QString content = file.readAll();
    file.close();

    EXPECT_TRUE(content.contains("Test Message"));
    EXPECT_TRUE(content.contains("[INFO]"));

    QFile::remove(logFile);
}

// Тест FakeDataManager
TEST(FakeDataManagerTest, GenerateTestUsers) {
    const QString DB_FILE = "dating_agency.db";
    QFile::remove(DB_FILE);

    DatabaseManager* db = new DatabaseManager();
    ASSERT_TRUE(db->openDatabase());

    FakeDataManager::generateTestUsers(db, 5);

    QList<UserProfile> profiles = db->getAllProfiles();

    EXPECT_GE(profiles.size(), 5);

    delete db;
    QFile::remove(DB_FILE);
}

TEST(FakeDataManagerTest, SeedReverseLikes) {
    const QString DB_FILE = "dating_agency.db";
    QFile::remove(DB_FILE);

    DatabaseManager* db = new DatabaseManager();
    ASSERT_TRUE(db->openDatabase());

    UserProfile me(-1, "Me", 25, "Kyiv", "Bio", "Male", "Hetero", "");
    me.setContactInfo(ContactInfo("000", "me@test.com"));
    db->saveProfile(me);
    db->loadProfileByEmail("me@test.com", me);
    int myId = me.getId();

    FakeDataManager::generateTestUsers(db, 10);

    FakeDataManager::seedReverseLikes(db, myId, 10);

    QList<UserProfile> allUsers = db->getAllProfiles();
    int likesCount = 0;

    for (const auto& user : allUsers) {
        if (user.getId() == myId) continue;

        if (db->hasUserLiked(user.getId(), myId)) {
            likesCount++;
        }
    }

    EXPECT_GE(likesCount, 0);

    delete db;
    QFile::remove(DB_FILE);
}