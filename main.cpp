//#include "mainwindow.h"
#include <QApplication>
//#include <stdio.h>
//#include "EntityController.h"

//#include <thread>

//#include <cmath>

#include "View3D.h"
#include "EntitiesContainer.h"
#include "PolyhedronTreeView.h"

#include "collections/TransformsSet.h"

#include "initializers/CameraAndLightInitalizer.h"
#include "initializers/SelectorInitializer.h"

#include "utils/factories/imp/SimpleGeometryFactory.h"
#include "utils/factories/imp/SimpleMaterialFactory.h"
#include "utils/factories/imp/SimpleTransformFactory.h"

#include "utils/Adapter.h"
#include "utils/Activator.h"
#include "utils/Centering.hpp"
#include "utils/EntityConstructor.h"

namespace bstu {
    Tree* create_box();
    Tree* create_greenhouse(int n);


    quint64 bigRand();
    quint64 MAX_BIG_RAND_VALUE = 0xFFFFFFFFFFFFFFFl;

    Tree* createRandomSurface(Plane plane, int pointsCount);
}

using namespace bstu;
int main(int argc, char *argv[])
{
    /// QObject объекты не надо удалять, они сами удалятся при завершении процесса
    QApplication a(argc, argv);

    srand(GetCurrentTime());

    //Tree* tree = create_box();

    //Tree* tree = create_greenhouse(10);
    //tree->restructure();

    Tree* tree = createRandomSurface(Plane(0.8, 0, 0.6, 3), 30);

    /// View с отображаемыми фигурами
    View3D* view = new bstu::View3D();
    /// Контейнер, обеспечивающий связь между данными и сущностями
    EntitiesContainer* mainContainer = new EntitiesContainer(view);
    ///
    PolyhedronTreeView* tree_view = new PolyhedronTreeView(mainContainer, nullptr, tree);

    /// Инициализация источника освещения и камеры
    /// (инициализация начнеться, когда view подаст сигнал о смене статуса
    /// удалять их в ручном режиме не надо, они удаляться вместе с view
    new CameraAndLightInitalizer(view);
    /// Инициализация источника освещения и камеры
    new SelectorInitializer(view);

    AbstractGeometryFactory* geometryFactory = new SimpleGeometryFactory();
    AbstractMaterialFactory* materialFactory = new SimpleMaterialFactory(view->rootEntity());
    AbstractTransformFactory* transformFactory = new SimpleTransformFactory(view->rootEntity());

    /// Для оптимизации обновления положения, при центровке
    TransformsSet* transformsSet = new TransformsSet(view);

    /// Отвечает за создание QEntity при добалении нового полихедрона в множество
    new EntityConstructor(geometryFactory, materialFactory, transformFactory, mainContainer, transformsSet, view);
    /// Отвечает за добавление QEntity на сцену
    new Adapter(view, mainContainer);
    /// Отвечает за активацию/деактивацию QEntity
    new Activator(mainContainer);
    /// Отвечает за центровку фигур
    new Centering(mainContainer, transformsSet);

    tree_view->show();
    QWidget* widget = QWidget::createWindowContainer(view);
    widget->show();
    int result = a.exec();

    delete geometryFactory;
    delete materialFactory;
    delete transformFactory;

    return result;
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

//    double mes = 3.14159265*R*R / 2 * 10 + 5*10;

    delete []V1;
    delete []V2;
    delete []P;
    delete []mV;

    return tree;
}

quint64 bigRand() {
    quint64 result = rand();
    result <<= 15; result |= rand();
    result <<= 15; result |= rand();
    result <<= 15; result |= rand();
    return result; //60 bits random;
}

Tree* createRandomSurface(Plane plane, int pointsCount) {
    if(pointsCount < 1) return nullptr;
    std::vector<Vertex> vertices(pointsCount);
    for(int i = 0; i < pointsCount; ++i) {
        Vertex vertex;
        vertex.x = bigRand() * 1.0 / MAX_BIG_RAND_VALUE * 10.0;
        vertex.y = bigRand() * 1.0 / MAX_BIG_RAND_VALUE * 10.0;
        vertex.z = -(plane.D + plane.A * vertex.x + plane.B * vertex.y) / plane.C;
        vertices[i] = vertex;
    }
    VolumePolygon** P = new VolumePolygon*[1];
    P[0] = new VolumePolygon(std::move(vertices));
    Polyhedron* polyhedron = new Polyhedron(P, 1);
    Tree* tree = new Tree();
    tree->addPolyhedron(polyhedron);
    return tree;
}

}
