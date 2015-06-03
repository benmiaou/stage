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

class MyWindow : public QMainWindow
{
    Q_OBJECT
public:
    MyWindow();
protected:
    int cpt;
    double zoomFactor;
    QLabel* myLabel;
    std::vector<QImage> QtImages;
std::vector<std::string> series;
std::string actualDirectory;
    void wheelEvent(QWheelEvent *event);



//void contextMenuEvent(QContextMenuEvent *event);
QActionGroup *alignmentGroup;
      QMenu *fileMenu;
       QAction *newAct;
    QLabel *infoLabel;

private :
    void createActions();
    void createMenus();
private slots :
    void openDirectory();
    void openSerie(int);
};


#endif // MYWINDOW_HPP
