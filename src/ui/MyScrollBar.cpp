#include "MyScrollBar.h"

MyScrollBar::MyScrollBar(Qt::Orientation orientation, QWidget *parent)
{
    this->setOrientation(orientation);
    this->setParent(parent);
}


void MyScrollBar::sliderChange(SliderChange change){
    this->update();
    this->parentWidget()->update();
}
