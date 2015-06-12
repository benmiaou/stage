#include "MyHistogram.hpp"
#include <QPushButton>
#include <QApplication>
#include <QKeyEvent>
#include <sstream>
#include <string>
#include <dirent.h>
#include <vector>
#include <QAction>
#include <QMenuBar>
#include <QLayout>
#include <QSignalMapper>
#include <iostream>
#include <QStatusBar>
#include <QPainter>
#include <QTimer>


MyHistogram::MyHistogram(Controller *controller)
{
    this->controller = controller;
    this->resize(800,600);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->update();
    scrollBarMin = new QScrollBar(Qt::Horizontal);
    scrollBarMin->setMinimum(0);
    scrollBarMin->setMaximum(255);
    scrollBarMin->setPageStep(1);
    layout->addWidget(scrollBarMin);
    histogramLabel = new MyHistogramLabel(this);
    histogramLabel->resize(this->size());
    layout->addWidget(histogramLabel);
    scrollBarMax = new QScrollBar(Qt::Horizontal);
    scrollBarMax->setMinimum(0);
    scrollBarMax->setMaximum(255);
    scrollBarMax->setPageStep(1);
    scrollBarMax->setValue(254);
    layout->addWidget(scrollBarMax);
    this->setLayout(layout);
    Histogram();
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start(160);

}

void MyHistogram::Histogram(){
    std::vector<int> histogram = controller->getHistogram();
    histogramLabel->updateHistogram(histogram);
    this->update();
}

void MyHistogram::paintEvent(QPaintEvent *event)
{
    int widthRatio = histogramLabel->width()/255;
    int mardging = 10;
     painter = new QPainter();
    if(!painter->isActive())
        painter->begin(this);
    painter->setPen(Qt::red);
    painter->drawLine ((scrollBarMin->value()+mardging)*widthRatio,0,(scrollBarMin->value()+mardging)*widthRatio,this->height());
    painter->drawLine ((scrollBarMax->value()+mardging)*widthRatio,0,(scrollBarMax->value()+mardging)*widthRatio,this->height());
    scrollBarMin->resize(histogramLabel->width(),30);
    scrollBarMax->resize(histogramLabel->width(),30);
    if(scrollBarMin->value() > scrollBarMax->value())
        scrollBarMax->setValue(scrollBarMin->value());
    controller->setThreshold(scrollBarMax->value(),scrollBarMin->value());

}


