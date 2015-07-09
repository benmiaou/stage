#include "MyHistogramLabel.hpp"

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

void MyHistogramLabel::selectedMinMax(int min, int max){
    this->min = min;
    this->max = max;
}

void MyHistogramLabel::paintEvent(QPaintEvent *event)
{
    int maxValue = 0;
    int mardging = 10;
    for(int i =0; i< histogram.size(); i++){
        if(histogram[i] > maxValue)
            maxValue = histogram[i];
    }
    int base = this->height();
    float heightRatio = (float)base/maxValue;
    int widthRatio = this->width()/255;
    painter = new QPainter();
    if(!painter->isActive())
        painter->begin(this);
    painter->setRenderHint(QPainter::Antialiasing);
    for(int i =0; i< histogram.size(); i++){
        if(i < max && i > min)
            painter->setPen(Qt::red);
        else
            painter->setPen(Qt::black);
        painter->drawLine ((i*widthRatio)+mardging*widthRatio,base,(i*widthRatio)+mardging*widthRatio,((base)-(histogram[i]*heightRatio)));
    }
}
