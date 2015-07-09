#ifndef MYHISTOGRAMLABEL_HPP
#define MYHISTOGRAMLABEL_HPP

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
    void selectedMinMax(int min, int max);
private :
    int min;
    int max;
    void paintEvent(QPaintEvent *event);
    QPainter *painter;
    int base;
    float heightRatio;
    int widthRatio;
    std::vector<int> histogram;
};

#endif // MYHISTOGRAMLABEL_HPP
