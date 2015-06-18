#ifndef MYHISTOGRAM_HPP
#define MYHISTOGRAM_HPP


#include <QMainWindow>
#include "Controller.hpp"
#include "MyScrollBar.h"
#include <QLabel>
#include "DGtal/base/Common.h"
#include "DGtal/io/Display3D.h"
#include "DGtal/io/viewers/Viewer3D.h"
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
    Controller *controller;
    MyScrollBar *scrollBarMin;
    MyScrollBar *scrollBarMax;
    QLabel *histogramLabel;
    QPainter *painter;
    QAction *Threshold;
    QAction *showThreshold;
    QTimer *m_timer;
    std::vector< std::vector<int> > histograms;
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);
private slots :
    void activateThreshold();
};

#endif // MYHISTOGRAM_HPP
