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

#include "MyWindow.hpp"

#include "MyLabel.hpp"
MyWindow::MyWindow()
{
    controller = new Controller();
    this->resize(800,800);
    QPixmap pixmap("./loading1.gif");
    splash = new QSplashScreen(pixmap,0);
    createActions();
    createMenus();
    myLabel = new QLabel(this);
    myLabel->setGeometry(0,0,800,800);
    myDrawer = new MyLabel(this);
    QColor bg_color(255, 0, 0, 0);
    QPalette p(myDrawer->palette());
    p.setColor(QPalette::Background, bg_color);
    myDrawer->setPalette(p);
}

void MyWindow::createActions()
{
    newAct = new QAction(tr("&Open Directory"), this);
    newAct->setStatusTip(tr("Open a DICOM directory"));
    connect(newAct, SIGNAL(triggered()),this ,SLOT(openDirectory()));

    edgesAct = new QAction(tr("&Show Edges"), this);
    edgesAct->setStatusTip(tr("Show Edges"));
    edgesAct->setCheckable(true);
    connect(edgesAct, SIGNAL(triggered()),this ,SLOT(refreshBool()));

    zoneAct = new QAction(tr("&Select Zone"), this);
    zoneAct->setStatusTip(tr("Select Zone"));
    zoneAct->setCheckable(true);
    connect(zoneAct, SIGNAL(triggered()),this ,SLOT(refreshBool()));

    contrastAct = new QAction(tr("&Enhance Contrast"), this);
    contrastAct->setStatusTip(tr("Enhance Contrast"));
    contrastAct->setCheckable(true);
    connect(contrastAct, SIGNAL(triggered()),this ,SLOT(refreshBool()));

    selectRegion = new QAction(tr("&Enhance Region"), this);
    selectRegion->setStatusTip(tr("Enhance Region"));
    selectRegion->setCheckable(true);
    connect(selectRegion, SIGNAL(triggered()),this ,SLOT(refreshBool()));
}
void MyWindow::refreshBool (){
    controller->refreshBool(edgesAct->isChecked(),zoneAct->isChecked(),contrastAct->isChecked(),selectRegion->isChecked());
    refreshImage(cpt);
}

void MyWindow::resizeEvent (QResizeEvent * event){
    refreshImage(cpt);
      if(myLabel->pixmap() != NULL){
        float ratio = (float)actualSize.height()/myLabel->pixmap()->height();
            myDrawer->setRatio(ratio);
    }
}

void MyWindow::createMenus()
{
    fileMenu = this->menuBar()->addMenu(tr("&File"));
    seriesMenu = this->menuBar()->addMenu(tr("&Series"));
    processMenu = this->menuBar()->addMenu(tr("&Processing"));
    fileMenu->addAction(newAct);
    processMenu->addAction(edgesAct);
    processMenu->addAction(zoneAct);
    processMenu->addAction(contrastAct);
    processMenu->addAction(selectRegion);
}

void MyWindow::openDirectory(){
    QString dir =  QFileDialog::getExistingDirectory();
    actualDirectory = dir.toStdString();
    series = controller->getSeries(actualDirectory);
    seriesMenu->clear();
    QSignalMapper* signalMapper = new QSignalMapper (this);
    for (int i =0; i< series.size(); i++){
        QAction* newAction = new QAction(tr(series[i].c_str()), this);
        newAction->setStatusTip(tr("Open a DICOM serie"));
        connect(newAction, SIGNAL(triggered()),signalMapper ,SLOT(map()));
        signalMapper ->setMapping(newAction,i);
        seriesMenu->addAction(newAction);
    }
    connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(openSerie(int))) ;
}



void MyWindow::openSerie(int i){    
    splash->show();
    splash->showMessage("LOADING",Qt::AlignBottom,Qt::white);
    controller->loadDICOMSerie(series[i],actualDirectory);
    myDrawer->show = true;
    splash->finish(this);
    myDrawer->isSelectable = true;
    zoneAct->setChecked(false);
    controller->refreshBool(edgesAct->isChecked(),zoneAct->isChecked(),contrastAct->isChecked(),selectRegion->isChecked());
    QImage image_Qt = controller->getDicom(0,1);
    actualSize = image_Qt.size();
    controller->resetZoom();
    QPixmap p = QPixmap::fromImage(image_Qt);
    myLabel->setPixmap(p.scaled(this->width(),this->height(),Qt::KeepAspectRatio));
    myLabel->adjustSize();
    this->resize(myLabel->size());
}


void MyWindow::refreshImage(int num){
    if(myLabel->pixmap() != NULL){
        QImage image_Qt = myLabel->pixmap()->toImage();
        if (!image_Qt.isNull()){
            myDrawer->show = !zoneAct->isChecked();
            myDrawer->isSelectable = !zoneAct->isChecked();

            float ratio = (float)actualSize.height()/myLabel->pixmap()->height();
            std::cout << "ratio : "<< ratio << std::endl;
            int x1,y1,x2,y2;
            myDrawer->selectedRectangle.getCoords(&x1,&y1,&x2,&y2);
            controller->refreshZone(x1,y1,x2,y2);
            image_Qt = controller->getDicom(num,ratio);
            if (!image_Qt.isNull()){
                QPixmap p = QPixmap::fromImage(image_Qt);
                myLabel->setPixmap(p.scaled(this->width(),this->height(),Qt::KeepAspectRatio));
                myLabel->resize(myLabel->pixmap()->width(),myLabel->pixmap()->height());
            }
            else
                cpt =0;
        }
    }
}

void MyWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        QApplication::quit();
    }
    if(event->key() == Qt::Key_Minus)
    {
        controller->changeThreshold(-1);
    }
    if(event->key() == Qt::Key_Plus)
    {
        controller->changeThreshold(+1);
    }
    refreshImage(cpt);

}

void MyWindow::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers().testFlag(Qt::ControlModifier)){
        double zoomModifier = 0;
        int delta = event->delta() / 8;
        if (delta > 0) {
            zoomModifier = 0.1;
        } else {
            zoomModifier = -0.1;
        }

        int posX = event->x();
        int posY = event->y();
        controller->refreshZoom(zoomModifier,posX,posY);
        refreshImage(cpt);
        event->accept();
    }
    else{
        int delta = event->delta() / 8;
        if (delta > 0) {
            cpt++;
        } else {
            if(cpt > 0)
                cpt--;
        }
        controller->resetZoom();
        refreshImage(cpt);
        event->accept();
    }
}
