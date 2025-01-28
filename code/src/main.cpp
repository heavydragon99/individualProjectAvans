#include <iostream>
#include <vector>

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QPainter>

// OpenCL includes
#include <opencl.hpp>

class CircleWidget : public QWidget
{
public:
    CircleWidget(QWidget *parent = nullptr) : QWidget(parent), radius(50) {}

    void setRadius(int r)
    {
        radius = r;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Qt::blue);
        painter.drawEllipse((width() - radius) / 2, (height() - radius) / 2, radius, radius);
    }

private:
    int radius;
};

class MainWindow : public QWidget
{
public:
    MainWindow(QWidget *parent = nullptr) : QWidget(parent)
    {
        // Set the initial window size to 500x600
        setFixedSize(500, 600);

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        QHBoxLayout *layout = new QHBoxLayout();

        // Circle widget, takes most of the space
        circleWidget = new CircleWidget(this);

        // Slider widget
        QSlider *slider = new QSlider(Qt::Horizontal, this); // Horizontal slider
        slider->setRange(10, 200);
        slider->setValue(50);
        QLabel *label = new QLabel("Adjust Circle Size", this);

        // Right-side vertical layout for the slider and label
        QWidget *sliderContainer = new QWidget(this);
        sliderContainer->setStyleSheet("background-color: gray;");
        sliderContainer->setFixedWidth(100); // Set fixed width to 100 pixels
        QVBoxLayout *sliderLayout = new QVBoxLayout(sliderContainer);
        sliderLayout->addWidget(label);
        sliderLayout->addWidget(slider);
        sliderLayout->setAlignment(Qt::AlignTop);

        // Add circle widget and slider layout to the horizontal layout
        layout->addWidget(circleWidget);
        layout->addWidget(sliderContainer);

        mainLayout->addLayout(layout);

        connect(slider, &QSlider::valueChanged, circleWidget, &CircleWidget::setRadius);
    }

private:
    CircleWidget *circleWidget;
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
