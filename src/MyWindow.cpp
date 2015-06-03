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

#include <QStatusBar>


#include "MyWindow.hpp"
#include "DICOMManager.h"

MyWindow::MyWindow()
{
    createActions();
    createMenus();
 QFileDialog dialog(this);
    myLabel = new QLabel(this);
    this->resize(800,800);
   }

void MyWindow::createActions()
 {
    newAct = new QAction(tr("&Open Directory"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Open a DICOM directory"));
    connect(newAct, SIGNAL(triggered()),this ,SLOT(openDirectory()));

}

void MyWindow::openDirectory(){
    QString dir =  QFileDialog::getExistingDirectory();
     actualDirectory = dir.toStdString();
 series = getSeries(actualDirectory);
 QMenu* seriesMenu = this->menuBar()->addMenu(tr("&Series"));

 QSignalMapper* signalMapper = new QSignalMapper (this);
 for (int i =0; i< series.size(); i++){
     QAction* newAction = new QAction(tr(series[i].c_str()), this);

     newAction->setShortcuts(QKeySequence::New);
     newAction->setStatusTip(tr("Open a DICOM serie"));
     connect(newAction, SIGNAL(triggered()),signalMapper ,SLOT(map()));
     signalMapper ->setMapping(newAction,i);
     seriesMenu->addAction(newAction);
 }
 connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(openSerie(int))) ;
}


void MyWindow::openSerie(int i){
    QtImages = getDICOMSerie(series[i],actualDirectory);
    cpt = 0;
    QImage image_Qt = QtImages[0];
    myLabel->setPixmap(QPixmap::fromImage(image_Qt));
    myLabel->setFixedSize(image_Qt.width(),image_Qt.height());
    this->resize(image_Qt.width(),image_Qt.height());

}

void MyWindow::createMenus()
{
     fileMenu = this->menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);

}


void MyWindow::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta() / 8;
    if(cpt+1 >= QtImages.size())
        cpt = 0;
    else
        if (delta > 0) {
            cpt++;
        } else {
            if(cpt > 0)
                cpt--;
        }
    QImage image_Qt = QtImages[cpt];
    myLabel->setFixedSize(image_Qt.width(),image_Qt.height());
    myLabel->setPixmap(QPixmap::fromImage(image_Qt));
    this->resize(image_Qt.width(),image_Qt.height());
    event->accept();
}
