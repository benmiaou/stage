#ifndef MYHISTOGRAMLABEL_H
#define MYHISTOGRAMLABEL_H

#include <QApplication>
#include <QLabel>
#include <QMenu>
#include <QMainWindow>
#include <QFileDialog>
#include <QWidget>
#include <QObject>

class MyHistogramLabel : public QLabel
{
public:
    MyHistogramLabel(QWidget *window);
    void updateHistogram(std::vector<int> histogram);
private :
    void paintEvent(QPaintEvent *event);
    QPainter *painter;
    int max;
    int base;
    float heightRatio;
    int widthRatio;
    std::vector<int> histogram;
};

#endif // MYHISTOGRAMLABEL_H
