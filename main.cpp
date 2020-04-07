#include "mainwindow.h"
#include <QApplication>
#include <stdio.h>
#include "ModelViewerWidget.h"

#include "ModelViewerWidget_Qt3D.h"
#include "PolyhedronTreeView_Qt3D.h"

#include "v2/CameraControllerFactory_MyOrbitCamera.h"
#include "v2/PolyhedronTreeView_v2.h"
#include "v2/EntityController_Polyhedron.h"

#include <thread>

#include <cmath>

namespace bstu {
    Tree* create_box();
    Tree* create_greenhouse(int n);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bstu::Tree* greenhouse = bstu::create_greenhouse(10);
    //greenhouse->restructure();

    bstu::Tree* box = bstu::create_box();

    bstu::PolyhedronTreeView_v2* tree_view = new bstu::PolyhedronTreeView_v2(greenhouse);
    bstu::CameraControllerFactory_MyOrbitCamera factory;
    bstu::View3D* view = new bstu::View3D();
    view->setCameraController(&factory);
    bstu::EntityController_Polyhedron* controller = new bstu::EntityController_Polyhedron(view);
    QObject::connect(tree_view, SIGNAL(polyhedronSelected(Polyhedron*)), controller, SLOT(addEntity(Polyhedron*)));
    QObject::connect(tree_view, SIGNAL(polyhedronUnselected(Polyhedron*)), controller, SLOT(removeEntity(Polyhedron*)));

//    /// Настраиваем освещение
//    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity();
//    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
//    light->setColor("white");
//    light->setIntensity(1);
//    lightEntity->addComponent(light);
//    /// Задаем позицию источника света как отдельное свойство
//    /// Прикрепляем позицию источника света к позиции камеры
//    Qt3DCore::QTransform *lightTransform = view->->componentsOfType<Qt3DCore::QTransform>().first();
//    lightEntity->addComponent(lightTransform);

    tree_view->show();
    view->container()->show();

    return a.exec();
}




namespace bstu {

Tree* create_box()
{
    Vertex PZ1[4] = { Vertex(-1, -1, 1), Vertex(-1, 1, 1), Vertex(1, -1, 1), Vertex(1, 1, 1) };
    Vertex PZ2[4] = { Vertex(-1, -1, -1), Vertex(-1, 1, -1), Vertex(1, -1, -1), Vertex(1, 1, -1) };
    Vertex PX1[4] = { Vertex(1, -1, -1), Vertex(1, -1, 1), Vertex(1, 1, -1), Vertex(1, 1, 1) };
    Vertex PX2[4] = { Vertex(-1, -1, -1), Vertex(-1, -1, 1), Vertex(-1, 1, -1), Vertex(-1, 1, 1) };
    Vertex PY1[4] = { Vertex(-1, 1, -1), Vertex(-1, 1, 1), Vertex(1, 1, -1), Vertex(1, 1, 1) };
    Vertex PY2[4] = {Vertex(1, -1, 1), Vertex(1, -1, -1), Vertex(-1, -1, 1), Vertex(-1, -1, -1)  };
    VolumePolygon* P[6];
    P[0] = new VolumePolygon(PX1, 4);
    P[1] = new VolumePolygon(PX2, 4);
    P[2] = new VolumePolygon(PY1, 4);
    P[3] = new VolumePolygon(PY2, 4);
    P[4] = new VolumePolygon(PZ1, 4);
    P[5] = new VolumePolygon(PZ2, 4);
    Polyhedron* T = new Polyhedron(P, 6);
    T->halve(bstu::Plane(1, -1, 0, 0));
    T->halve(bstu::Plane(-1, -1, 0, 0));
    Tree* tree = new Tree;
    tree->addPolyhedron(T);
    return tree;
}



Tree* create_greenhouse(int n)
{
    int nVertices = 2*n + 4;
    Vertex* mV = new Vertex[nVertices];

    double R = 2.5;
    Vertex* V1 = new Vertex[n];
    Vertex* V2 = new Vertex[n];
    VolumePolygon** P = new VolumePolygon*[n+6];
    for (int i=0; i < n; i++)
    {
      float angle = /*-3.1415/2 +*/ 3.14159265 * i / (n-1);
      mV[i] = Vertex(std::cos(angle)*R, std::sin(angle)*R, 0);
      mV[i+n] = Vertex(std::cos(angle)*R, std::sin(angle)*R, 10);
      V1[i] = mV[i];
      V2[i] = mV[i+n];
    }
    for (int i=0; i < n-1; i++)
    {
      Vertex V[4] = {V1[i], V2[i], V2[(i+1)%n], V1[(i+1)%n]};
      P[i] = new VolumePolygon(V, 4);
    }
    Vertex V[4];
    // Вершины основания теплицы
    mV[2*n] = Vertex(-R, -1, 0);
    mV[2*n+1] = Vertex(R, -1, 0);
    mV[2*n+2] = Vertex(-R, -1, 10);
    mV[2*n+3] = Vertex(R, -1, 10);
    P[n-1] = new VolumePolygon(V1, n);
    P[n] = new VolumePolygon(V2, n);
    // Нижний 4-угольник на торце
    V[0] = V1[0]; V[1] = V1[n-1]; V[2] = mV[2*n]; V[3] = mV[2*n+1];
    P[n+1] = new VolumePolygon(V, 4);
    // Нижний 4-угольник на втором торце
    V[0] = V2[0]; V[1] = V2[n-1]; V[2] = mV[2*n+2]; V[3] = mV[2*n+3];
    P[n+2] = new VolumePolygon(V, 4);
    // Боковые прямоугольники
    V[0] = V1[0]; V[1] = mV[2*n+1]; V[2] = mV[2*n+3]; V[3] = V2[0];
    P[n+3] = new VolumePolygon(V, 4);
    V[0] = V1[n-1]; V[1] = mV[2*n]; V[2] = mV[2*n+2]; V[3] = V2[n-1];
    P[n+4] = new VolumePolygon(V, 4);
    // Основание
    V[0] = mV[2*n];
    V[1] = mV[2*n+1];
    V[2] = mV[2*n+2];
    V[3] = mV[2*n+3];
    P[n+5] = new VolumePolygon(V, 4);

    Tree* tree = new Tree();
    Polyhedron* polyhedron = new Polyhedron(P, n+6);
    tree->addPolyhedron(polyhedron);

    // Разбиение
    int m = 10;
    double h = 2.5 * 2 / m;
    for (int i=0; i < m-1; i++)
    {
      polyhedron->halve(Plane(-1, 0, 0, -2.5 + h*(i+1))); // Разбиение по оси x
    }

    h = (2.5 + 1) / m;
    for (int i=0; i < m-1; i++)
    {
      polyhedron->halve(Plane(0, -1, 0, -1 + h*(i+1))); // Разбиение по оси y
    }

    h = (10.0) / m;
    for (int i=0; i < m-1; i++)
    {
      polyhedron->halve(Plane(0, 0, -1, h*(i+1))); // Разбиение по оси z
    }

    polyhedron->link();
    polyhedron->link();

    double mes = 3.14159265*R*R / 2 * 10 + 5*10;

    delete []V1;
    delete []V2;
    delete []P;
    delete []mV;

    return tree;
}



}
