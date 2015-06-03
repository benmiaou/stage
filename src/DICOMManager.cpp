#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkImageSeriesReader.h"
#include "itkImage.h"
#include "itkImageRegionConstIterator.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
#include "DICOMManager.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"


#include "itkImage.h"
#include "itkImageFileWriter.h"





#include <QImage>

typedef signed short    PixelType;
const unsigned int      Dimension = 2;
typedef itk::Image< PixelType, Dimension >ImageType;

template<typename TImage, typename TLabelImage>
static void SummarizeLabelStatistics (TImage* image,
                                      TLabelImage* labelImage);



ImageType::Pointer loadDICOM(std::string filename){



    typedef itk::Image< PixelType, Dimension >ImageType;
    typedef itk::ImageSeriesReader< ImageType >        ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    typedef itk::GDCMImageIO       ImageIOType;
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    reader->SetImageIO( dicomIO );
    reader->SetFileName(filename);
    reader->Update();
    return reader->GetOutput();
}

ImageType::Pointer enhanceLungContrast(ImageType::Pointer myITKImage){
    typedef itk::RescaleIntensityImageFilter< ImageType, ImageType >
            RescaleFilterType;
    RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
    rescaleFilter->SetInput(myITKImage);
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();
    return rescaleFilter->GetOutput();

}

QImage ITKImageToQImage(ImageType::Pointer myITKImage){
    int wight = myITKImage->GetLargestPossibleRegion().GetSize()[0];
    int height = myITKImage->GetLargestPossibleRegion().GetSize()[1];

    QImage *image_Qt = new QImage (wight,height,QImage::Format_RGB32);
    for(unsigned int i = 0; i < wight; i++)
    {
        for(unsigned int j = 0; j < height; j++)
        {
            ImageType::IndexType pixelIndex;
            pixelIndex[0] = i;
            pixelIndex[1] = j;
            int pixelValue = myITKImage->GetPixel(pixelIndex);
            QRgb value = qRgb(pixelValue,pixelValue,pixelValue);
            image_Qt->setPixel(i,j, value );
        }

    }
    return *image_Qt;
}

QImage getDICOM(std::string filename){
    ImageType::Pointer myITKImage = loadDICOM(filename);
    myITKImage = enhanceLungContrast(myITKImage);
    return ITKImageToQImage(myITKImage);
}


std::vector<std::string> getSeries(std::string directoryName){
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetUseSeriesDetails( true );
    nameGenerator->SetDirectory(directoryName);
    typedef std::vector< std::string >    SeriesIdContainer;
    const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
    SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
    SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();

    std::vector<std::string> series;
    while( seriesItr != seriesEnd )
    {
        series.push_back(seriesItr->c_str());
        seriesItr++;
    }
    return series;
}
std::vector<QImage> getDICOMSerie(std::string seriesIdentifier,std::string directoryName){
    typedef itk::GDCMImageIO       ImageIOType;
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    typedef itk::ImageSeriesReader< ImageType >        ReaderType;
    ReaderType::Pointer reader = ReaderType::New();


    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetUseSeriesDetails( true );
    nameGenerator->SetDirectory(directoryName);
/*
    typedef std::vector< std::string >    SeriesIdContainer;
    const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
    SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
    SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
    std::string seriesIdentifier;
    typedef std::vector< std::string >   FileNamesContainer;
    FileNamesContainer fileNames;
    reader->SetImageIO( dicomIO );
    std::vector<QImage> images_Qt;

        seriesIdentifier = seriesItr->c_str();*/
    std::vector<QImage> images_Qt;
    typedef std::vector< std::string >   FileNamesContainer;
    FileNamesContainer fileNames;
        fileNames = nameGenerator->GetFileNames(seriesIdentifier);
        for (int i =0; i< fileNames.size(); i++){
            std::cout << fileNames[i] << std::endl;
            reader->SetFileName(fileNames[i]);
            reader->UpdateLargestPossibleRegion();
            reader->Update();
            ImageType::Pointer myITKImage = reader->GetOutput();
            myITKImage = enhanceLungContrast(myITKImage);
            QImage image_Qt =  ITKImageToQImage(myITKImage);
            images_Qt.push_back(image_Qt);
        }
    return images_Qt;
}



std::vector<QImage> getDICOMDirectory(std::string directoryName){
    typedef itk::GDCMImageIO       ImageIOType;
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    typedef itk::ImageSeriesReader< ImageType >        ReaderType;
    ReaderType::Pointer reader = ReaderType::New();


    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetUseSeriesDetails( true );
    nameGenerator->SetDirectory(directoryName);

    typedef std::vector< std::string >    SeriesIdContainer;
    const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
    SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
    SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
    std::string seriesIdentifier;
    typedef std::vector< std::string >   FileNamesContainer;
    FileNamesContainer fileNames;
    reader->SetImageIO( dicomIO );
    std::vector<QImage> images_Qt;
    while( seriesItr != seriesEnd )
    {
        seriesIdentifier = seriesItr->c_str();
        fileNames = nameGenerator->GetFileNames(seriesIdentifier);
        for (int i =0; i< fileNames.size(); i++){
            std::cout << fileNames[i] << std::endl;
            reader->SetFileName(fileNames[i]);
            reader->UpdateLargestPossibleRegion();
            reader->Update();
            ImageType::Pointer myITKImage = reader->GetOutput();
            myITKImage = enhanceLungContrast(myITKImage);
            QImage image_Qt =  ITKImageToQImage(myITKImage);
            images_Qt.push_back(image_Qt);
        }
        seriesItr++;
    }
    return images_Qt;
}
