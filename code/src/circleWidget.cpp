#include "circleWidget.h"
#include <QPainter>

const double timeStep = 0.016; // 60 FPS

CircleWidget::CircleWidget(QWidget *aParent, int aPosX, int aPosY, int aRadius, float aVelocity, float aAcceleration)
    : QWidget(aParent), mRadius(aRadius), mVelocity(aVelocity), mAcceleration(aAcceleration)
{
    setGeometry(aPosX, aPosY, mRadius, mRadius); // Use setGeometry instead of move to handle initial position.

    mTimer = new QTimer(this);
    connect(mTimer, &QTimer::timeout, this, &CircleWidget::updatePosition);
    mTimer->start(16); // Approximately 60 FPS
}

void CircleWidget::setRadius(int aRadius)
{
    mRadius = aRadius;
    setGeometry(x(), y(), mRadius, mRadius); // Update geometry if radius changes
    update();
}

void CircleWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(Qt::blue);
    painter.drawEllipse(0, 0, mRadius, mRadius); // Drawing the circle at its current position
}

void CircleWidget::updatePosition()
{
    mVelocity += mAcceleration * timeStep; // Update velocity based on acceleration (gravity)
    int newY = y() + mVelocity * timeStep; // Update position based on velocity (distance = velocity * time)

    if (newY + mRadius > parentWidget()->height()) // Check if the ball hits the bottom of the screen
    {
        newY = parentWidget()->height() - mRadius; // Prevent the ball from going off-screen
        mVelocity = 0;                             // Stop the ball
    }

    setGeometry(x(), newY, mRadius, mRadius); // Update the widget's position
}
