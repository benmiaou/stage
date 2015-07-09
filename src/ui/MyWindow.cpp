#include <QMenuBar>
#include <QSignalMapper>
#include <QKeyEvent>
#include <QFileDialog>
#include <QApplication>
#include <QSignalMapper>


#include "MyWindow.hpp"





MyWindow::MyWindow(Controller *controller)
{
    this->setFocusPolicy(Qt::StrongFocus);
    this->controller = controller;  
    myLabel = new QLabel(this);
    myDrawer = new MyLabel(this);
    viewer3D = new My3Dviewer(this);
    menuBar = new QMenuBar();
    menuBar->setNativeMenuBar(false);
    controller->setMenuBar(menuBar);
    histogram = new MyHistogram(controller);
    controller->resize(800,800);
    QPixmap pixmap("./loading1.gif");
    splash = new QSplashScreen(pixmap,0);
    createActions();
    createMenus();
    myLabel->setGeometry(0,0,800,800);
    myDrawer->setGeometry(0,0,800,800);
    QColor bg_color(255, 0, 0, 0);
    QPalette p(myDrawer->palette());
    p.setColor(QPalette::Background, bg_color);
    myDrawer->setPalette(p);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(ShowContextMenu(const QPoint&)));

}



void MyWindow::showHistogram(){
    if(myLabel->pixmap() != NULL){
        QImage image_Qt = myLabel->pixmap()->toImage();
        if (!image_Qt.isNull()){
            histogram->show();
            histogram->update();

        }
    }
}

void MyWindow::view3D(){
    if(myLabel->pixmap() != NULL){
        QImage image_Qt = myLabel->pixmap()->toImage();
        if (!image_Qt.isNull()){
            viewer3D->creatScene(controller);
        }
    }
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
    contrastAct->setStatusTip(tr("enhance global contrast"));
    contrastAct->setCheckable(true);
    connect(contrastAct, SIGNAL(triggered()),this ,SLOT(refreshBool()));

    selectRegion = new QAction(tr("&Enhance Region"), this);
    selectRegion->setStatusTip(tr("enhance contrast in region"));
    selectRegion->setCheckable(true);
    connect(selectRegion, SIGNAL(triggered()),this ,SLOT(refreshBool()));

    selectBronchi = new QAction(tr("&selectBronchi"), this);
    selectBronchi->setStatusTip(tr("show bronchi in region"));
    selectBronchi->setCheckable(true);
    connect(selectBronchi, SIGNAL(triggered()),this ,SLOT(refreshBool()));

    histogramAct = new QAction(tr("&Show Histogram"), this);
    histogramAct->setStatusTip(tr("Show Histogram"));
    connect(histogramAct, SIGNAL(triggered()),this ,SLOT(showHistogram()));

    view3DAct = new QAction(tr("&Show 3D View"), this);
    view3DAct->setStatusTip(tr("Show 3D View"));
    connect(view3DAct, SIGNAL(triggered()),this ,SLOT(view3D()));
}
void MyWindow::refreshBool (){
    controller->refreshBool(edgesAct->isChecked(),zoneAct->isChecked(),contrastAct->isChecked(),selectRegion->isChecked(),selectBronchi->isChecked());
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
    fileMenu = menuBar->addMenu(tr("&File"));
    seriesMenu = menuBar->addMenu(tr("&Series"));
    processMenu = menuBar->addMenu(tr("&Processing"));
    fileMenu->addAction(newAct);
    processMenu->addAction(edgesAct);
    processMenu->addAction(contrastAct);
    processMenu->addAction(histogramAct);
    processMenu->addAction(view3DAct);
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
    refreshBool ();
    QImage image_Qt = controller->getDicom(0,1);
    actualSize = image_Qt.size();
    controller->resetZoom();
    QPixmap p = QPixmap::fromImage(image_Qt);
    myLabel->setPixmap(p.scaled(this->width(),this->height(),Qt::KeepAspectRatio));
    myLabel->adjustSize();
    this->resize(myLabel->size());
    histogram->updateHistogram();
}


void MyWindow::refreshImage(int num){
    if(myLabel->pixmap() != NULL){
        QImage image_Qt = myLabel->pixmap()->toImage();
        if (!image_Qt.isNull()){

            float ratio = 1;
            myDrawer->show = !zoneAct->isChecked();
            myDrawer->isSelectable = !zoneAct->isChecked();
            if(myLabel->pixmap()->height() > myLabel->pixmap()->width())
                ratio = (float)actualSize.height()/myLabel->pixmap()->height();
            else
                ratio = (float)actualSize.width()/myLabel->pixmap()->width();
            int x1,y1,x2,y2;
            myDrawer->selectedRectangle.getCoords(&x1,&y1,&x2,&y2);
            controller->refreshZone(x1,y1,x2,y2);
            image_Qt = controller->getDicom(num,ratio);
            if (!image_Qt.isNull()){
                QPixmap p = QPixmap::fromImage(image_Qt);
                myLabel->setPixmap(p.scaled(this->width(),this->height(),Qt::KeepAspectRatio));
                myLabel->resize(myLabel->pixmap()->width(),myLabel->pixmap()->height());
                myDrawer->resize(myLabel->pixmap()->width(),myLabel->pixmap()->height());

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
    if(event->key() == Qt::Key_Up)
    {
        cpt++;
    }
    if(event->key() == Qt::Key_Down)
    {
        if(cpt > 0)
            cpt--;
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

void MyWindow::ShowContextMenu(const QPoint& pos)
{
    QPoint globalPos = this->mapToGlobal(pos);
    std::cout << myLabel->mapFromGlobal(globalPos).x() << myLabel->mapFromGlobal(globalPos).y() << std::endl;
    controller->refreshMousePos(myLabel->mapFromGlobal(globalPos).x(),myLabel->mapFromGlobal(globalPos).y());

    QMenu myMenu;
    myMenu.addAction(selectRegion);
    myMenu.addAction(zoneAct);
    myMenu.addAction(selectBronchi);

    QAction* selectedItem = myMenu.exec(globalPos);
    this->menuBar->update();
    if (selectedItem)
    {

    }
    else
    {

    }
}


