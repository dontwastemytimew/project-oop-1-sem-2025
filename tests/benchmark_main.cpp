#include "DatabaseManager.h"
#include "MatchEngine.h"
#include "UserProfile.h"
#include "Preference.h"
#include "UserLogger.h"
#include "BenchmarkTool.h"
#include "FakeDataManager.h"
#include <QCoreApplication>
#include <QDebug>
#include <QRandomGenerator>
#include <QSqlTableModel>

const int MIN_AGE = 25;
const int MAX_AGE = 35;
const int N_Filter_Runs = 100;

void runDatabaseBenchmark(DatabaseManager* dbManager) {
    if (!dbManager) return;

    MatchEngine engine(dbManager);
    const int N_Insert = 1000;
    const int N_Match = 10000;

    // Створення тестових профілів
    UserProfile profile1(1, "UserA", 30, "Kyiv", "", "Чоловік", "Гетеро");
    UserProfile profile2(2, "UserB", 28, "Kyiv", "", "Жінка", "Гетеро");

    // ЗАГАЛЬНІ
    qDebug() << "==================================================";
    qDebug() << "          STARTING DATABASE BENCHMARKS            ";
    qDebug() << "==================================================";

    // ТЕСТ ШВИДКОСТІ ЗАПИСУ (INSERT)
    QList<UserProfile> bulkProfiles = FakeDataManager::generateList(N_Insert);

    BenchmarkTool::run(QString("1. Bulk Insert %1 Profiles (OPTIMIZED WRITE)").arg(N_Insert), 1, [dbManager, bulkProfiles]() {
        dbManager->saveProfileBulk(bulkProfiles);
    });

    // ТЕСТ ШВИДКОСТІ СЕЛЕКТУ З ФІЛЬТРАЦІЄЮ (SELECT WHERE)
    BenchmarkTool::run(QString("2. Search %1 Profiles (SQL filter)").arg(N_Filter_Runs), N_Filter_Runs, [dbManager]() {
        Preference prefs(25, 35, "Kyiv", "Чоловік", "Гетеро");
        dbManager->getProfilesByCriteria(prefs);
    });

    //ТЕСТ ШВИДКОСТІ ЛОГІКИ ЗІСТАВЛЕННЯ (Core Match Logic - CPU)
    BenchmarkTool::run(QString("3. MatchEngine::isCompatible (%1 runs)").arg(N_Match), N_Match, [&engine, &profile1, &profile2]() {
        engine.isCompatible(profile1, profile2);
    });

    // ТЕСТ ШВИДКОСТІ АГРЕГАЦІЇ (GROUP BY / COUNT)
    BenchmarkTool::run("4. DatabaseManager::getGenderStatistics (Aggregation)", 100, [dbManager]() {
        dbManager->getGenderStatistics();
    });

    // Швидкість фільтрації на рівні SQL
    BenchmarkTool::run("5A. Filter in SQL (Fast)", N_Filter_Runs, [dbManager]() {
        Preference prefs(MIN_AGE, MAX_AGE, "", "", "");
        dbManager->getProfilesByCriteria(prefs);
    });

    // Швидкість фільтрації на рівні C++
    BenchmarkTool::run("5B. Filter in C++ (Slow)", N_Filter_Runs, [dbManager]() {
        // Завантажуємо ВСІХ користувачів у пам'ять
        QList<UserProfile> allProfiles = dbManager->getAllProfiles();

        // Фільтруємо у циклі C++
        QList<UserProfile> filteredProfiles;
        for (const UserProfile& profile : allProfiles) {
            if (profile.getAge() >= MIN_AGE && profile.getAge() <= MAX_AGE) {
                filteredProfiles.append(profile);
            }
        }
    });

    // ТЕСТ ШВИДКОСТІ ОНОВЛЕННЯ GUI
    const int N_GUI = 100;
    BenchmarkTool::run("6. GUI Update Speed (QSqlTableModel creation)", N_GUI, [dbManager]() {
        QSqlTableModel* model = dbManager->getUsersModel(nullptr);
        delete model;
    });


    qDebug() << "==================================================";
    qDebug() << "       BENCHMARK COMPLETE. CHECK DB SIZE.         ";
    qDebug() << "==================================================";
}

int main(int argc, char *argv[]) {
    QCoreApplication::setOrganizationName("DatingAgency");
    QCoreApplication::setApplicationName("TitleApp");

    QCoreApplication a(argc, argv);

    UserLogger::init();

    DatabaseManager dbManager;
    if (!dbManager.openDatabase()) {
        qDebug() << "Failed to open database! Exiting.";
        return 1;
    }

    if (dbManager.countUsers() < 10000) {
        qDebug() << "DB has less than 10,000 users. Seeding data...";

        // Генеруємо 10 000 профілів
        QList<UserProfile> seedingProfiles = FakeDataManager::generateList(10000);

        QElapsedTimer seedTimer;
        seedTimer.start();

        dbManager.saveProfileBulk(seedingProfiles);

        qDebug() << QString("Seeding complete in %1 ms.").arg(seedTimer.elapsed());
    }

    runDatabaseBenchmark(&dbManager);

    return 0;
}