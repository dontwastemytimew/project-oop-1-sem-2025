#include <gtest/gtest.h>
#include <QFile>
#include "MatchEngine.h"
#include "DatabaseManager.h"
#include "UserProfile.h"
#include "ContactInfo.h"

class MatchEngineTests : public ::testing::Test {
protected:
    DatabaseManager* dbManager;
    MatchEngine* engine;
    const QString DB_FILE = "dating_agency.db";

    void SetUp() override {
        QFile::remove(DB_FILE);
        dbManager = new DatabaseManager();
        if (!dbManager->openDatabase()) {
            FAIL() << "Failed to open DB";
        }
        engine = new MatchEngine(dbManager);
    }

    void TearDown() override {
        delete engine;
        delete dbManager;
        QFile::remove(DB_FILE);
    }

    UserProfile createProfileInMemory(const QString& gender, const QString& orientation,
                                      int age, const QString& city) {
        UserProfile p;
        p.setGender(gender);
        p.setOrientation(orientation);
        p.setAge(age);
        p.setCity(city);
        return p;
    }
};

// ТЕСТИ ЛОГІКИ СУМІСНОСТІ
TEST_F(MatchEngineTests, Compatibility_Standard_Score) {
    UserProfile p1 = createProfileInMemory("Чоловік", "Гетеро", 25, "Київ");

    UserProfile p2 = createProfileInMemory("Жінка", "Гетеро", 24, "Київ");

    // Розрахунок очікуваного балу:
    // Орієнтація: Сумісні (немає штрафу -1000)
    // Місто (Київ == Київ): +20
    // Вік (|25-24| <= 2): +20
    // Біо: 0
    // Теги: 0
    // Сума: 40. Відсоток: (40*100)/105 = 38.

    EXPECT_TRUE(engine->isCompatible(p1, p2));
    EXPECT_EQ(engine->compatibilityPercent(p1, p2), 38);
}

TEST_F(MatchEngineTests, Compatibility_Incompatible_Orientation) {
    UserProfile p1 = createProfileInMemory("Чоловік", "Гетеро", 25, "Київ");
    UserProfile p2 = createProfileInMemory("Чоловік", "Гетеро", 25, "Київ");

    EXPECT_FALSE(engine->isCompatible(p1, p2));
    EXPECT_EQ(engine->compatibilityPercent(p1, p2), 0);
}

TEST_F(MatchEngineTests, Compatibility_Age_Mismatch_But_Same_City) {
    // Велика різниця у віці, але одне місто.
    // P1: 20 років. P2: 50 років. Різниця 30.
    // Місто: Київ (+20 балів).

    UserProfile p1 = createProfileInMemory("Чоловік", "Гетеро", 20, "Київ");
    UserProfile p2 = createProfileInMemory("Жінка", "Гетеро", 50, "Київ");

    // Score = 20 (місто) + 0 (вік) = 20.

    EXPECT_TRUE(engine->isCompatible(p1, p2));

    // Відсоток: (20*100)/105 = 19.
    EXPECT_EQ(engine->compatibilityPercent(p1, p2), 19);
}

TEST_F(MatchEngineTests, Compatibility_Total_Mismatch) {
    // Різні міста, велика різниця у віці -> 0 балів -> Несумісні
    UserProfile p1 = createProfileInMemory("Чоловік", "Гетеро", 20, "Київ");
    UserProfile p2 = createProfileInMemory("Жінка", "Гетеро", 50, "Львів");

    // Score = 0 (місто) + 0 (вік)

    EXPECT_FALSE(engine->isCompatible(p1, p2));
    EXPECT_EQ(engine->compatibilityPercent(p1, p2), 0);
}

TEST_F(MatchEngineTests, Bisexual_Compatibility) {
    // Бісексуал сумісний з ким завгодно
    // Але щоб score > 0, потрібне співпадіння міста або віку.
    UserProfile p1 = createProfileInMemory("Чоловік", "Бісексуал", 25, "Київ");
    UserProfile p2 = createProfileInMemory("Чоловік", "Гей/Лесбі", 26, "Київ"); // Вік < 2, Місто +

    // Score = 20 (місто) + 20 (вік) = 40.
    EXPECT_TRUE(engine->isCompatible(p1, p2));
    EXPECT_EQ(engine->compatibilityPercent(p1, p2), 38);
}

// ТЕСТИ ІНТЕГРАЦІЇ З БД (Лайки)
TEST_F(MatchEngineTests, MutualLikes_Logic) {
    UserProfile u1(-1, "User1", 20, "Kyiv", "Bio");
    u1.setContactInfo(ContactInfo("111", "u1@test.com"));
    ASSERT_TRUE(dbManager->saveProfile(u1));

    UserProfile u2(-1, "User2", 20, "Kyiv", "Bio");
    u2.setContactInfo(ContactInfo("222", "u2@test.com"));
    ASSERT_TRUE(dbManager->saveProfile(u2));

    dbManager->loadProfileByEmail("u1@test.com", u1);
    dbManager->loadProfileByEmail("u2@test.com", u2);

    int id1 = u1.getId();
    int id2 = u2.getId();

    EXPECT_TRUE(dbManager->addLike(id1, id2));
    EXPECT_FALSE(dbManager->isMutualLike(id1, id2));

    EXPECT_TRUE(dbManager->addLike(id2, id1));
    EXPECT_TRUE(dbManager->isMutualLike(id1, id2));
}