#include "Controller.hpp"




Controller::Controller(){
    mDicom = new DICOMMManager();
    edge = zone = contrast = false;
    x1 = x2 = y1 = y2 = 0;
    threshold = 1;

}

void Controller::changeThreshold (int thresholdModifier){
    if(thresholdModifier + threshold > 0)
        threshold += thresholdModifier;
}

void Controller::refreshBool(bool edge, bool zone ,bool contrast, bool selectRegion){
    this->edge = edge;
    this->zone = zone;
    this->contrast = contrast;
    this->selectRegion = selectRegion;
}

void Controller::loadDICOMSerie(std::string seriesIdentifier,std::string directoryName){
    mDicom->loadDICOMSerie(seriesIdentifier,directoryName);
    zoomFactor = 1;
    posX = posY = 0;
}


std::vector<std::string> Controller::getSeries(std::string directoryName){
    return mDicom->getSeries(directoryName);

}

void Controller::refreshZoom(double zoomModifier, int posY, int posX){
    this->posX = posX;
    this->posY = posY;
    if(zoomFactor + zoomModifier> 1)
        zoomFactor += zoomModifier;
}

void Controller::refreshZone(int x1, int y1, int x2, int y2){
    this->x1 = x1;
    this->x2 = x2;
    this->y1 = y1;
    this->y2 = y2;

}

void Controller::resetZoom(){
    posX = 0;
    posY = 0;
    zoomFactor = 1;
}

QImage Controller::getDicom(int num,float ratio){

    QImage null;
    DICOMMManager::ImageType::Pointer mItkImage = mDicom->getImageFromSerie(num);
    if (mItkImage.IsNotNull()){
        if(zone)
            if(x1 >= 0 && y1 >= 0 && x2 >= 0 && y2 >= 0){
                int wight = mItkImage->GetLargestPossibleRegion().GetSize()[0];
                int height = mItkImage->GetLargestPossibleRegion().GetSize()[1];
                if(x1*ratio < wight && y1*ratio < height && x2*ratio < wight && y2*ratio < height)
                    mItkImage = mDicom->extractRegion(mItkImage,x1*ratio,y1*ratio,x2*ratio,y2*ratio);
            }
        if(selectRegion)
            if(x1 >= 0 && y1 >= 0){
                int wight = mItkImage->GetLargestPossibleRegion().GetSize()[0];
                int height = mItkImage->GetLargestPossibleRegion().GetSize()[1];
                if(x1*ratio < wight && y1*ratio < height)
                    mItkImage = mDicom->extractSelectedRegion(mItkImage,x1*ratio,y1*ratio);
            }
        if(contrast)
            mItkImage = mDicom->enhanceContrast(mItkImage);
        if(edge)
            mItkImage = mDicom->getEdges(mItkImage,threshold);
        return(mDicom->ITKImageToQImage(mItkImage));
    }
    return null;
}

