#include "MyHistogram.hpp"

#include <QMenuBar>
#include <QLayout>




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
    histogramLabel = new MyHistogramLabel(this);
    histogramLabel->resize(this->size());
    layout2->addWidget(histogramLabel);
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
    lungSegmentation = new QAction(tr("&Preview Lung Segmentation"), this);
    lungSegmentation->setStatusTip(tr("Preview Lung Segmentation"));
    lungSegmentation->setCheckable(true);
    connect(lungSegmentation, SIGNAL(triggered()),this ,SLOT(activateLungSegmentation()));
    processMenu->addAction(lungSegmentation);

    simpleThreshold = new QAction(tr("&Preview Simple Threshold"), this);
    simpleThreshold->setStatusTip(tr("Preview Simple Threshold"));
    simpleThreshold->setCheckable(true);
    connect(simpleThreshold, SIGNAL(triggered()),this ,SLOT(activateSimpleThreshold()));
    processMenu->addAction(simpleThreshold);


    QMenu *thresholdMenu =  menuBar->addMenu(tr("&Action"));
    applyLungSeg= new QAction(tr("&Apply Lung Segmentation"), this);
    applyLungSeg->setStatusTip(tr("Apply Lung Segmentation"));
    connect(applyLungSeg, SIGNAL(triggered()),this ,SLOT(applyLungSegmentation()));
    thresholdMenu->addAction(applyLungSeg);

    applySimpleThresh= new QAction(tr("&Apply Simple Threshold"), this);
    applySimpleThresh->setStatusTip(tr("Apply Simple Threshold"));
    connect(applySimpleThresh, SIGNAL(triggered()),this ,SLOT(applySimpleThreshold()));
    thresholdMenu->addAction(applySimpleThresh);



    QMenu *histogramMenu =  menuBar->addMenu(tr("&Histogram Type"));
    QActionGroup* histogramType = new QActionGroup(this);
    histogramType1 = new QAction(tr("&Current 2D"), this);
    histogramType1->setStatusTip(tr("Current 2D"));
    histogramType1->setCheckable(true);
    histogramType1->setChecked(true);
    histogramType1->setActionGroup(histogramType);

    histogramType2 = new QAction(tr("&Total 2D"), this);
    histogramType2->setStatusTip(tr("Total 2D"));
    histogramType2->setCheckable(true);
    histogramType2->setActionGroup(histogramType);

    histogramType3 = new QAction(tr("&3D"), this);
    histogramType3->setStatusTip(tr("3D"));
    histogramType3->setCheckable(true);
    histogramType3->setActionGroup(histogramType);


    histogramMenu->addActions(histogramType->actions());



    this->setMenuBar(menuBar);


}
void MyHistogram::applyLungSegmentation(){
    lungSegmentation->setChecked(false);
    controller->activeLungSegmentation(lungSegmentation->isChecked());
    controller->applyLungSegmentation();
    updateHistogram();
}

void MyHistogram::activateLungSegmentation(){
    controller->activeLungSegmentation(lungSegmentation->isChecked());
    this->update();
}

void MyHistogram::applySimpleThreshold(){
    simpleThreshold->setChecked(false);
    controller->activeSimpleThreshold(simpleThreshold->isChecked());
    controller->applySimpleThreshold();
    updateHistogram();
}

void MyHistogram::activateSimpleThreshold(){
    controller->activeSimpleThreshold(simpleThreshold->isChecked());
    this->update();
}

void MyHistogram::updateHistogram(){
    histograms = controller->getHistograms();
}

void MyHistogram::update3D(){
    viewer->clear();
    int minVal = scrollBarMin->value();
    int maxVal = scrollBarMax->value();
    for(int i=0; i<histograms.size() ; i++)
        for(int j=0; j<histograms[i].size(); j++){
            bool draw = false;
            draw = true;
            Point pBL = Point(j,0,i);
            Point pTL = Point(j,(histograms[i][j]/100)+1,i);
            if(j > minVal && j < maxVal){
                *viewer <<  CustomColors3D( Color( 255, i%255, j), Color( 255, i%255, j) );
                draw = true;
            }
            else
                *viewer <<  CustomColors3D( Color( 0, i%255, j), Color( 0, i%255, j) );
            if(draw)
                viewer->addLine(pBL,pTL,1);
        }
    *viewer<< Viewer3D<>::updateDisplay;
}


void MyHistogram::paintEvent(QPaintEvent *event)
{
    int upper = 255;
    int lower = 0;
    controller->getThresholdValues(upper,lower);
    int minVal = scrollBarMin->value();
    int maxVal = scrollBarMax->value();
    if(minVal != actualLower)
        if(minVal > maxVal)
            scrollBarMax->setValue(minVal);
    if(maxVal != actualUpper)
        if(minVal > maxVal)
            scrollBarMin->setValue(maxVal);
    minVal = scrollBarMin->value();
    maxVal = scrollBarMax->value();
    if(histogramType3->isChecked()){
        viewer->show();
        update3D();
        histogramLabel->hide();
    }
    else{
        viewer->hide();
        histogramLabel->show();
        std::vector<int> histogram;
        if(histogramType1->isChecked())
            histogram = histograms[controller->getCurrent()];
        else{
            for(int j=1; j<histograms[0].size(); j++)
                for(int i=0; i<histograms.size() ; i++){
                    if(histogram.size() > j)
                        histogram[j] += histograms[i][j];
                    else
                        histogram.push_back(histograms[i][j]);
                }
        }
        histogramLabel->selectedMinMax(minVal,maxVal);
        histogramLabel->updateHistogram(histogram);
    }
    controller->setThreshold(scrollBarMax->value(),scrollBarMin->value());
}


void MyHistogram::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->hide();
}
