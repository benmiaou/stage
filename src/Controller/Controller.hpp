#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP


#include "DICOMManager.hpp"
#include <QMainWindow>


using namespace std;


class Controller : public QMainWindow
{
 public :
    Controller();
    bool zoneEdge;
    void refreshZoom(double zoomModifier, int posX,int posY);
    void changeThreshold (int newThreshold);
    void refreshBool(bool edge, bool zone ,bool contrast, bool selectRegion, bool selectBronchi);
    void loadDICOMSerie(string seriesIdentifier,string directoryName);
    void refreshZone(int x1, int y1, int x2, int y2);
    std::vector<std::string> getSeries(std::string directoryName);
    std::vector<int> getHistogram();
    void refreshMousePos(int posX, int posY);
    void resetZoom();
    void setThreshold(int max, int min);
    bool isNeedingRefresh();
    void activeLungSegmentation(bool isActive);
    void activeSimpleThreshold(bool isActive);
    QImage getDicom(int num,float ratio);
    std::vector< std::vector<int> > getHistograms();
    int getCurrent();
    void applyLungSegmentation();
    void applySimpleThreshold();
    void getThresholdValues(int &max, int &min);

 protected :

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
     bool isActiveLungSegmentation;
     bool isActiveSimpleThreshold;
     int x1,x2,y1,y2;
     int mouseX;
     int mouseY;
     int lowerThreshold;
     int upperThreshold;
     int numActualImage;
     bool needRefresh;
};





#endif // CONTROLLER_HPP
