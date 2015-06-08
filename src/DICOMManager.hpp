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
    ImageType::Pointer zoom(ImageType::Pointer image, double factor, int posX, int posY);
    ImageType::Pointer enhanceContrast(ImageType::Pointer myITKImage);
    ImageType::Pointer extractRegion(ImageType::Pointer src ,int startX,int startY, int endX, int endY);
    ImageType::Pointer  getEdges(ImageType::Pointer  src,int threshold);
    ImageType::Pointer extractSelectedRegion(ImageType::Pointer src ,int startX,int startY);

private :

    ImageType::Pointer rescale(ImageType::Pointer myITKImage);




    std::vector<ImageType::Pointer> actualSerie;
    ImageType::Pointer loadDICOM(std::string filename);

    std::vector<ImageType::Pointer> getDICOMDirectory(std::string directoryName);








};


#endif // DICOMMANAGER_H
