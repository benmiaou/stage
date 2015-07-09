#ifndef DICOMMANAGER_H
#define DICOMMANAGER_H

#include <QImage>
#include "itkImage.h"



class DICOMMManager
{
public :
    typedef signed short    PixelType;
    typedef itk::Image< PixelType, 2 >ImageType;
    DICOMMManager();

    std::vector<std::string> getSeries(std::string directoryName);



    void loadDICOMSerie(std::string seriesIdentifier,std::string directoryName);
    QImage ITKImageToQImage(ImageType::Pointer myITKImage);

    ImageType::Pointer getImageFromSerie(int num);
    std::vector<int> getHistogram(int num);
    ImageType::Pointer enhanceContrast(ImageType::Pointer myITKImage);
    ImageType::Pointer extractRegion(ImageType::Pointer src ,int startX,int startY, int endX, int endY);
    ImageType::Pointer  getEdges(ImageType::Pointer  src,int threshold);
    ImageType::Pointer enhanceSelectedRegion(ImageType::Pointer src ,int seedPosX,int seedPosY);
    ImageType::Pointer extractBronchiInRegion(ImageType::Pointer src ,int seedPosX,int seedPosY);
    ImageType::Pointer threshold(ImageType::Pointer src, int upperThreshold, int lowerThreshold);

private :
    ImageType::Pointer  getConvexHull(ImageType::Pointer src);
    ImageType::Pointer  convexHull(ImageType::Pointer src);
    ImageType::Pointer fileHoleInBinary(ImageType::Pointer src);
    ImageType::Pointer  removeBackground(ImageType::Pointer src);
    ImageType::Pointer smoothImage(ImageType::Pointer src);
    ImageType::Pointer rescale(ImageType::Pointer myITKImage);
    ImageType::Pointer zoom(ImageType::Pointer image, double factor, int posX, int posY);
    ImageType::Pointer lungSegementation(ImageType::Pointer src);
    ImageType::Pointer extractSelectedRegion(ImageType::Pointer src ,int startX,int startY);
    ImageType::Pointer loadDICOM(std::string filename);
    std::vector<ImageType::Pointer> getDICOMDirectory(std::string directoryName);

    std::vector<ImageType::Pointer> actualSerie;








};


#endif // DICOMMANAGER_H
