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
#include "itkRegionOfInterestImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"


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

ImageType::Pointer QTimageToITKImage(QImage image){
    ImageType::Pointer myITKImage = ImageType::New();
    int wight = image.width();
    int height = image.height();
    const ImageType::SizeType  size  = {{wight, height}};
    const ImageType::IndexType start = {{ 0, 0}};
    ImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);

    myITKImage->SetRegions( region );
    myITKImage->Allocate(true);


    for(unsigned int i = 0; i < wight; i++)
    {
        for(unsigned int j = 0; j < height; j++)
        {
            ImageType::IndexType pixelIndex;
            pixelIndex[0] = i;
            pixelIndex[1] = j;
            QRgb value = image.pixel(i,j);
            int pixelValue = qRed(value);
            myITKImage->SetPixel(pixelIndex,pixelValue);
        }
    }
    return myITKImage;
}

ImageType::Pointer extractRegion(ImageType::Pointer src ,int startX,
                                 int startY, int endX, int endY){
    const itk::IndexValueType startx =
            static_cast< itk::IndexValueType >(startX);
    const itk::IndexValueType endx =
            static_cast< itk::IndexValueType >(endX);
    const itk::IndexValueType starty =
            static_cast< itk::IndexValueType >(startY);
    const itk::IndexValueType endy =
            static_cast< itk::IndexValueType >(endY);

    ImageType::IndexType start;
    start[0] = startx;
    start[1] = starty;

    ImageType::IndexType end;
    end[0] = endx;
    end[1] = endy;

    ImageType::RegionType region;
    region.SetIndex(start);
    region.SetUpperIndex(end);
    typedef itk::RegionOfInterestImageFilter< ImageType, ImageType >
            FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(src);
    filter->SetRegionOfInterest(region);
    filter->UpdateLargestPossibleRegion();
    filter->Update();
    return filter->GetOutput();
}


QImage zoom(QImage image, double factor, int posX, int posY){

    ImageType::Pointer myITKImage = QTimageToITKImage(image);
    int wight = myITKImage->GetLargestPossibleRegion().GetSize()[0];
    int height = myITKImage->GetLargestPossibleRegion().GetSize()[1];
    int hWight = wight/2;
    int hHeight = height/2;
    typedef unsigned char T_InputPixel;
    typedef itk::IdentityTransform<double, 2>
            T_Transform;
    typedef itk::BSplineInterpolateImageFunction<ImageType, double, double>
            T_Interpolator;
    typedef itk::ResampleImageFilter<ImageType, ImageType>
            T_ResampleFilter;
    T_Transform::Pointer _pTransform = T_Transform::New();
    _pTransform->SetIdentity();
    T_Interpolator::Pointer _pInterpolator = T_Interpolator::New();
    _pInterpolator->SetSplineOrder(3);
    T_ResampleFilter::Pointer _pResizeFilter = T_ResampleFilter::New();
    _pResizeFilter->SetTransform(_pTransform);
    _pResizeFilter->SetInterpolator(_pInterpolator);
    const double vfOutputOrigin[2]  = {posX/2, posY/2};
    _pResizeFilter->SetOutputOrigin(vfOutputOrigin);
    unsigned int nNewWidth = (int)(wight*factor);
    unsigned int nNewHeight =(int)(height*factor);
    const ImageType::RegionType& inputRegion =
            myITKImage->GetLargestPossibleRegion();
    const ImageType::SizeType& vnInputSize = inputRegion.GetSize();
    unsigned int nOldWidth = vnInputSize[0];
    unsigned int nOldHeight = vnInputSize[1];
    const ImageType::SpacingType& vfInputSpacing =
            myITKImage->GetSpacing();


    double vfOutputSpacing[2];
    vfOutputSpacing[0] = vfInputSpacing[0] * (double) nOldWidth / (double) nNewWidth;
    vfOutputSpacing[1] = vfInputSpacing[1] * (double) nOldHeight / (double) nNewHeight;
    _pResizeFilter->SetOutputSpacing(vfOutputSpacing);
    itk::Size<2> vnOutputSize = { {nNewWidth, nNewHeight} };
    _pResizeFilter->SetSize(vnOutputSize);
    _pResizeFilter->SetInput(myITKImage);
     _pResizeFilter->UpdateLargestPossibleRegion();
    _pResizeFilter->Update();
    myITKImage = _pResizeFilter->GetOutput();
    myITKImage->Update();
    myITKImage = extractRegion(myITKImage ,0,0, wight-1, height-1);
    myITKImage->Update();
    return ITKImageToQImage(myITKImage);
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


QImage getDICOM(std::string filename){
    ImageType::Pointer myITKImage = loadDICOM(filename);
    myITKImage = enhanceLungContrast(myITKImage);
    return ITKImageToQImage(myITKImage);
}


std::vector<std::string> getSeries(std::string directoryName){
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    typedef itk::ImageFileReader< ImageType >     ReaderType;
    typedef std::vector< std::string >    SeriesIdContainer;

    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetUseSeriesDetails( true );
    nameGenerator->SetDirectory(directoryName);
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
std::vector<QImage> getDICOMSerie(std::string seriesIdentifier,
                                  std::string directoryName){

    typedef itk::ImageSeriesReader< ImageType >        ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

    //----------------------------------------------------
    typedef itk::MetaDataDictionary   DictionaryType;
    typedef itk::MetaDataObject< std::string > MetaDataStringType;
    typedef itk::GDCMImageIO       ImageIOType;
    typedef itk::GDCMImageIO       ImageIOType;
    ImageIOType::Pointer dicomIO = ImageIOType::New();



    //----------------------------------------------------

    nameGenerator->SetUseSeriesDetails( true );
    nameGenerator->SetDirectory(directoryName);
    std::vector<QImage> images_Qt;
    typedef std::vector< std::string >   FileNamesContainer;
    FileNamesContainer fileNames;
    fileNames = nameGenerator->GetFileNames(seriesIdentifier);
    for (int i =0; i< fileNames.size(); i++){
        std::cout << fileNames[i] << std::endl;
        reader->SetFileName(fileNames[i]);
        //----------------------------
        reader->SetImageIO( dicomIO );
        //---------------------------
        reader->UpdateLargestPossibleRegion();
        reader->Update();
        //--------------------------------------------------
        /*  const  DictionaryType & dictionary = dicomIO->GetMetaDataDictionary();
        DictionaryType::ConstIterator itr = dictionary.Begin();
        DictionaryType::ConstIterator end = dictionary.End();
        while( itr != end )
        {
            itk::MetaDataObjectBase::Pointer  entry = itr->second;
            MetaDataStringType::Pointer entryvalue =
                    dynamic_cast<MetaDataStringType *>( entry.GetPointer() );
            if( entryvalue )
            {
                std::string tagkey   = itr->first;
                std::string labelId;
                bool found =  itk::GDCMImageIO::GetLabelFromTag( tagkey, labelId );
                std::string tagvalue = entryvalue->GetMetaDataObjectValue();
                if( found )
                {
                    std::cout << "(" << tagkey << ") " << labelId;
                    std::cout << " = " << tagvalue.c_str() << std::endl;
                }
                else
                {
                    std::cout << "(" << tagkey <<  ") " << "Unknown";
                    std::cout << " = " << tagvalue.c_str() << std::endl;
                }
            }
            ++itr;
        }*/


        //--------------------------------------------------
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
            myITKImage->UpdateOutputData();
            myITKImage->UpdateOutputInformation();
            QImage image_Qt =  ITKImageToQImage(myITKImage);
            images_Qt.push_back(image_Qt);
        }
        seriesItr++;
    }
    return images_Qt;
}
