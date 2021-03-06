#ifndef MYWINDOW_HPP
#define MYWINDOW_HPP


#include <QObject>
#include <QSplashScreen>
#include <QActionGroup>
#include <QMenuBar>


#include "MyLabel.hpp"
#include "MyHistogram.hpp"
#include "Controller.hpp"
#include "My3Dviewer.h"

class MyWindow : public QMainWindow
{
    Q_OBJECT
public:
    MyWindow(Controller *controller);
    void refreshImage(int cpt);
protected:
    int cpt;
    Controller *controller;
    My3Dviewer *viewer3D;
    std::vector<std::string> series;
    std::string actualDirectory;
    QLabel* myLabel;
    MyLabel* myDrawer;
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent * event);

    QSize actualSize;
    MyHistogram *histogram;
    QSplashScreen *splash;
    QActionGroup *alignmentGroup;
    QMenuBar *menuBar;
    QMenu *fileMenu;
    QMenu *seriesMenu;
    QMenu *processMenu;
    QAction *newAct;
    QAction *edgesAct;
    QAction *zoneAct;
    QAction *contrastAct;
    QAction *selectRegion;
    QAction *selectBronchi;
    QAction *histogramAct;
    QAction *view3DAct;

private :

    void createActions();
    void createMenus();

private slots :
    void view3D();
    void openDirectory();
    void openSerie(int);
    void refreshBool();
    void ShowContextMenu(const QPoint& pos);
    void showHistogram();
};


#endif // MYWINDOW_HPP
