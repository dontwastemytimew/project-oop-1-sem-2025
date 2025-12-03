#include <gtest/gtest.h>
#include <QFile>
#include "DatabaseManager.h"
#include "UserProfile.h"
#include "ContactInfo.h"
#include "UserLogger.h"
#include "Preference.h"

class DatabaseManagerTests : public ::testing::Test {
protected:
    DatabaseManager* dbManager;
    const QString DB_FILE = "dating_agency.db";
    const QString LOG_FILE = "test_db_log.txt";

    void SetUp() override {
        QFile::remove(DB_FILE);
        QFile::remove(LOG_FILE);

        UserLogger::init(LOG_FILE);

        dbManager = new DatabaseManager();

        ASSERT_TRUE(dbManager->openDatabase());
    }

    void TearDown() override {
        delete dbManager;
        QFile::remove(DB_FILE);
        QFile::remove(LOG_FILE);
    }
};

TEST_F(DatabaseManagerTests, SaveAndLoadProfile) {
    ContactInfo info("12345", "test@mail.com");
    UserProfile user(-1, "TestUser", 25, "Kyiv", "Bio");
    user.setContactInfo(info);
    user.setGender("Male");
    user.setOrientation("Hetero");

    ASSERT_TRUE(dbManager->saveProfile(user));
    ASSERT_TRUE(dbManager->profileExists("test@mail.com"));

    UserProfile loadedUser;
    ASSERT_TRUE(dbManager->loadProfileByEmail("test@mail.com", loadedUser));
    EXPECT_EQ(loadedUser.getName(), "TestUser");
    EXPECT_EQ(loadedUser.getAge(), 25);
}

TEST_F(DatabaseManagerTests, UpdateProfile) {
    UserProfile user(-1, "OriginalName", 30, "Lviv", "Bio");
    user.setContactInfo(ContactInfo("000", "update@mail.com"));
    dbManager->saveProfile(user);

    UserProfile toUpdate;
    dbManager->loadProfileByEmail("update@mail.com", toUpdate);
    toUpdate.setName("NewName");

    ASSERT_TRUE(dbManager->updateProfile(toUpdate));

    UserProfile loaded;
    dbManager->loadProfileByEmail("update@mail.com", loaded);
    EXPECT_EQ(loaded.getName(), "NewName");
}

TEST_F(DatabaseManagerTests, DeleteProfile) {
    UserProfile user(-1, "DeleteMe", 40, "Odesa", "Bio");
    user.setContactInfo(ContactInfo("111", "del@mail.com"));
    dbManager->saveProfile(user);

    UserProfile loaded;
    dbManager->loadProfileByEmail("del@mail.com", loaded);

    ASSERT_TRUE(dbManager->deleteProfile(loaded.getId()));
    ASSERT_FALSE(dbManager->profileExists("del@mail.com"));
}

TEST_F(DatabaseManagerTests, SearchByCriteria) {
    // Створюємо двох користувачів
    UserProfile u1(-1, "Anna", 20, "Kyiv", "Bio");
    u1.setContactInfo(ContactInfo("1", "anna@mail.com"));
    dbManager->saveProfile(u1);

    UserProfile u2(-1, "Oleg", 30, "Lviv", "Bio");
    u2.setContactInfo(ContactInfo("2", "oleg@mail.com"));
    dbManager->saveProfile(u2);

    Preference pref(0, 0, "Kyiv");
    QList<UserProfile> results = dbManager->getProfilesByCriteria(pref, -1);

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].getName(), "Anna");
}