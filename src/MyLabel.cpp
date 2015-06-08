#include "MyLabel.hpp"
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

MyLabel::MyLabel(QWidget* parent)
{
    ratio = 1;
    isSelectable = true;
    show = false;
    isDrawing = false;
    this->setParent(parent);
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start(16);
    this->resize(10000,10000);
}
void MyLabel::setRatio(float ratio){
    int x1,y1,x2,y2;
    this->ratio = ratio;
    x1 = P1.x();
    y1 = P1.y();
    x2 = P2.x();
    y2 = P2.y();
    selectedRectangle.setCoords(x1/ratio,y1/ratio,x2/ratio,y2/ratio);
}

void MyLabel::paintEvent(QPaintEvent *event)
{
    if(isSelectable){
        if(isDrawing){
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setPen(Qt::black);
             P2 = this->mapFromGlobal(QCursor::pos())*ratio;
            selectedRectangle = QRect(P1, P2);
            setRatio(ratio);
            painter.drawRect(selectedRectangle);
        }
        else if(show){
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setPen(Qt::black);
            painter.drawRect(selectedRectangle);
        }
    }

}
void MyLabel::mousePressEvent(QMouseEvent *event){
    P1 = event->pos()*ratio;
    isDrawing = true;
}

void MyLabel::mouseReleaseEvent(QMouseEvent *event){
    P2 = event->pos()*ratio;
    isDrawing = false;
}