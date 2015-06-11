#ifndef MYWINDOW_HPP
#define MYWINDOW_HPP

#include <QApplication>
#include <QLabel>
#include <QFileDialog>
#include <QMenu>
#include <QMainWindow>
#include <QFileDialog>
#include <QWidget>
#include <QObject>
#include <QSplashScreen>
#include "MyLabel.hpp"
#include "Controller.hpp"


class MyWindow : public QMainWindow
{
    Q_OBJECT
public:
    MyWindow();
protected:
    int cpt;
    Controller *controller;
    std::vector<std::string> series;
    std::string actualDirectory;
    QLabel* myLabel;
    MyLabel* myDrawer;
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent * event);



QSize actualSize;
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
    QLabel *infoLabel;
    QTimer *m_timer;

private :
    void refreshImage(int cpt);
    void createActions();
    void createMenus();

private slots :
    void openDirectory();
    void openSerie(int);
    void refreshBool();
     void ShowContextMenu(const QPoint& pos);
};


#endif // MYWINDOW_HPP
