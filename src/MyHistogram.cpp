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
#include "DGtal/base/Common.h"
#include "DGtal/io/Display3D.h"
#include "DGtal/io/viewers/Viewer3D.h"

using namespace DGtal;
using namespace Z3i;



MyHistogram::MyHistogram(Controller *controller)
{
    actualUpper = 255;
    actualLower = 0;
    painter = new QPainter();
    QWidget *centralWidget = new QWidget();
    this->setCentralWidget(centralWidget);
    this->controller = controller;
    this->resize(800,600);
    QVBoxLayout *layout = new QVBoxLayout();
    scrollBarMin = new MyScrollBar(Qt::Horizontal,this);
    scrollBarMin->setMinimum(0);
    scrollBarMin->setMaximum(255);
    scrollBarMin->setPageStep(1);
    layout->addWidget(scrollBarMin);
    QVBoxLayout *layout2 = new QVBoxLayout();
    viewer = new Viewer3D<>();
    viewer->setParent(this);
   /* histogramLabel = new QLabel(this);
    histogramLabel->setScaledContents(true);*/
    layout2->addWidget(viewer);
    layout2->setSpacing(25);
    layout->addLayout(layout2);
    scrollBarMax = new MyScrollBar(Qt::Horizontal,this);
    scrollBarMax->setMinimum(0);
    scrollBarMax->setMaximum(255);
    scrollBarMax->setPageStep(1);
    scrollBarMax->setValue(255);
    layout->addWidget(scrollBarMax);
    centralWidget->setLayout(layout);

    QMenuBar *menuBar = new QMenuBar();
    QMenu *processMenu =  menuBar->addMenu(tr("&Action"));
    Threshold = new QAction(tr("&Apply Threshold"), this);
    Threshold->setStatusTip(tr("Apply Threshold"));
    Threshold->setCheckable(true);
    connect(Threshold, SIGNAL(triggered()),this ,SLOT(activateThreshold()));
    showThreshold= new QAction(tr("&Show Threshold"), this);
    showThreshold->setStatusTip(tr("Show Threshold"));
    showThreshold->setCheckable(true);
    connect(showThreshold, SIGNAL(triggered()),this ,SLOT(update()));
    processMenu->addAction(Threshold);
    processMenu->addAction(showThreshold);
    this->setMenuBar(menuBar);


}


void MyHistogram::activateThreshold(){
    controller->activeThreshold(Threshold->isChecked());
    this->update();
}

void MyHistogram::updateHistogram(){
    histograms = controller->getHistograms();
}

void MyHistogram::paintEvent(QPaintEvent *event)
{    
    viewer->clear();
    int minVal = scrollBarMin->value();
    int maxVal = scrollBarMax->value();
    for(int i=0; i<histograms.size() ; i++)
        for(int j=0; j<histograms[i].size(); j++){
            Point pBL = Point(j,0,i);
            Point pTL = Point(j,histograms[i][j]/100,i);
            if(j > minVal && j < maxVal){
                *viewer <<  CustomColors3D( Color( 255, i%255, (j*5)%255 ), Color( 255, i%255, (j*5)%255 ) );
                 viewer->addLine(pBL,pTL,1);
            }
            else
                if(showThreshold->isChecked()){
                    *viewer <<  CustomColors3D( Color( 0, i%255, (j*5)%255 ), Color( 0, i%255, (j*5)%255 ) );
                    viewer->addLine(pBL,pTL,1);
                }
        }
    *viewer<< Viewer3D<>::updateDisplay;
    viewer->update();
    if(minVal != actualLower)
        if(minVal > maxVal)
            scrollBarMax->setValue(minVal);
    if(maxVal != actualUpper)
        if(minVal > maxVal)
            scrollBarMin->setValue(maxVal);
    controller->setThreshold(scrollBarMax->value(),scrollBarMin->value());
    }

void MyHistogram::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->hide();
}
