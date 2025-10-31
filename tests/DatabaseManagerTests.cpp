#include <gtest/gtest.h>
#include "DatabaseManager.h"
#include "UserProfile.h"
#include "ContactInfo.h"
#include "UserLogger.h"

class DatabaseManagerTests : public ::testing::Test {
protected:
    DatabaseManager* dbManager;

    // Ця функція викликається перед КОЖНИМ тестом
    void SetUp() override {
        // Ініціалізуємо логер (щоб бачити логи під час тестів)
        UserLogger::init("test_log.txt");

        dbManager = new DatabaseManager(); 

        ASSERT_TRUE(dbManager->openDatabase()); 
    }

    // Ця функція викликається після КОЖНОГО тесту
    void TearDown() override {
        delete dbManager; 

        QFile::remove("dating_agency.db"); 
        QFile::remove("test_log.txt");
    }
};


TEST_F(DatabaseManagerTests, SaveNewProfile) {
    ContactInfo contacts("123", "test@example.com");
    UserProfile profile(-1, "Test User", 25, "Test City", "Bio");
    profile.setContactInfo(contacts);

    bool result = dbManager->saveProfile(profile);

    ASSERT_TRUE(result);

    ASSERT_TRUE(dbManager->profileExists("test@example.com"));
}

TEST_F(DatabaseManagerTests, AvoidsDuplicates) {
    ContactInfo contacts("123", "test@example.com");
    UserProfile profile(-1, "Test User", 25, "Test City", "Bio");
    profile.setContactInfo(contacts);
    
    dbManager->saveProfile(profile);

    bool exists = dbManager->profileExists("test@example.com");
    ASSERT_TRUE(exists);
}

TEST_F(DatabaseManagerTests, LoadProfileByEmail) {
    ContactInfo contacts("123", "load_me@example.com");
    UserProfile profile(-1, "Load User", 30, "Load City", "Bio");
    profile.setContactInfo(contacts);
    
    dbManager->saveProfile(profile);

    UserProfile loadedProfile;
    bool loadResult = dbManager->loadProfileByEmail("load_me@example.com", loadedProfile);

    ASSERT_TRUE(loadResult);
    ASSERT_EQ(loadedProfile.getName(), "Load User");
    ASSERT_EQ(loadedProfile.getAge(), 30);
    ASSERT_EQ(loadedProfile.getContactInfo().getPhone(), "123");
}

TEST_F(DatabaseManagerTests, UpdateProfile) {
    ContactInfo contacts("123", "update_me@example.com");
    UserProfile profile(-1, "Original Name", 30, "City", "Bio");
    profile.setContactInfo(contacts);
    dbManager->saveProfile(profile);

    UserProfile profileToUpdate;
    dbManager->loadProfileByEmail("update_me@example.com", profileToUpdate);

    profileToUpdate.setName("Updated Name");
    bool updateResult = dbManager->updateProfile(profileToUpdate);
    ASSERT_TRUE(updateResult);

    UserProfile updatedProfile;
    dbManager->loadProfileByEmail("update_me@example.com", updatedProfile);
    
    ASSERT_EQ(updatedProfile.getName(), "Updated Name");
    ASSERT_EQ(updatedProfile.getId(), profileToUpdate.getId());
}

TEST_F(DatabaseManagerTests, DeleteProfile) {
    ContactInfo contacts("456", "delete_me@example.com");
    UserProfile profile(-1, "Delete User", 40, "City", "Bio");
    profile.setContactInfo(contacts);

    dbManager->saveProfile(profile);

    UserProfile loadedProfile;
    bool loadResult = dbManager->loadProfileByEmail("delete_me@example.com", loadedProfile);
    ASSERT_TRUE(loadResult);

    int profileId = loadedProfile.getId();
    bool deleteResult = dbManager->deleteProfile(profileId);
    ASSERT_TRUE(deleteResult);

    ASSERT_FALSE(dbManager->profileExists("delete_me@example.com"));
}


TEST_F(DatabaseManagerTests, LoadNonExistentProfile) {
    UserProfile emptyProfile;

    bool loadResult = dbManager->loadProfileByEmail("nonexistent@example.com", emptyProfile);

    ASSERT_FALSE(loadResult);
}