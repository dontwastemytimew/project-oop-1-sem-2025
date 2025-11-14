#ifndef MATCHESPAGEWIDGET_H
#define MATCHESPAGEWIDGET_H

#include <QWidget>

class MatchesPageWidget : public QWidget {
    Q_OBJECT
public:
    explicit MatchesPageWidget(QWidget *parent = nullptr);
    ~MatchesPageWidget();
};

#endif // MATCHESPAGEWIDGET_H