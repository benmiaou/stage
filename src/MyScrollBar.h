#ifndef MYSCROLLBAR_H
#define MYSCROLLBAR_H

#include <QScrollBar>

class MyScrollBar : public QScrollBar
{
public:
    MyScrollBar(Qt::Orientation orientation, QWidget * parent);
private:
    void sliderChange(SliderChange change);
};

#endif // MYSCROLLBAR_H
