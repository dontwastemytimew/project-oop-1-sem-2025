#include "MatchEngine.h"
#include "UserLogger.h"
#include "UserProfile.h"
#include <QtMath>
#include <QSettings>
#include <QList>
#include <algorithm>

MatchEngine::MatchEngine(DatabaseManager* dbManager)
    : m_dbManager(dbManager)
{
}

bool MatchEngine::isCompatible(const UserProfile& p1, const UserProfile& p2) const
{
    // 1. Не можеш бути сумісним із собою
    if (p1.getId() == p2.getId() && p1.getId() != -1)
        return false;

    // 2. Критична несумісність (орієнтація/гендер)
    if (compatibilityScore(p1, p2) < 0) {
        return false;
    }

    int score = compatibilityScore(p1, p2);

    // Визначаємо, чи сумісність > 0. Це дозволить профілям з 1% сумісності відображатися.
    return score > 0;
}

int MatchEngine::compatibilityScore(const UserProfile& p1, const UserProfile& p2) const
{
    int score = 0;

    // КРИТИЧНА НЕСУМІСНІСТЬ

    // Гетеро (p1) шукає протилежну стать
    if (p1.getOrientation() == "Гетеро" && p1.getGender() == p2.getGender()) {
        return -1000;
    }
    // Гей/Лесбі (p1) шукає ту ж саму стать
    else if (p1.getOrientation() == "Гей/Лесбі" && p1.getGender() != p2.getGender()) {
        return -1000;
    }
    // Бісексуал та Інше завжди сумісні.

    // БАЛИ ЗА КРИТЕРІЯМ

    // Місто (+20 балів)
    if (!p1.getCity().isEmpty() && p1.getCity() == p2.getCity()) {
        score += 20;
    }

    // Вік (різниця < 3 років = +20 балів)
    int ageDiff = qAbs(p1.getAge() - p2.getAge());
    if (ageDiff <= 2) {
        score += 20;
    } else if (ageDiff < 6) {
        score += 10; // Бонус за близький вік
    }

    // Біо (+5 балів за наявність)
    if (!p1.getBio().isEmpty() && !p2.getBio().isEmpty()) {
        score += 5;
    }

    // ЗБІГ ТЕГІВ (+10 за тег)
    if (m_dbManager) {
        QList<QString> p1Tags = m_dbManager->getTagsForUser(p1.getId());
        QList<QString> p2Tags = m_dbManager->getTagsForUser(p2.getId());

        for (const QString& tag : p1Tags) {
            if (p2Tags.contains(tag)) {
                score += 10; // +10 балів за збіг інтересу
            }
        }
    }

    return score;
}

int MatchEngine::compatibilityPercent(const UserProfile& p1, const UserProfile& p2) const
{
    int score = compatibilityScore(p1, p2);

    if (score < 0)
        return 0;

    // Максимальний бал (20 місто + 20 вік <2 + 5 біо + 50 теги (5*10) = 95)
    const int MAX_SCORE = 105;

    int percent = qFloor((score * 100.0) / MAX_SCORE);

    if (percent > 100) percent = 100;

    return percent;
}


QList<QPair<UserProfile, int>> MatchEngine::getSortedMatches(int userId) const
{
    QList<QPair<UserProfile, int>> result;

    // 1. Отримуємо профіль поточного користувача
    UserProfile me;
    if (!m_dbManager->getCurrentUserProfile(me)) {
        UserLogger::log(Error, "MatchEngine failed: Could not load current user profile.");
        return result;
    }

    // 2. Отримуємо список ID, з якими стався взаємний лайк
    QList<int> matchIds = m_dbManager->getMutualMatchIds(userId);
    if (matchIds.isEmpty()) return result;

    // 3. Завантажуємо повні профілі цих ID
    QList<UserProfile> matchedUsers = m_dbManager->getProfilesByIds(matchIds);

    // 4. Обчислюємо Score для кожного і додаємо до списку
    for (const UserProfile& target : matchedUsers)
    {
        int percent = compatibilityPercent(me, target);
        result.append({target, percent});
    }

    // 5. Сортування (від найбільшого відсотка до найменшого)
    std::sort(result.begin(), result.end(),
              [](const auto& a, const auto& b){
                  return a.second > b.second;
              });

    return result;
}