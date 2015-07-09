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
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkImageToHistogramFilter.h"
#include "itkNeighborhoodConnectedImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryMorphologicalOpeningImageFilter.h"
#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "DICOMManager.hpp"
#include "itkBinaryImageToShapeLabelMapFilter.h"
#include "itkLabelMapToBinaryImageFilter.h"


template<typename TImage, typename TLabelImage>
static void SummarizeLabelStatistics (TImage* image,TLabelImage* labelImage);
const  unsigned int Dimension = 2;

//https://en.wikibooks.org/wiki/Algorithm_Implementation/Geometry/Convex_hull/Monotone_chain
typedef double coord_t;
typedef double coord2_t;
struct Point {
    coord_t x, y;

    bool operator <(const Point &p) const {
        return x < p.x || (x == p.x && y < p.y);
    }
};

coord2_t cross(const Point &O, const Point &A, const Point &B)
{
    return (long)(A.x - O.x) * (B.y - O.y) - (long)(A.y - O.y) * (B.x - O.x);
}

std::vector<Point> convex_hull(std::vector<Point> P)
{
    int n = P.size(), k = 0;
    std::vector<Point> H(2*n);

    // Sort points lexicographically
    std::sort(P.begin(), P.end());

    // Build lower hull
    for (int i = 0; i < n; ++i) {
        while (k >= 2 && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
        H[k++] = P[i];
    }

    // Build upper hull
    for (int i = n-2, t = k+1; i >= 0; i--) {
        while (k >= t && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
        H[k++] = P[i];
    }

    H.resize(k);
    return H;
}
//-----------------------------------------------------------------------------------------------------

DICOMMManager::ImageType::Pointer  DICOMMManager::getConvexHull(ImageType::Pointer src){
    int wight = src->GetLargestPossibleRegion().GetSize()[0];
    int height = src->GetLargestPossibleRegion().GetSize()[1];
    ImageType::Pointer dest = ImageType::New();
    ImageType::RegionType region;
    ImageType::IndexType start;
    start[0] = 0;
    start[1] = 0;
    ImageType::SizeType size;
    unsigned int NumRows = wight;
    unsigned int NumCols = height;
    size[0] = NumRows;
    size[1] = NumCols;
    region.SetSize(size);
    region.SetIndex(start);
    dest->SetRegions(region);
    dest->SetOrigin(src->GetOrigin());
    dest->Allocate();

    for(unsigned int i = 0; i <wight; i++)
        for(unsigned int j = 0; j < height; j++)
        {
            ImageType::IndexType pixelIndex;
            pixelIndex[0] = i;
            pixelIndex[1] = j;
            dest->SetPixel(pixelIndex,0);
        }
    std::vector<Point> P;
    for(unsigned int i = 0; i < wight; i++)
    {
        for(unsigned int j = 1; j < height-1; j++)
        {
            ImageType::IndexType pixelIndexBef;
            pixelIndexBef[0] = i;
            pixelIndexBef[1] = j-1;
            ImageType::IndexType pixelIndex;
            pixelIndex[0] = i;
            pixelIndex[1] = j;
            ImageType::IndexType pixelIndexAft;
            pixelIndexAft[0] = i;
            pixelIndexAft[1] = j+1;
            int pixelValueBef = src->GetPixel(pixelIndexBef);
            int pixelValue = src->GetPixel(pixelIndex);
            int pixelValueAft = src->GetPixel(pixelIndexAft);
            if(pixelValue == 255 && (pixelValueBef == 0 || pixelValueAft == 0)){
                Point p;
                p.x = i;
                p.y = j;
                P.push_back(p);
            }
        }

    }
    P = convex_hull(P);
    for(unsigned int i = 1; i < P.size(); i++){

        ImageType::IndexType nextPixelIndex;
        nextPixelIndex[0] = P[i-1].x;
        nextPixelIndex[1] = P[i-1].y;


        float actualX =  P[i].x;
        float actualY =  P[i].y;
        float startY = actualY;
        float startX = actualX;
        float ratioX = fabs((nextPixelIndex[0]-startX)/(nextPixelIndex[1]-startY));
        float ratioY = fabs((nextPixelIndex[1]-startY)/(nextPixelIndex[0]-startX));
        if(ratioX > 1 || ratioX==0)
            ratioX = 1;
        if(ratioY > 1 || ratioY==0)
            ratioY = 1;
        if(nextPixelIndex[0] > 15 && nextPixelIndex[1] > 15){
            while((int)actualX != nextPixelIndex[0] || (int)actualY != nextPixelIndex[1] ){
                ImageType::IndexType pixelIndex;
                pixelIndex[0] = (int)actualX;
                pixelIndex[1] = (int)actualY;
                dest->SetPixel(pixelIndex, 255);

                if(nextPixelIndex[0]-(int)actualX != 0){
                    actualX += (((nextPixelIndex[0]-(int)actualX))/abs((nextPixelIndex[0]-(int)actualX))) * ratioX;
                }
                if(nextPixelIndex[1]-(int)actualY != 0){
                    actualY += ((nextPixelIndex[1]-(int)actualY)/abs((int)actualY-nextPixelIndex[1])) *  ratioY;
                }
            }
        }
    }


    for(unsigned int i = 0; i < wight; i++)
    {
        bool isIn = false;
        for(unsigned int j = 1; j < height-1; j++)
        {
            ImageType::IndexType pixelIndex;
            pixelIndex[0] = i;
            pixelIndex[1] = j;
            int pixelValue = dest->GetPixel(pixelIndex);
            ImageType::IndexType nextPixelIndex;
            nextPixelIndex[0] = i;
            nextPixelIndex[1] = j+1;
            int nextPixelValue = dest->GetPixel(nextPixelIndex);

            if(pixelValue == 255 && nextPixelValue!=255){
                if(isIn)
                    isIn = false;
                else
                    for(unsigned int k = j+1; k < height-1; k++){
                        ImageType::IndexType pixelIndex;
                        pixelIndex[0] = i;
                        pixelIndex[1] = k;
                        int pixelValue = dest->GetPixel(pixelIndex);
                        ImageType::IndexType nextPixelIndex;
                        nextPixelIndex[0] = i;
                        nextPixelIndex[1] = j+1;
                        int nextPixelValue = dest->GetPixel(nextPixelIndex);
                        if(pixelValue == 255 && nextPixelValue!=255)
                            isIn = true;

                    }
            }
            if(isIn)
                dest->SetPixel(pixelIndex,255);
        }
    }
    return dest;
}


//-------------------------------------------------------------------------------------------------------------

DICOMMManager::ImageType::Pointer  DICOMMManager::convexHull(ImageType::Pointer src){
    typedef itk::BinaryImageToShapeLabelMapFilter<ImageType> BinaryImageToShapeLabelMapFilterType;
    BinaryImageToShapeLabelMapFilterType::Pointer binaryImageToShapeLabelMapFilter = BinaryImageToShapeLabelMapFilterType::New();
    typedef BinaryImageToShapeLabelMapFilterType::OutputImageType LabelMapType;
    typedef BinaryImageToShapeLabelMapFilterType::LabelObjectType LabelObjectType;

    typedef itk::LabelMapToBinaryImageFilter<BinaryImageToShapeLabelMapFilterType::OutputImageType,ImageType> LabelMapToBinaryImageFilterType;
    LabelMapToBinaryImageFilterType::Pointer LabelMapToBinaryImageFilter = LabelMapToBinaryImageFilterType::New();
    LabelMapToBinaryImageFilterType::Pointer LabelMapToBinaryImageFilter2 = LabelMapToBinaryImageFilterType::New();
    typedef itk::AddImageFilter <ImageType, ImageType >
            AddImageFilterType;
    AddImageFilterType::Pointer addFilter
            = AddImageFilterType::New ();

    int wight = src->GetLargestPossibleRegion().GetSize()[0];
    int height = src->GetLargestPossibleRegion().GetSize()[1];
    ImageType::Pointer lung1 = ImageType::New();
    ImageType::Pointer lung2 = ImageType::New();
    ImageType::RegionType region;
    ImageType::IndexType start;
    start[0] = 0;
    start[1] = 0;
    ImageType::SizeType size;
    unsigned int NumRows = wight;
    unsigned int NumCols = height;
    size[0] = NumRows;
    size[1] = NumCols;

    region.SetSize(size);
    region.SetIndex(start);
    lung1->SetRegions(region);
    lung1->SetOrigin(src->GetOrigin());
    lung1->SetSpacing(src->GetSpacing());
    lung1->Allocate();


    lung2->SetRegions(region);
    lung2->SetOrigin(src->GetOrigin());
    lung2->SetSpacing(src->GetSpacing());
    lung2->Allocate();
    BinaryImageToShapeLabelMapFilterType::OutputImageType *shapeLabel;

    for(unsigned int i = 0; i <wight; i++)
        for(unsigned int j = 0; j < height; j++)
        {
            ImageType::IndexType pixelIndex;
            pixelIndex[0] = i;
            pixelIndex[1] = j;
            lung1->SetPixel(pixelIndex,0);
            lung2->SetPixel(pixelIndex,0);
        }
    //------------------------------------------------------------------------------------------
    binaryImageToShapeLabelMapFilter->SetInput(src);
    binaryImageToShapeLabelMapFilter->SetInputForegroundValue(255);
    binaryImageToShapeLabelMapFilter->Update();

    shapeLabel = binaryImageToShapeLabelMapFilter->GetOutput();
    if(shapeLabel->GetNumberOfLabelObjects() != 2)
        return src;

    for(unsigned int i = 0; i < binaryImageToShapeLabelMapFilter->GetOutput()->GetNumberOfLabelObjects(); i++)
    {
        LabelObjectType::Pointer labelObject = binaryImageToShapeLabelMapFilter->GetOutput()->GetNthLabelObject(i);
        for(unsigned int pixelId = 0; pixelId < labelObject->Size(); pixelId++)
        {
            if(i==0)
                lung1->SetPixel(labelObject->GetIndex(pixelId),255);
            else
                lung2->SetPixel(labelObject->GetIndex(pixelId),255);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------


    addFilter->SetInput1(getConvexHull(lung1));
    addFilter->SetInput2(getConvexHull(lung2));
    addFilter->Update();

    return addFilter->GetOutput();

}






DICOMMManager::ImageType::Pointer  DICOMMManager::removeBackground(ImageType::Pointer src){
    typedef itk::NeighborhoodConnectedImageFilter<ImageType,
            ImageType > ConnectedFilterType;
    ConnectedFilterType::Pointer neighborhoodConnected
            = ConnectedFilterType::New();


    int wight = src->GetLargestPossibleRegion().GetSize()[0];
    int height = src->GetLargestPossibleRegion().GetSize()[1];
    typedef itk::SubtractImageFilter <ImageType, ImageType >
            SubtractImageFilterType;
    SubtractImageFilterType::Pointer subtractFilter
            = SubtractImageFilterType::New ();

    for(unsigned int i = 0; i <wight; i++)
        for(unsigned int j = 0; j < height; j++)
        {
            if(i == 0 || j ==0 || i == wight-1 || j == height-1){
                ImageType::IndexType pixelIndex;
                pixelIndex[0] = i;
                pixelIndex[1] = j;
                int pixelValue = src->GetPixel(pixelIndex);
                if(pixelValue == 255){
                    neighborhoodConnected->SetInput(src);
                    neighborhoodConnected->SetLower(2);
                    neighborhoodConnected->SetUpper(255);
                    ImageType::SizeType   radius;
                    radius[0] = 0;
                    radius[1] = 0;
                    neighborhoodConnected->SetRadius(radius);
                    ImageType::IndexType  index;
                    index[0] = i;
                    index[1] = j;
                    neighborhoodConnected->SetSeed( index );
                    neighborhoodConnected->SetReplaceValue(255);
                    neighborhoodConnected->Update();

                    subtractFilter->SetInput1(src);
                    subtractFilter->SetInput2(neighborhoodConnected->GetOutput());
                    subtractFilter->Update();
                    src = subtractFilter->GetOutput();
                    src->Update();
                }
            }
        }
    return src;
}


DICOMMManager::ImageType::Pointer DICOMMManager::threshold(ImageType::Pointer src , int upperThreshold, int lowerThreshold){
    typedef itk::BinaryThresholdImageFilter <ImageType, ImageType>
            BinaryThresholdImageFilterType;
    BinaryThresholdImageFilterType::Pointer thresholdFilter
            = BinaryThresholdImageFilterType::New();
    typedef itk::MaskImageFilter< ImageType, ImageType > MaskFilterType;
    MaskFilterType::Pointer maskFilter = MaskFilterType::New();



    thresholdFilter->SetInput(src);
    thresholdFilter->SetLowerThreshold(lowerThreshold);
    thresholdFilter->SetUpperThreshold(upperThreshold);
    thresholdFilter->SetInsideValue(255);
    thresholdFilter->SetOutsideValue(0);
    thresholdFilter->Update();
    //return thresholdFilter->GetOutput();
    ImageType::Pointer dest = thresholdFilter->GetOutput();


    dest = removeBackground(dest);
    dest = lungSegementation(dest);
    dest = fileHoleInBinary(dest);

    dest = convexHull(dest);
    dest->SetSpacing(src->GetSpacing());
    maskFilter->SetInput(src);
    maskFilter->SetMaskImage(dest);
    maskFilter->Update();
    return enhanceContrast(rescale(maskFilter->GetOutput()));
}


std::vector<int> DICOMMManager::getHistogram(int num){
    std::vector<int> actualHistogram;
    if(num < actualSerie.size()){
        ImageType::Pointer src = actualSerie[num];
        typedef itk::Statistics::ImageToHistogramFilter< ImageType >
                ImageToHistogramFilterType;

        ImageToHistogramFilterType::HistogramType::MeasurementVectorType
                lowerBound(src->GetImageDimension());
        lowerBound.Fill(0);

        ImageToHistogramFilterType::HistogramType::MeasurementVectorType
                upperBound(src->GetImageDimension());
        upperBound.Fill(255) ;

        ImageToHistogramFilterType::HistogramType::SizeType
                size(src->GetImageDimension());
        size.Fill(255);

        ImageToHistogramFilterType::Pointer imageToHistogramFilter =
                ImageToHistogramFilterType::New();
        imageToHistogramFilter->SetInput(src);
        imageToHistogramFilter->SetHistogramBinMinimum(lowerBound);
        imageToHistogramFilter->SetHistogramBinMaximum(upperBound);
        imageToHistogramFilter->SetHistogramSize(size);
        imageToHistogramFilter->Update();
        ImageToHistogramFilterType::HistogramType* histogram =
                imageToHistogramFilter->GetOutput();
        const unsigned int histogramSize = histogram->Size();

        for( unsigned int bin=0; bin < histogramSize; bin++ )
        {
            actualHistogram.push_back(histogram->GetFrequency(bin, 0));
        }
    }
    return actualHistogram;
}



DICOMMManager::DICOMMManager(){
}


QImage DICOMMManager::ITKImageToQImage(ImageType::Pointer myITKImage){
    int wight = myITKImage->GetLargestPossibleRegion().GetSize()[0];
    int height = myITKImage->GetLargestPossibleRegion().GetSize()[1];

    QImage image_Qt(wight,height,QImage::Format_RGB32);
    for(unsigned int i = 0; i < wight; i++)
    {
        for(unsigned int j = 0; j < height; j++)
        {
            ImageType::IndexType pixelIndex;
            pixelIndex[0] = i;
            pixelIndex[1] = j;
            int pixelValue = myITKImage->GetPixel(pixelIndex);
            QRgb value = qRgb(pixelValue,pixelValue,pixelValue);
            image_Qt.setPixel(i,j, value );
        }

    }
    return image_Qt;
}

DICOMMManager::ImageType::Pointer DICOMMManager::fileHoleInBinary(ImageType::Pointer src){
    typedef itk::BinaryBallStructuringElement<
            ImageType::PixelType,2>                  StructuringElementType;

    StructuringElementType structuringElementClose;
    typedef itk::BinaryImageToShapeLabelMapFilter<ImageType> BinaryImageToShapeLabelMapFilterType;
    BinaryImageToShapeLabelMapFilterType::Pointer binaryImageToShapeLabelMapFilter = BinaryImageToShapeLabelMapFilterType::New();

    typedef itk::LabelMapToBinaryImageFilter<BinaryImageToShapeLabelMapFilterType::OutputImageType,ImageType> LabelMapToBinaryImageFilterType;
    LabelMapToBinaryImageFilterType::Pointer LabelMapToBinaryImageFilter = LabelMapToBinaryImageFilterType::New();

    typedef itk::InvertIntensityImageFilter <ImageType>
            InvertIntensityImageFilterType;

    InvertIntensityImageFilterType::Pointer invertIntensityFilter
            = InvertIntensityImageFilterType::New();
    InvertIntensityImageFilterType::Pointer invertIntensityFilter2
            = InvertIntensityImageFilterType::New();


    typedef BinaryImageToShapeLabelMapFilterType::OutputImageType LabelMapType;
    std::vector<unsigned long> labelsToRemove;


    structuringElementClose.SetRadius(10);
    structuringElementClose.CreateStructuringElement();
    BinaryImageToShapeLabelMapFilterType::OutputImageType *shapeLabel;



    invertIntensityFilter->SetInput(src);
    invertIntensityFilter->SetMaximum(255);
    invertIntensityFilter->Update();

    binaryImageToShapeLabelMapFilter->SetInput(invertIntensityFilter->GetOutput());
    binaryImageToShapeLabelMapFilter->SetInputForegroundValue(255);
    binaryImageToShapeLabelMapFilter->Update();
    shapeLabel = binaryImageToShapeLabelMapFilter->GetOutput();
    labelsToRemove.clear();
    for(unsigned int i = 0; i < shapeLabel->GetNumberOfLabelObjects(); i++)
    {
        BinaryImageToShapeLabelMapFilterType::OutputImageType::LabelObjectType* labelObject = shapeLabel->GetNthLabelObject(i);
        if(labelObject->Size() < 10000)
            labelsToRemove.push_back(labelObject->GetLabel());

    }
    for(unsigned int i = 0; i < labelsToRemove.size(); ++i)
    {
        shapeLabel->RemoveLabel(labelsToRemove[i]);
    }
    LabelMapToBinaryImageFilter->SetInput(shapeLabel);
    LabelMapToBinaryImageFilter->SetBackgroundValue(0);
    LabelMapToBinaryImageFilter->SetForegroundValue(255);
    LabelMapToBinaryImageFilter->Update();

    invertIntensityFilter2->SetInput(LabelMapToBinaryImageFilter->GetOutput());
    invertIntensityFilter2->SetMaximum(255);
    invertIntensityFilter2->Update();

    return invertIntensityFilter2->GetOutput();

}


DICOMMManager::ImageType::Pointer DICOMMManager::lungSegementation(ImageType::Pointer src){
    typedef itk::BinaryBallStructuringElement<
            ImageType::PixelType,2>                  StructuringElementType;

    StructuringElementType structuringElementClose;
    typedef itk::BinaryMorphologicalClosingImageFilter <ImageType, ImageType, StructuringElementType>
            BinaryMorphologicalClosingImageFilterType;
    BinaryMorphologicalClosingImageFilterType::Pointer closingFilter
            = BinaryMorphologicalClosingImageFilterType::New();
    typedef itk::BinaryImageToShapeLabelMapFilter<ImageType> BinaryImageToShapeLabelMapFilterType;
    BinaryImageToShapeLabelMapFilterType::Pointer binaryImageToShapeLabelMapFilter = BinaryImageToShapeLabelMapFilterType::New();
    typedef itk::LabelMapToBinaryImageFilter<BinaryImageToShapeLabelMapFilterType::OutputImageType,ImageType> LabelMapToBinaryImageFilterType;
    LabelMapToBinaryImageFilterType::Pointer LabelMapToBinaryImageFilter = LabelMapToBinaryImageFilterType::New();
    BinaryImageToShapeLabelMapFilterType::Pointer binaryImageToShapeLabelMapFilter2 = BinaryImageToShapeLabelMapFilterType::New();
    LabelMapToBinaryImageFilterType::Pointer LabelMapToBinaryImageFilter2 = LabelMapToBinaryImageFilterType::New();

    std::vector<unsigned long> labelsToRemove;
    structuringElementClose.SetRadius(2);
    structuringElementClose.CreateStructuringElement();
    BinaryImageToShapeLabelMapFilterType::OutputImageType *shapeLabel;

    binaryImageToShapeLabelMapFilter->SetInput(src);
    binaryImageToShapeLabelMapFilter->SetInputForegroundValue(255);
    binaryImageToShapeLabelMapFilter->Update();
    shapeLabel = binaryImageToShapeLabelMapFilter->GetOutput();
    for(unsigned int i = 0; i < shapeLabel->GetNumberOfLabelObjects(); i++)
    {
        BinaryImageToShapeLabelMapFilterType::OutputImageType::LabelObjectType* labelObject = shapeLabel->GetNthLabelObject(i);
        if(labelObject->Size() < 40)
            labelsToRemove.push_back(labelObject->GetLabel());
    }
    for(unsigned int i = 0; i < labelsToRemove.size(); ++i)
    {
        shapeLabel->RemoveLabel(labelsToRemove[i]);
    }
    LabelMapToBinaryImageFilter->SetInput(shapeLabel);
    LabelMapToBinaryImageFilter->SetBackgroundValue(0);
    LabelMapToBinaryImageFilter->SetForegroundValue(255);
    LabelMapToBinaryImageFilter->Update();


    closingFilter->SetInput(LabelMapToBinaryImageFilter->GetOutput());
    closingFilter->SetKernel(structuringElementClose);
    closingFilter->SetForegroundValue(255);
    closingFilter->Update();

    labelsToRemove.clear();
    BinaryImageToShapeLabelMapFilterType::OutputImageType *shapeLabel2;
    binaryImageToShapeLabelMapFilter2->SetInput(closingFilter->GetOutput());
    binaryImageToShapeLabelMapFilter2->SetInputForegroundValue(255);
    binaryImageToShapeLabelMapFilter2->Update();
    shapeLabel2 = binaryImageToShapeLabelMapFilter2->GetOutput();
    if(shapeLabel2->GetNumberOfLabelObjects() > 2){
        int max1 = 0;
        int max2 = 0;
        for(unsigned int i = 0; i < shapeLabel2->GetNumberOfLabelObjects(); i++)
        {
            BinaryImageToShapeLabelMapFilterType::OutputImageType::LabelObjectType* labelObject2 = shapeLabel2->GetNthLabelObject(i);
            if(labelObject2->Size() > max1){
                max2 = max1;
                max1 = labelObject2->Size();
            }
            if(labelObject2->Size() > max2 && labelObject2->Size() < max1)
                max2 = labelObject2->Size();
        }
        for(unsigned int i = 0; i < shapeLabel2->GetNumberOfLabelObjects(); i++)
        {
            BinaryImageToShapeLabelMapFilterType::OutputImageType::LabelObjectType* labelObject2 = shapeLabel2->GetNthLabelObject(i);
            if(labelObject2->Size() < max2)
                labelsToRemove.push_back(labelObject2->GetLabel());
        }
        for(unsigned int i = 0; i < labelsToRemove.size(); ++i)
        {
            shapeLabel2->RemoveLabel(labelsToRemove[i]);
        }
    }
    LabelMapToBinaryImageFilter2->SetInput(shapeLabel2);
    LabelMapToBinaryImageFilter2->SetBackgroundValue(0);
    LabelMapToBinaryImageFilter2->SetForegroundValue(255);
    LabelMapToBinaryImageFilter2->Update();



    return  LabelMapToBinaryImageFilter2->GetOutput();
}


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
    /*
    smoothing->SetInput( castFilterIn->GetOutput() );
    smoothing->SetNumberOfIterations( 5 );
    smoothing->SetTimeStep( 0.125 );
    smoothing->Update();
*/
    confidenceConnected->SetMultiplier(5);
    confidenceConnected->SetNumberOfIterations(5);
    confidenceConnected->SetReplaceValue(255);
    confidenceConnected->SetSeed( index );
    confidenceConnected->SetInitialNeighborhoodRadius(1);
    confidenceConnected->SetInput( castFilterIn->GetOutput() );
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


    ImageType::Pointer regionWithoutHole = fileHoleInBinary(selectRegion);

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
    ImageType::Pointer regionWithoutHole = fileHoleInBinary(selectRegion);

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
        return actualSerie[num];
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
    actualSerie = std::vector<ImageType::Pointer>();
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
        myITKImage = rescale(smoothImage(myITKImage));
        actualSerie.push_back(myITKImage);
    }

}
