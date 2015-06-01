#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QFileDialog>

#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageRegionConstIterator.h"
#include "itkAdaptiveHistogramEqualizationImageFilter.h"

int main(int argc, char *argv[])
{
    typedef signed short    PixelType;
    const unsigned int      Dimension = 2;
    typedef itk::Image< PixelType, Dimension >ImageType;
    typedef itk::ImageSeriesReader< ImageType >        ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    typedef itk::GDCMImageIO       ImageIOType;
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    reader->SetImageIO( dicomIO );
    reader->SetFileName("im0");
    reader->Update();

ImageType::Pointer myITKImage = reader->GetOutput();





int wight = myITKImage->GetLargestPossibleRegion().GetSize()[0];
int height = myITKImage->GetLargestPossibleRegion().GetSize()[1];

QImage *image_Qt = new QImage (wight,height,QImage::Format_RGB32);
for(unsigned int i = 0; i < wight-1; i++)
  {
  for(unsigned int j = 0; j < height-1; j++)
  {
    ImageType::IndexType pixelIndex;
    pixelIndex[0] = i;
    pixelIndex[1] = j;

   int pixelValue = myITKImage->GetPixel(pixelIndex);
   //if(pixelValue < 0)
    //   pixelValue = 0;
   if(pixelValue > 255)
       pixelValue = 255;

   QRgb value = qRgb(pixelValue,pixelValue,pixelValue);
   image_Qt->setPixel(i,j, value );
  }

  }
    QApplication app(argc, argv);
    QLabel myLabel;
    myLabel.setPixmap(QPixmap::fromImage(*image_Qt));
    myLabel.show();
    return app.exec();
}
