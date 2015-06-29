#ifndef MY3DVIEWER_H
#define MY3DVIEWER_H

#include <QWidget>
#include "DGtal/io/viewers/Viewer3D.h"
#include "Controller.hpp"

using namespace DGtal;
using namespace Z3i;

class My3Dviewer : QWidget
{
public:
    My3Dviewer(QWidget *parent);
    void creatScene(Controller *controller);
private:
    Viewer3D<> *viewer;
};

#endif // MY3DVIEWER_H
