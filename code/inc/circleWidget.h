#ifndef CIRCLEWIDGET_H
#define CIRCLEWIDGET_H

#include <QWidget>
#include <QTimer>

class CircleWidget : public QWidget
{
public:
    explicit CircleWidget(QWidget *aParent = nullptr, int aPosX = 0, int aPosY = 0, int aRadius = 10, float aVelocity = 100, float aAcceleration = 9.8);
    void setRadius(int aRadius);
    void setPosition(int aX, int aY);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updatePosition();

private:
    int mRadius;
    float mVelocity;
    float mAcceleration;
    QTimer *mTimer;
};

#endif // CIRCLEWIDGET_H