#ifndef MYHISTOGRAM_HPP
#define MYHISTOGRAM_HPP



#include "Controller.hpp"
#include "MyScrollBar.h"
#include "DGtal/io/viewers/Viewer3D.h"
#include "MyHistogramLabel.hpp"

using namespace DGtal;
using namespace Z3i;


class MyHistogram : public QMainWindow
{
    Q_OBJECT
public:
    MyHistogram(Controller *controller);
    void updateHistogram();
protected:
   int  actualUpper;
    int actualLower;
    Viewer3D<> *viewer;
    MyHistogramLabel *histogramLabel;
    Controller *controller;
    MyScrollBar *scrollBarMin;
    MyScrollBar *scrollBarMax;
    QPainter *painter;
    QAction *lungSegmentation;
    QAction *applyLungSeg;
    QAction *simpleThreshold;
    QAction *applySimpleThresh;
    QAction *histogramType1;
    QAction *histogramType2;
    QAction *histogramType3;
    QTimer *m_timer;
    std::vector< std::vector<int> > histograms;
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);
    void update3D();
private slots :
    void activateLungSegmentation();
    void applyLungSegmentation();
    void activateSimpleThreshold();
    void applySimpleThreshold();
};

#endif // MYHISTOGRAM_HPP
