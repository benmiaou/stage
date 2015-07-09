#include "My3Dviewer.h"
#include <QImage>
#include <QColor>
#include <QLayout>

My3Dviewer::My3Dviewer(QWidget *parent)
{
    viewer = new Viewer3D<>();
    viewer->setParent(this);
    viewer->update();
    viewer->show();
    this->hide();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(viewer);
    this->setLayout(layout);
}





void My3Dviewer::creatScene(Controller *controller)
{
   // std::cout << " TEST " << std::endl;
    viewer->clear();
    this->show();
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
                if(qGray(color) > 150){
                    Point p = Point(i,j,num);
                    *viewer << p;
                    *viewer <<  CustomColors3D( Color(qGray(color),num%255,(i+j)%255), Color(qGray(color),num%255,(i+j)%255) );
                }
            }
        num++;

        image_Qt = controller->getDicom(num,1);
    }
    *viewer<< Viewer3D<>::updateDisplay;


}
