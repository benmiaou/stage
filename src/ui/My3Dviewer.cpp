#include "My3Dviewer.h"
#include <QImage>
#include <QColor>

My3Dviewer::My3Dviewer(QWidget *parent)
{
    this->show();
    viewer = new Viewer3D<>();
    viewer->setParent(this);
     viewer->update();
    viewer->show();
    //this->setParent(parent);
}


void My3Dviewer::creatScene(Controller *controller)
{
   // std::cout << " TEST " << std::endl;
     viewer->update();
    viewer->clear();
    viewer->setSizeIncrement(0,0);
    Point p1( 0, 0, 0 );
    Point p2( 10, 10 , 10 );
    Domain domain( p1, p2 );
    *viewer << domain;
    int num = 75;
    QImage image_Qt = controller->getDicom(num,1);
    while(!image_Qt.isNull() && num < 450){
        std::cout << "3D Load "<< num << std::endl;
        int wight = image_Qt.width();
        int height = image_Qt.height();

        for(unsigned int i = 0; i < wight; i++)
            for(unsigned int j = 0; j < height; j++)
            {

                QRgb color = image_Qt.pixel(i,j);
                if(qGray(color) > 220){
                    Point p = Point(i,j,num);
                    *viewer << p;
                    *viewer <<  CustomColors3D( Color(qGray(color),num%255,(i+j)%255), Color(qGray(color),num%255,(i+j)%255) );
                }
            }
        num++;

        image_Qt = controller->getDicom(num,1);
    }
    *viewer<< Viewer3D<>::updateDisplay;
    viewer->update();
    viewer->show();
}
