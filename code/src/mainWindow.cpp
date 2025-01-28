#include "mainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QLabel>

MainWindow::MainWindow(QWidget *aParent) : QWidget(aParent)
{
    // Set the initial window size to 500x600
    setFixedSize(500, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *layout = new QHBoxLayout();

    // Circle widget, takes most of the space
    mCircleWidget = new CircleWidget(this);

    // Slider widget
    QSlider *slider = new QSlider(Qt::Horizontal, this); // Horizontal slider
    slider->setRange(10, 200);
    slider->setValue(50);
    QLabel *label = new QLabel("Adjust Circle Size", this);

    // Right-side vertical layout for the slider and label
    QWidget *sliderContainer = new QWidget(this);
    sliderContainer->setStyleSheet("background-color: gray;");
    sliderContainer->setFixedWidth(100); // Set fixed width to 150 pixels
    QVBoxLayout *sliderLayout = new QVBoxLayout(sliderContainer);
    sliderLayout->addWidget(label);
    sliderLayout->addWidget(slider);
    sliderLayout->setAlignment(Qt::AlignTop);

    // Add circle widget and slider layout to the horizontal layout
    layout->addWidget(mCircleWidget);
    layout->addWidget(sliderContainer);

    mainLayout->addLayout(layout);

    connect(slider, &QSlider::valueChanged, mCircleWidget, &CircleWidget::setRadius);
}