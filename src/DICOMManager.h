#ifndef DICOMMANAGER_H
#define DICOMMANAGER_H

#include <QApplication>
QImage getDICOM(std::string filename);
std::vector<QImage> getDICOMDirectory(std::string directoryName);
std::vector<std::string> getSeries(std::string directoryName);
std::vector<QImage> getDICOMSerie(std::string seriesIdentifier,
                                  std::string directoryName);
QImage zoom(QImage image, double factor, int posX, int posY);

#endif // DICOMMANAGER_H
