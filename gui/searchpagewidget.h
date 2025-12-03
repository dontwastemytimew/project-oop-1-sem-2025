#ifndef SEARCHPAGEWIDGET_H
#define SEARCHPAGEWIDGET_H

#include <QSpinBox>
#include <QComboBox>
#include <QStackedWidget>
#include <QLabel>
#include "DatabaseManager.h"
#include "UserProfile.h"
#include "MatchEngine.h"

class SearchPageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchPageWidget(QWidget *parent = nullptr);
    ~SearchPageWidget();

    void setDatabaseManager(DatabaseManager* dbManager);
    void setCurrentUser(const UserProfile& profile);

    signals:
        void matchFound(int currentUserId, int targetId);

protected:
    void changeEvent(QEvent *event) override;

    private slots:
        void on_btn_Find_clicked();
    void on_Like_clicked();
    void on_Skip_clicked();

private:
    void showNextProfile();
    void setupCityAutocomplete();
    void showMatchPopup(const UserProfile& target);
    void retranslateUi();

    DatabaseManager* m_dbManager;
    MatchEngine* m_matchEngine;
    UserProfile m_currentUser;

    QList<UserProfile> m_currentMatches;
    int m_currentMatchIndex;

    QSpinBox* m_minAgeSpin;
    QSpinBox* m_maxAgeSpin;
    QComboBox* m_genderCombo;
    QComboBox* m_orientationCombo;
    QLineEdit* m_cityEdit;
    QPushButton* m_findButton;

    QLabel* m_labelMinAge;
    QLabel* m_labelMaxAge;
    QLabel* m_labelGender;
    QLabel* m_labelOrientation;
    QLabel* m_labelCity;
    QLabel* m_placeholderLabel;

    QStackedWidget* m_resultsStack;
    QPushButton* m_likeButton;
    QPushButton* m_skipButton;
};

#endif // SEARCHPAGEWIDGET_H