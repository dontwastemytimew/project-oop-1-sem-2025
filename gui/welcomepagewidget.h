#ifndef WELCOMEPAGEWIDGET_H
#define WELCOMEPAGEWIDGET_H

#include <QString>
#include <QTimer>
#include <QLabel>

class WelcomePageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WelcomePageWidget(QWidget *parent = nullptr);

    private slots:
    void updateTextAnimation();

private:
    QLabel* m_welcomeTextLabel;
    QTimer* m_animationTimer;
    QString m_fullText;
    int m_currentIndex = 0;
};

#endif //WELCOMEPAGEWIDGET_H
