#ifndef MYHISTOGRAM_HPP
#define MYHISTOGRAM_HPP

#include <QApplication>
#include <QLabel>
#include <QMenu>
#include <QMainWindow>
#include <QFileDialog>
#include <QWidget>
#include <QObject>
#include <QScrollBar>
#include "Controller.hpp"
#include "MyHistogramLabel.h"

class MyHistogram : public QWidget
{
public:
    MyHistogram(Controller *controller);
    void Histogram();
protected:
    Controller *controller;
    QScrollBar *scrollBarMin;
    QScrollBar *scrollBarMax;
    MyHistogramLabel *histogramLabel;
    QPainter *painter;
    QTimer *m_timer;
    void paintEvent(QPaintEvent *event);
};

#endif // MYHISTOGRAM_HPP
