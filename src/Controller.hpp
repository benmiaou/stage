#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "DICOMManager.hpp"
#include <QImage>

using namespace std;


class Controller
{
 public :
    Controller();
    void refreshZoom(double zoomModifier, int posY,int posX);
    void changeThreshold (int newThreshold);
    void refreshBool(bool edge, bool zone ,bool contrast, bool selectRegion);
    void loadDICOMSerie(string seriesIdentifier,string directoryName);
    void refreshZone(int x1, int y1, int x2, int y2);
    std::vector<std::string> getSeries(std::string directoryName);
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
     int x1,x2,y1,y2;

};





#endif // CONTROLLER_HPP
