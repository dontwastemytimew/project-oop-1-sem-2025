#include "MatchEngine.h"
#include "UserProfile.h"
#include <benchmark/benchmark.h>
#include <algorithm>
#include <QStringList>
#include <vector>
#include "FakeDataManager.h"
#include <QImage>
#include <QPixmap>
#include <QStringBuilder>

static UserProfile p1(1, "UserA", 30, "Kyiv", "", "Чоловік", "Гетеро");
static UserProfile p2(2, "UserB", 28, "Kyiv", "", "Жінка", "Гетеро");

// 1. ТЕСТ ШВИДКОСТІ ЛОГІКИ ЗІСТАВЛЕННЯ
static void BM_CompatibilityScore(benchmark::State& state) {
    MatchEngine engine(nullptr); 
    
    for (auto _ : state) {
        engine.compatibilityScore(p1, p2);
    }
}

// 2. ТЕСТ ШВИДКОСТІ ОБРОБКИ ТЕГІВ (String Operations)
static void BM_TagsProcessing(benchmark::State& state) {
    QString tagsString = "Кіно, IT, Подорожі, Спорт, Кулінарія, Коти, Собаки";
    QString searchTag = "Коти";

    for (auto _ : state) {
        QStringList tags = tagsString.split(",", Qt::SkipEmptyParts);
        bool found = tags.contains(searchTag);
        benchmark::DoNotOptimize(found);
    }
}
BENCHMARK(BM_TagsProcessing)->Iterations(100000);

// 3. ТЕСТ ШВИДКОСТІ СОРТУВАННЯ
static void BM_Sorting(benchmark::State& state) {
    QList<int> scores;
    for(int i=0; i<1000; ++i) scores.append(rand() % 100);

    for (auto _ : state) {
        state.PauseTiming();
        QList<int> copy = scores;
        state.ResumeTiming();

        std::sort(copy.begin(), copy.end(), std::greater<int>());
    }
}
BENCHMARK(BM_Sorting)->Iterations(1000);

BENCHMARK(BM_CompatibilityScore)->Iterations(1000000);

// 4. ТЕСТ КОПІЮВАННЯ ОБ'ЄКТІВ
static void BM_UserProfile_Copy(benchmark::State& state) {
    UserProfile source(1, "LongNameForTestingStringAlloc", 30,
                      "SomeCityNameIsHere", "LongBioTextShouldBeCopied",
                      "Man", "Hetero");

    for (auto _ : state) {
        UserProfile copy = source;
        benchmark::DoNotOptimize(copy);
    }
}
BENCHMARK(BM_UserProfile_Copy);

// 5. ТЕСТ СОРТУВАННЯ СКЛАДНИХ ОБ'ЄКТІВ
static void BM_SortProfilesByScore(benchmark::State& state) {
    const int N = state.range(0);
    QList<UserProfile> profiles = FakeDataManager::generateList(N);
    UserProfile myProfile(9999, "Me", 25, "Kyiv", "", "Man", "Hetero");
    MatchEngine engine(nullptr);

    for (auto _ : state) {
        state.PauseTiming();
        QList<UserProfile> copy = profiles;
        state.ResumeTiming();

        std::sort(copy.begin(), copy.end(),
            [&engine, &myProfile](const UserProfile& a, const UserProfile& b) {
                return engine.compatibilityScore(myProfile, a) >
                       engine.compatibilityScore(myProfile, b);
            });
    }
}
BENCHMARK(BM_SortProfilesByScore)->RangeMultiplier(10)->Range(100, 5000);

// 6. ТЕСТ ПОШУКУ
static void BM_FindUserInList(benchmark::State& state) {
    const int N = 1000;
    QList<UserProfile> list = FakeDataManager::generateList(N);
    int targetId = list.last().getId();

    for (auto _ : state) {
        auto it = std::find_if(list.begin(), list.end(),
                              [targetId](const UserProfile& p){ return p.getId() == targetId; });
        benchmark::DoNotOptimize(it);
    }
}
BENCHMARK(BM_FindUserInList);

// 7. GUI ПІДГОТОВКА
static void BM_StringFormatting_Plus(benchmark::State& state) {
    QString name = "Anastasia";
    QString city = "Dnipropetrovsk";
    int age = 25;

    for (auto _ : state) {
        QString result = name + ", " + QString::number(age) + ", " + city;
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_StringFormatting_Plus);

static void BM_StringFormatting_Arg(benchmark::State& state) {
    QString name = "Anastasia";
    QString city = "Dnipropetrovsk";
    int age = 25;

    for (auto _ : state) {
        QString result = QString("%1, %2, %3").arg(name).arg(age).arg(city);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_StringFormatting_Arg);

// 8. РОБОТА З ЗОБРАЖЕННЯМИ
static void BM_ImageScaling(benchmark::State& state) {
    QImage largeImage(3840, 2160, QImage::Format_ARGB32);
    largeImage.fill(Qt::blue);

    for (auto _ : state) {
        QImage small = largeImage.scaled(200, 200, Qt::KeepAspectRatio, Qt::FastTransformation);
        benchmark::DoNotOptimize(small);
    }
}
BENCHMARK(BM_ImageScaling)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();