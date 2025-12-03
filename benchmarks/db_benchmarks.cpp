#include "DatabaseManager.h"
#include "UserProfile.h"
#include "Preference.h"
#include "FakeDataManager.h"
#include "UserLogger.h"
#include <benchmark/benchmark.h>
#include <QCoreApplication>
#include <QList>

class DBFixture : public benchmark::Fixture {
public:
    DatabaseManager* dbManager;

    void SetUp(const ::benchmark::State& state) {
        static bool loggerInitialized = false;
        if (!loggerInitialized) {
            UserLogger::init("benchmark_log.txt");
            loggerInitialized = true;
        }

        dbManager = new DatabaseManager();
        if (!dbManager->openDatabase()) {
            qCritical("Failed to open database!");
            exit(1);
        }

        static bool seeded = false;
        if (!seeded && dbManager->countUsers() < 10000) {
            QList<UserProfile> profiles = FakeDataManager::generateList(10000);
            dbManager->saveProfileBulk(profiles);
            seeded = true;
        }
    }

    void TearDown(const ::benchmark::State& state) {
        dbManager->closeDatabase();
        delete dbManager;
    }
};


// 1. ТЕСТ ШВИДКОСТІ ЗАПИСУ (BULK INSERT)
BENCHMARK_F(DBFixture, BM_BulkInsert)(benchmark::State& state) {
    QList<UserProfile> profilesToInsert = FakeDataManager::generateList(1000);

    for (auto _ : state) {
        dbManager->saveProfileBulk(profilesToInsert);
    }
}

// 2. ТЕСТ ШВИДКОСТІ ФІЛЬТРАЦІЇ (SELECT WHERE)
BENCHMARK_F(DBFixture, BM_SqlFilter)(benchmark::State& state) {
    Preference prefs(25, 35, "Kyiv", "Чоловік", "Гетеро");
    int currentId = 1;

    for (auto _ : state) {
        dbManager->getProfilesByCriteria(prefs, currentId);
    }
}

// 3. ТЕСТ ШВИДКОСТІ АГРЕГАЦІЇ (GROUP BY / COUNT)
BENCHMARK_F(DBFixture, BM_Aggregation)(benchmark::State& state) {
    for (auto _ : state) {
        dbManager->getGenderStatistics();
    }
}


// 4. ТЕСТ ШВИДКОСТІ ОТРИМАННЯ СПИСКУ МІСТ (DISTINCT)
BENCHMARK_F(DBFixture, BM_GetCities)(benchmark::State& state) {
    for (auto _ : state) {
        dbManager->getAllCities();
    }
}

//5. WRITE: Одинична вставка (Slow)
BENCHMARK_F(DBFixture, BM_SaveProfile_Single)(benchmark::State& state) {
    UserProfile p(-1, "TestUser", 25, "Kyiv", "Bio", "Чоловік", "Гетеро");
    p.setContactInfo(ContactInfo("123", "single_insert@test.com"));

    for (auto _ : state) {
        dbManager->saveProfile(p);
    }
}

// 6. WRITE: Масова вставка (Fast)
BENCHMARK_F(DBFixture, BM_SaveProfile_Bulk)(benchmark::State& state) {
    auto profiles = FakeDataManager::generateList(1000);

    for (auto _ : state) {
        dbManager->saveProfileBulk(profiles);
    }
}

// 7. READ: Пошук за критеріями
BENCHMARK_F(DBFixture, BM_Search_Criteria)(benchmark::State& state) {
    Preference prefs(20, 30, "Kyiv", "Жінка", "Гетеро");

    for (auto _ : state) {
        dbManager->getProfilesByCriteria(prefs, 1);
    }
}

// 8. READ: Унікальні міста
BENCHMARK_F(DBFixture, BM_GetAllCities)(benchmark::State& state) {
    for (auto _ : state) {
        dbManager->getAllCities();
    }
}

// 9. RELATIONS: Взаємні Лайки (INNER JOIN)
BENCHMARK_F(DBFixture, BM_GetMutualMatches)(benchmark::State& state) {
    for (auto _ : state) {
        dbManager->getMutualMatchIds(1);
    }
}

// 10. RELATIONS: Завантаження списку профілів (WHERE IN)
BENCHMARK_F(DBFixture, BM_GetProfilesByIds)(benchmark::State& state) {
    QList<int> ids;
    for(int i=1; i<=100; ++i) ids << i;

    for (auto _ : state) {
        dbManager->getProfilesByIds(ids);
    }
}

// 11. ANALYTICS: Агрегація (GROUP BY)
BENCHMARK_F(DBFixture, BM_Analytics_Gender)(benchmark::State& state) {
    for (auto _ : state) {
        dbManager->getGenderStatistics();
    }
}