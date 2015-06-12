#include "MyHistogramLabel.h"
#include <QPainter>
#include <iostream>

MyHistogramLabel::MyHistogramLabel(QWidget * window)
{
    this->setParent(window);
}


void MyHistogramLabel::updateHistogram(std::vector<int> histogram){
    this->histogram = histogram;
    this->update();
}


void MyHistogramLabel::paintEvent(QPaintEvent *event)
{
    int max = 0;
    int mardging = 10;
    for(int i =0; i< histogram.size(); i++){
        if(histogram[i] > max)
            max = histogram[i];
    }
    int base = this->height();
    float heightRatio = (float)base/max;
    int widthRatio = this->width()/255;
    painter = new QPainter();
    if(!painter->isActive())
        painter->begin(this);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::black);
    for(int i =0; i< histogram.size(); i++){
        painter->drawLine ((i*widthRatio)+mardging*widthRatio,base,(i*widthRatio)+mardging*widthRatio,((base)-(histogram[i]*heightRatio)));
    }
}
