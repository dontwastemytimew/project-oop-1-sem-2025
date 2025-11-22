#include <gtest/gtest.h>
#include "DatabaseManager.h"
#include "UserProfile.h"
#include "ContactInfo.h"
#include "UserLogger.h"
#include "Preference.h"

class DatabaseManagerTests : public ::testing::Test {
protected:
    DatabaseManager* dbManager;

    // Ця функція викликається перед КОЖНИМ тестом
    void SetUp() override {
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

TEST_F(DatabaseManagerTests, SaveProfile_RejectsEmptyName) {
    ContactInfo contacts("123", "test@example.com");
    UserProfile profile(-1, "", 25, "Test City", "Bio");
    profile.setContactInfo(contacts);

    bool result = dbManager->saveProfile(profile);

    ASSERT_FALSE(result);
}

TEST_F(DatabaseManagerTests, SaveProfile_RejectsInvalidEmail) {
    ContactInfo contacts("123", "test.example.com");
    UserProfile profile(-1, "Test User", 25, "Test City", "Bio");
    profile.setContactInfo(contacts);

    bool result = dbManager->saveProfile(profile);

    ASSERT_FALSE(result);
}

TEST_F(DatabaseManagerTests, SaveProfile_RejectsUnderage) {
    ContactInfo contacts("123", "test@example.com");
    UserProfile profile(-1, "Test User", 17, "Test City", "Bio");
    profile.setContactInfo(contacts);

    bool result = dbManager->saveProfile(profile);

    ASSERT_FALSE(result);
}

class SearchTests : public DatabaseManagerTests {
protected:
    void SetUp() override {
        DatabaseManagerTests::SetUp();


        UserProfile profileA(-1, "Анна", 25, "Київ", "Bio A");
        profileA.setContactInfo(ContactInfo("111", "anna@example.com"));

        UserProfile profileB(-1, "Борис", 30, "Львів", "Bio B");
        profileB.setContactInfo(ContactInfo("222", "boris@example.com"));

        UserProfile profileC(-1, "Вікторія", 35, "Київ", "Bio C");
        profileC.setContactInfo(ContactInfo("333", "vika@example.com"));

        dbManager->saveProfile(profileA);
        dbManager->saveProfile(profileB);
        dbManager->saveProfile(profileC);
    }
};


TEST_F(SearchTests, GetProfilesByCity) {
    Preference prefs(0, 0, "Київ");

    QList<UserProfile> results = dbManager->getProfilesByCriteria(prefs);

    ASSERT_EQ(results.count(), 2);
    ASSERT_EQ(results[0].getName(), "Анна");
    ASSERT_EQ(results[1].getName(), "Вікторія");
}


TEST_F(SearchTests, GetProfilesByAgeRange) {
    Preference prefs(28, 32, "");

    QList<UserProfile> results = dbManager->getProfilesByCriteria(prefs);

    ASSERT_EQ(results.count(), 1);
    ASSERT_EQ(results[0].getName(), "Борис");
}


TEST_F(SearchTests, GetProfilesByAgeAndCity) {
    Preference prefs(30, 40, "Київ");

    QList<UserProfile> results = dbManager->getProfilesByCriteria(prefs);

    ASSERT_EQ(results.count(), 1);
    ASSERT_EQ(results[0].getName(), "Вікторія");
}


TEST_F(SearchTests, GetProfiles_NoResults) {
    Preference prefs(50, 60, "Одеса");

    QList<UserProfile> results = dbManager->getProfilesByCriteria(prefs);

    ASSERT_EQ(results.count(), 0);
}