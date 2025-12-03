#include <gtest/gtest.h>

#include "MatchEngine.h"
#include "UserProfile.h"
#include "DatabaseManager.h"
#include <QSqlQuery>

UserProfile makeProfile(int id, const QString& name, int age, const QString& city,
                        const QString& gender, const QString& orientation)
{
    UserProfile p;
    p.setId(id);
    p.setName(name);
    p.setAge(age);
    p.setCity(city);
    p.setGender(gender);
    p.setOrientation(orientation);
    return p;
}

TEST(MatchEngineTests, HighCompatibilitySameCityCloseAge)
{
    DatabaseManager db;
    MatchEngine engine(&db);

    UserProfile p1 = makeProfile(1, "Аня", 22, "Київ", "Жіноча", "Гетеро");
    UserProfile p2 = makeProfile(2, "Ігор", 23, "Київ", "Чоловіча", "Гетеро");

    int score = engine.compatibilityScore(p1, p2);
    int percent = engine.compatibilityPercent(p1, p2);
    bool compatible = engine.isCompatible(p1, p2);

    EXPECT_GT(score, 0);
    EXPECT_GE(percent, 60);
    EXPECT_TRUE(compatible);
}

TEST(MatchEngineTests, IncompatibleOrientation)
{
    DatabaseManager db;
    MatchEngine engine(&db);

    UserProfile p1 = makeProfile(1, "Марія", 25, "Львів", "Жіноча", "Гетеро");
    UserProfile p2 = makeProfile(2, "Олена", 25, "Львів", "Жіноча", "Гетеро");

    int score = engine.compatibilityScore(p1, p2);
    int percent = engine.compatibilityPercent(p1, p2);
    bool compatible = engine.isCompatible(p1, p2);

    EXPECT_LT(score, 0);
    EXPECT_EQ(percent, 0);
    EXPECT_FALSE(compatible);
}

TEST(MatchEngineTests, AgeDifferenceTooBig)
{
    DatabaseManager db;
    MatchEngine engine(&db);

    UserProfile p1 = makeProfile(1, "Вася", 20, "Київ", "Чоловіча", "Гетеро");
    UserProfile p2 = makeProfile(2, "Оксана", 40, "Київ", "Жіноча", "Гетеро");

    int percent = engine.compatibilityPercent(p1, p2);
    bool compatible = engine.isCompatible(p1, p2);

    EXPECT_LT(percent, 60);
    EXPECT_FALSE(compatible);
}

TEST(DatabaseManagerTests, MutualLikeBecomesMatch)
{
    DatabaseManager db;
    ASSERT_TRUE(db.openDatabase());

    QSqlQuery("DELETE FROM likes");

    int userId = 1;
    int targetId = 2;

    EXPECT_TRUE(db.addLike(userId, targetId));
    EXPECT_FALSE(db.isMutualLike(userId, targetId));

    EXPECT_TRUE(db.addLike(targetId, userId));
    EXPECT_TRUE(db.isMutualLike(userId, targetId));

    QList<int> matches = db.getMutualMatchIds(userId);
    EXPECT_TRUE(matches.contains(targetId));
}
