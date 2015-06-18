#ifndef MYLABEL_HPP
#define MYLABEL_HPP

#include <QLabel>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>


class MyLabel : public QLabel
{
public:
    MyLabel(QWidget* parent);
    QRect selectedRectangle;
    bool show;
    bool isSelectable;
    void setRatio(float ratio);
protected:
    float ratio;
    bool isDrawing;
    QPoint P1;
    QPoint P2;


    QTimer *m_timer;
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

};



#endif // MYLABEL_HPP
