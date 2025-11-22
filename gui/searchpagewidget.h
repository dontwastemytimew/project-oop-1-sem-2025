#ifndef SEARCHPAGEWIDGET_H
#define SEARCHPAGEWIDGET_H

#include <QWidget>
#include <QList>
#include "DatabaseManager.h"
#include "UserProfile.h"

class QSpinBox;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QComboBox;

class SearchPageWidget : public QWidget {
    Q_OBJECT
public:
    explicit SearchPageWidget(QWidget *parent = nullptr);

    void setDatabaseManager(DatabaseManager* dbManager);

    private slots:
        void on_btn_Find_clicked();
    void on_Like_clicked();
    void on_Skip_clicked();

private:
    DatabaseManager* m_dbManager;

    void showNextProfile();

    QSpinBox* m_minAgeSpin;
    QSpinBox* m_maxAgeSpin;
    QLineEdit* m_cityEdit;
    QPushButton* m_findButton;

    QStackedWidget* m_resultsStack;
    QPushButton* m_likeButton;
    QPushButton* m_skipButton;

    QList<UserProfile> m_currentMatches;
    int m_currentMatchIndex;
    QComboBox* m_genderCombo;
    QComboBox* m_orientationCombo;
};

#endif // SEARCHPAGEWIDGET_H