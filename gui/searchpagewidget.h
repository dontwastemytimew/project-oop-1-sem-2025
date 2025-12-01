#ifndef SEARCHPAGEWIDGET_H
#define SEARCHPAGEWIDGET_H

#include <QWidget>
#include <QVector>
#include "DatabaseManager.h"
#include "UserProfile.h"
#include "profilecard.h"
#include "MatchEngine.h"

class QSpinBox;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QComboBox;

class SearchPageWidget : public QWidget {
    Q_OBJECT
public:
    explicit SearchPageWidget(QWidget *parent = nullptr);
    ~SearchPageWidget();

    void setDatabaseManager(DatabaseManager* dbManager);

    /**
     * @brief Встановлює дані поточного користувача. Викликається при старті додатку.
     */
    void setCurrentUser(const UserProfile& profile);

    /**
     * @brief Повертає ID поточного користувача.
     */
    int getCurrentUserId() const;


    signals:
        void matchFound(int userId, int targetId);

    private slots:
        void on_btn_Find_clicked();
    void on_Like_clicked();
    void on_Skip_clicked();

private:
    DatabaseManager* m_dbManager = nullptr;
    QSpinBox* m_minAgeSpin;
    QSpinBox* m_maxAgeSpin;
    QLineEdit* m_cityEdit;
    QPushButton* m_findButton;
    QComboBox* m_genderCombo;
    QComboBox* m_orientationCombo;
    QStackedWidget* m_resultsStack;
    QPushButton* m_likeButton;
    QPushButton* m_skipButton;
    QVector<UserProfile> m_currentMatches;
    int m_currentMatchIndex = 0;
    UserProfile m_currentUser;
    MatchEngine *m_matchEngine;
    void showNextProfile();

    /**
     * @brief Показує спливаюче вікно про Match.
     * @param target Профіль, з яким відбувся метч.
     */
    void showMatchPopup(const UserProfile& target);
    void setupCityAutocomplete();
};

#endif // SEARCHPAGEWIDGET_H