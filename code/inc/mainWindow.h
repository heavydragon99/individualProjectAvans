#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "circleWidget.h"

class MainWindow : public QWidget
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    CircleWidget *mCircleWidget;
};

#endif // MAINWINDOW_H