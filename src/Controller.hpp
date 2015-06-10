#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "DICOMManager.hpp"
#include <QImage>

using namespace std;


class Controller
{
 public :
    Controller();
    void refreshZoom(double zoomModifier, int posX,int posY);
    void changeThreshold (int newThreshold);
    void refreshBool(bool edge, bool zone ,bool contrast, bool selectRegion, bool selectBronchi);
    void loadDICOMSerie(string seriesIdentifier,string directoryName);
    void refreshZone(int x1, int y1, int x2, int y2);
    std::vector<std::string> getSeries(std::string directoryName);
    void refreshMousePos(int posX, int posY);
     void resetZoom();
    QImage getDicom(int num,float ratio);
 private :
     DICOMMManager *mDicom;
     int threshold;
     double zoomFactor;
     int posY;
     int posX;
     bool edge;
     bool zone;
     bool contrast;
     bool selectRegion;
     bool selectBronchi;
     int x1,x2,y1,y2;
     int mouseX;
     int mouseY;

};





#endif // CONTROLLER_HPP
