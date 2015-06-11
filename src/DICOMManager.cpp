#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkImageSeriesReader.h"
#include "itkImage.h"
#include "itkImageRegionConstIterator.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkResampleImageFilter.h"
#include "itkCannyEdgeDetectionImageFilter.h"
#include "itkIntensityWindowingImageFilter.h"
#include "itkMinimumMaximumImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkCastImageFilter.h"
#include "itkConfidenceConnectedImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkGeodesicActiveContourLevelSetImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkFastMarchingImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkVotingBinaryIterativeHoleFillingImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkImageDuplicator.h"




#include "DICOMManager.hpp"
#include <QImage>


template<typename TImage, typename TLabelImage>
static void SummarizeLabelStatistics (TImage* image,TLabelImage* labelImage);
const  unsigned int Dimension = 2;

DICOMMManager::DICOMMManager(){
}


QImage DICOMMManager::ITKImageToQImage(ImageType::Pointer myITKImage){
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

DICOMMManager::ImageType::Pointer DICOMMManager::fillHoleInBinary(ImageType::Pointer src, int sizeMax){
    int wight = src->GetLargestPossibleRegion().GetSize()[0];
    int height = src->GetLargestPossibleRegion().GetSize()[1];
    typedef itk::ImageDuplicator< ImageType > DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage(src);
    duplicator->Update();
    ImageType::Pointer dest =  duplicator->GetOutput();
    std::vector<ImageType::IndexType> indexs;
    for(unsigned int i = 0; i < wight; i++)
    {
        bool alreadyPassIn = false;
        indexs.clear();
        for(unsigned int j = 0; j < height; j++)
        {
            ImageType::IndexType pixelIndex;
            pixelIndex[0] = i;
            pixelIndex[1] = j;
            int pixelValue = src->GetPixel(pixelIndex);
            if(pixelValue == 255){
                if(indexs.size() > sizeMax)
                    indexs.clear();
                for(int k =0; k < indexs.size(); k++){
                    dest->SetPixel(indexs[k],255);
                }
                indexs.clear();
                if(!alreadyPassIn)
                    alreadyPassIn = true;
                dest->SetPixel(pixelIndex,255);
            }
            else{
                if(alreadyPassIn)
                    indexs.push_back(pixelIndex);
                dest->SetPixel(pixelIndex,0);
            }
        }
    }
    for(unsigned int j = 0; j < height; j++)
    {
        bool alreadyPassIn = false;
        indexs.clear();
        for(unsigned int i = 0; i < wight; i++)
        {
            ImageType::IndexType pixelIndex;
            pixelIndex[0] = i;
            pixelIndex[1] = j;
            int pixelValue = dest->GetPixel(pixelIndex);
            if(pixelValue == 255){
                if(indexs.size() > sizeMax)
                    indexs.clear();
                for(int k =0; k < indexs.size(); k++){
                    dest->SetPixel(indexs[k],255);
                }
                indexs.clear();
                if(!alreadyPassIn)
                    alreadyPassIn = true;
                dest->SetPixel(pixelIndex,255);
            }
            else{
                if(alreadyPassIn)
                    indexs.push_back(pixelIndex);
                dest->SetPixel(pixelIndex,0);
            }
        }
    }
    return dest;
}

/*
DICOMMManager::ImageType::Pointer DICOMMManager::QTimageToITKImage(QImage image){
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
*/

DICOMMManager::ImageType::Pointer DICOMMManager::smoothImage(ImageType::Pointer src){
    typedef   float InternalPixelType;
    typedef itk::Image<InternalPixelType, Dimension> InternalImageType;
    typedef itk::CurvatureFlowImageFilter< InternalImageType, InternalImageType >CurvatureFlowImageFilterType;
    CurvatureFlowImageFilterType::Pointer smoothing = CurvatureFlowImageFilterType::New();
    typedef itk::CastImageFilter< ImageType, InternalImageType > CastFilterTypeIn;
    typedef itk::CastImageFilter< InternalImageType, ImageType > CastFilterTypeOut;
    CastFilterTypeIn::Pointer castFilterIn = CastFilterTypeIn::New();
    CastFilterTypeOut::Pointer castFilterOut = CastFilterTypeOut::New();
    castFilterIn->SetInput(src);
    castFilterIn->Update();

    smoothing->SetInput(castFilterIn->GetOutput());
    smoothing->SetNumberOfIterations( 5 );
    smoothing->SetTimeStep( 0.125 );
    smoothing->Update();

    castFilterOut->SetInput(smoothing->GetOutput());
    castFilterOut->Update();

    return castFilterOut->GetOutput();
}

DICOMMManager::ImageType::Pointer DICOMMManager::extractSelectedRegion(ImageType::Pointer src ,int seedPosX,int seedPosY){
    typedef   float InternalPixelType;
    typedef itk::Image<InternalPixelType, Dimension> InternalImageType;
    typedef itk::CastImageFilter< ImageType, InternalImageType > CastFilterTypeIn;
    typedef itk::CastImageFilter< InternalImageType, ImageType > CastFilterTypeOut;
    CastFilterTypeIn::Pointer castFilterIn = CastFilterTypeIn::New();
    CastFilterTypeOut::Pointer castFilterOut = CastFilterTypeOut::New();
    typedef itk::CurvatureFlowImageFilter< InternalImageType, InternalImageType >CurvatureFlowImageFilterType;
    CurvatureFlowImageFilterType::Pointer smoothing = CurvatureFlowImageFilterType::New();
    typedef itk::ConfidenceConnectedImageFilter<InternalImageType, InternalImageType> ConnectedFilterType;
    ConnectedFilterType::Pointer confidenceConnected = ConnectedFilterType::New();
    InternalImageType::IndexType  index;
    index[0] = seedPosX;
    index[1] = seedPosY;

    castFilterIn->SetInput(src);
    castFilterIn->Update();

    smoothing->SetInput( castFilterIn->GetOutput() );
    smoothing->SetNumberOfIterations( 5 );
    smoothing->SetTimeStep( 0.125 );
    smoothing->Update();

    confidenceConnected->SetMultiplier( 5 );
    confidenceConnected->SetNumberOfIterations(5);
    confidenceConnected->SetReplaceValue(255);
    confidenceConnected->SetSeed( index );
    confidenceConnected->SetInitialNeighborhoodRadius(2);
    confidenceConnected->SetInput( smoothing->GetOutput() );
    confidenceConnected->Update();

    castFilterOut->SetInput(confidenceConnected->GetOutput());
    castFilterOut->Update();

    return castFilterOut->GetOutput();

}
DICOMMManager::ImageType::Pointer DICOMMManager::enhanceSelectedRegion(ImageType::Pointer src ,int seedPosX,int seedPosY){
    typedef itk::AddImageFilter <ImageType, ImageType >AddImageFilterType;
    AddImageFilterType::Pointer addFilter = AddImageFilterType::New ();
    typedef itk::BinaryBallStructuringElement<ImageType::PixelType, ImageType::ImageDimension>
            StructuringElementType;
    StructuringElementType structuringElement;
    typedef itk::BinaryDilateImageFilter <ImageType, ImageType, StructuringElementType> BinaryDilateImageFilterType;
    BinaryDilateImageFilterType::Pointer dilateFilter  = BinaryDilateImageFilterType::New();
    typedef itk::InvertIntensityImageFilter <ImageType> InvertIntensityImageFilterType;
    InvertIntensityImageFilterType::Pointer invertIntensityFilter = InvertIntensityImageFilterType::New();
    typedef itk::MaskImageFilter< ImageType, ImageType > MaskFilterType;
    MaskFilterType::Pointer maskFilter = MaskFilterType::New();


    ImageType::Pointer selectRegion = extractSelectedRegion(src , seedPosX, seedPosY);

    /*structuringElement.SetRadius(25);
    structuringElement.CreateStructuringElement();
    dilateFilter->SetInput(confidenceConnected->GetOutput());
    dilateFilter->SetKernel(structuringElement);
    dilateFilter->Update();*/


    ImageType::Pointer regionWithoutHole = fillHoleInBinary(selectRegion,30);

    maskFilter->SetInput(src);
    maskFilter->SetMaskImage(regionWithoutHole);
    maskFilter->Update();
    addFilter->SetInput1(enhanceContrast(rescale(maskFilter->GetOutput())));

    invertIntensityFilter->SetInput(regionWithoutHole);
    invertIntensityFilter->SetMaximum(255);
    invertIntensityFilter->Update();
    maskFilter->SetInput(src);
    maskFilter->SetMaskImage(invertIntensityFilter->GetOutput());
    maskFilter->Update();
    addFilter->SetInput2(rescale(maskFilter->GetOutput()));

    addFilter->Update();

    return addFilter->GetOutput();
}

DICOMMManager::ImageType::Pointer DICOMMManager::extractBronchiInRegion(ImageType::Pointer src ,int seedPosX,int seedPosY){
    typedef itk::SubtractImageFilter <ImageType, ImageType >  SubtractImageFilterType;
    SubtractImageFilterType::Pointer subtractFilter = SubtractImageFilterType::New ();
    typedef itk::MaskImageFilter< ImageType, ImageType > MaskFilterType;
    MaskFilterType::Pointer maskFilter = MaskFilterType::New();

    ImageType::Pointer selectRegion = extractSelectedRegion(src , seedPosX, seedPosY);
    ImageType::Pointer regionWithoutHole = fillHoleInBinary(selectRegion,30);

    subtractFilter->SetInput1(regionWithoutHole);
    subtractFilter->SetInput2(selectRegion);
    subtractFilter->Update();

    maskFilter->SetInput(src);
    maskFilter->SetMaskImage(subtractFilter->GetOutput());
    maskFilter->Update();

    return enhanceContrast(rescale(maskFilter->GetOutput()));
}


DICOMMManager::ImageType::Pointer DICOMMManager::extractRegion(ImageType::Pointer src ,int startX,
                                                               int startY, int endX, int endY){
    const itk::IndexValueType startx =
            static_cast< itk::IndexValueType >(startX);
    const itk::IndexValueType endx =
            static_cast< itk::IndexValueType >(endX);
    const itk::IndexValueType starty =
            static_cast< itk::IndexValueType >(startY);
    const itk::IndexValueType endy =
            static_cast< itk::IndexValueType >(endY);
    int width = src->GetLargestPossibleRegion().GetSize()[0];
    int height = src->GetLargestPossibleRegion().GetSize()[1];
    float ratio = 1;


    ImageType::IndexType start;
    start[0] = startx;
    start[1] = starty;

    ImageType::IndexType end;
    end[0] = endx;
    end[1] = endy;

    int centerX = startx + (endx-startx)/2;
    int centerY = starty + (endy-starty)/2;


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

    ImageType::Pointer dest = filter->GetOutput();
    int destWidth = dest->GetLargestPossibleRegion().GetSize()[0];
    int destHeight = dest->GetLargestPossibleRegion().GetSize()[1];
    if(destWidth > destHeight)
        ratio = ((float)width/destWidth);
    else
        ratio = ((float)height/destHeight);
    dest->Update();
    dest = zoom(filter->GetOutput(),ratio,dest->GetOrigin()[0],dest->GetOrigin()[1]);
    dest->Update();
    return dest;
}

DICOMMManager::ImageType::Pointer  DICOMMManager::getEdges(ImageType::Pointer  src,int threshold){
    typedef  float          InputPixelType;
    typedef itk::Image<InputPixelType, Dimension>  InputImageType;

    typedef itk::CastImageFilter< ImageType, InputImageType > CastFilterType;
    CastFilterType::Pointer castFilter = CastFilterType::New();
    castFilter->SetInput(src);
    castFilter->Update();

    typedef itk::CannyEdgeDetectionImageFilter< InputImageType, InputImageType >
            FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(castFilter->GetOutput());
    filter->SetVariance(8);
    filter->SetUpperThreshold(threshold);
    filter->SetLowerThreshold(0);
    filter->Update();

    typedef itk::RescaleIntensityImageFilter< InputImageType, ImageType > RescaleType;
    RescaleType::Pointer rescaler = RescaleType::New();
    rescaler->SetInput(filter->GetOutput());
    rescaler->SetOutputMinimum(0);
    rescaler->SetOutputMaximum(255);
    rescaler->Update();

    return rescaler->GetOutput();

}





DICOMMManager::ImageType::Pointer DICOMMManager::zoom(ImageType::Pointer myITKImage, double factor, int posX, int posY){

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
    const double vfOutputOrigin[2]  = {posX, posY};
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
    // myITKImage = _pResizeFilter->GetOutput();
    //myITKImage->Update();
    //myITKImage = extractRegion(myITKImage ,0,0, wight-1, height-1);
    //myITKImage->Update();
    return _pResizeFilter->GetOutput();
}

DICOMMManager::ImageType::Pointer DICOMMManager::rescale(ImageType::Pointer myITKImage){
    typedef itk::RescaleIntensityImageFilter< ImageType, ImageType >
            RescaleFilterType;
    RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
    rescaleFilter->SetInput(myITKImage);
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();
    return rescaleFilter->GetOutput();
}


DICOMMManager::ImageType::Pointer DICOMMManager::getImageFromSerie(int num){
    ImageType::Pointer Null;
    if(num < actualSerie.size() && num >= 0)
        return rescale(smoothImage(actualSerie[num]));
    return Null;
}

DICOMMManager::ImageType::Pointer DICOMMManager::enhanceContrast(ImageType::Pointer myITKImage){
    typedef itk::IntensityWindowingImageFilter <ImageType, ImageType> IntensityWindowingImageFilterType;
    typedef itk::MinimumMaximumImageCalculator <ImageType>
            ImageCalculatorFilterType;
    ImageCalculatorFilterType::Pointer imageCalculatorFilter
            = ImageCalculatorFilterType::New ();
    imageCalculatorFilter->SetImage(myITKImage);
    imageCalculatorFilter->Compute();
    IntensityWindowingImageFilterType::Pointer filter = IntensityWindowingImageFilterType::New();
    filter->SetInput(myITKImage);
    filter->SetWindowMinimum(imageCalculatorFilter->GetMinimum());
    filter->SetWindowMaximum(imageCalculatorFilter->GetMaximum());
    filter->SetOutputMinimum(0);
    filter->SetOutputMaximum(255);
    filter->Update();

    return filter->GetOutput();
}



std::vector<std::string> DICOMMManager::getSeries(std::string directoryName){
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

void DICOMMManager::loadDICOMSerie(std::string seriesIdentifier,
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
    typedef std::vector< std::string >   FileNamesContainer;
    FileNamesContainer fileNames;
    fileNames = nameGenerator->GetFileNames(seriesIdentifier);
    actualSerie.clear();
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
        myITKImage = rescale(myITKImage);
        actualSerie.push_back(myITKImage);
    }

}
