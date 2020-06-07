#include <QApplication>

#include "View3D.h"
#include "EntitiesContainer.h"
#include "PolyhedronTreeView.h"

#include "collections/TransformsSet.h"

#include "initializers/CameraAndLightInitalizer.h"
#include "initializers/SelectorInitializer.h"

#include "utils/factories/imp/SimpleGeometryFactory.h"
#include "utils/factories/imp/SimpleMaterialFactory.h"
#include "utils/factories/imp/SimpleTransformFactory.h"
#include "utils/factories/imp/MonotoneMethod_GeometryFactory.h"

#include "utils/Adapter.h"
#include "utils/Activator.h"
#include "utils/Centering.hpp"
#include "utils/Cleaner.hpp"
#include "utils/EntityConstructor.h"

namespace bstu {
    Tree* create_box();
    Tree* create_greenhouse(int n);

    Tree* create_star(int n);

    typedef Vertex Convertor(Vertex);
    Vertex simpleConvertor(Vertex);
    Tree* test_ComplesSurface_1(Convertor = simpleConvertor);
    Tree* test_ComplesSurface_2(Convertor = simpleConvertor);
    Tree* test_ComplesSurface_3(Convertor = simpleConvertor);
    Tree* test_ComplesSurface_4(Convertor = simpleConvertor);
    Tree* test_ComplesSurface_5(Convertor = simpleConvertor);
}

using namespace bstu;
int main(int argc, char *argv[])
{
    /// QObject объекты не надо удалять, они сами удалятся при завершении процесса
    QApplication a(argc, argv);

    srand(GetCurrentTime());

    Tree* tree =
            //test_ComplesSurface_1(); //Работает
            //test_ComplesSurface_2(); //Работает (повернуть на 180 градусов)
            //test_ComplesSurface_3(); // Работает
            //test_ComplesSurface_4(); //Работает (повернуть на 180 градусов)
            //test_ComplesSurface_5(); //Работает
            //create_box(); // Работает
            create_greenhouse(10); // Работает
            //create_star(4); //Работает

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

    AbstractGeometryFactory* geometryFactory =
            //new SimpleGeometryFactory();
            new MonotoneMethod_GeometryFactory();
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
    /// Отвечает за удаление ненужных сущностей
    new Cleaner(mainContainer);

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

Vertex simpleConvertor(Vertex sourse) {
    return sourse;
}

Tree* test_ComplesSurface_1(Convertor convertor) {
    int vertexCount = 15;
    Vertex* vertexes = new Vertex[vertexCount];
    vertexes[0].x = 15;   vertexes[0].y = 8;
    vertexes[1].x = 13;   vertexes[1].y = 9;
    vertexes[2].x = 12;   vertexes[2].y = 15;
    vertexes[3].x = 9;    vertexes[3].y = 13;
    vertexes[4].x = 7;    vertexes[4].y = 14;
    vertexes[5].x = 3;    vertexes[5].y = 12;
    vertexes[6].x = 6;    vertexes[6].y = 11;
    vertexes[7].x = 5;    vertexes[7].y = 7;
    vertexes[8].x = 2;    vertexes[8].y = 10;
    vertexes[9].x = 1;    vertexes[9].y = 4;
    vertexes[10].x = 4;   vertexes[10].y = 2;
    vertexes[11].x = 8;   vertexes[11].y = 3;
    vertexes[12].x = 11;  vertexes[12].y = 1;
    vertexes[13].x = 10;  vertexes[13].y = 6;
    vertexes[14].x = 14;  vertexes[14].y = 5;

    for(int i = 0; i != vertexCount; ++i) {
        vertexes[i].z = 0;
        vertexes[i] = convertor(vertexes[i]);
    }

    int polygonsCount = 1;
    VolumePolygon** polygons = new VolumePolygon*[polygonsCount];
    polygons[0] = new VolumePolygon(vertexes, vertexCount);
    Tree* tree = new Tree();
    tree->addPolyhedron(new Polyhedron(polygons, polygonsCount));
    return tree;
}

Tree* test_ComplesSurface_2(Convertor convertor) {
    int vertexCount = 8;
    Vertex* vertexes = new Vertex[vertexCount];
    vertexes[0].x = 0;   vertexes[0].y = 4;
    vertexes[1].x = 0;   vertexes[1].y = 1;
    vertexes[2].x = 1;   vertexes[2].y = 2;
    vertexes[3].x = 2;   vertexes[3].y = 0;
    vertexes[4].x = 3;   vertexes[4].y = 2;
    vertexes[5].x = 4;   vertexes[5].y = 1;
    vertexes[6].x = 4;   vertexes[6].y = 4;
    vertexes[7].x = 2;   vertexes[7].y = 3;

    for(int i = 0; i != vertexCount; ++i) {
        vertexes[i].z = 0;
        vertexes[i] = convertor(vertexes[i]);
    }

    int polygonsCount = 1;
    VolumePolygon** polygons = new VolumePolygon*[polygonsCount];
    polygons[0] = new VolumePolygon(vertexes, vertexCount);
    Tree* tree = new Tree();
    tree->addPolyhedron(new Polyhedron(polygons, polygonsCount));
    return tree;
}

Tree* test_ComplesSurface_3(Convertor convertor) {
    int vertexCount = 5;
    Vertex* vertexes = new Vertex[vertexCount];
    vertexes[0].x = 0;   vertexes[0].y = 7;
    vertexes[1].x = 3;   vertexes[1].y = 5;
    vertexes[2].x = 3;   vertexes[2].y = 0;
    vertexes[3].x = 2;   vertexes[3].y = 3;
    vertexes[4].x = 0;   vertexes[4].y = 2;

    for(int i = 0; i != vertexCount; ++i) {
        vertexes[i].z = 0;
        vertexes[i] = convertor(vertexes[i]);
    }

    int polygonsCount = 1;
    VolumePolygon** polygons = new VolumePolygon*[polygonsCount];
    polygons[0] = new VolumePolygon(vertexes, vertexCount);
    Tree* tree = new Tree();
    tree->addPolyhedron(new Polyhedron(polygons, polygonsCount));
    return tree;
}

Tree* test_ComplesSurface_4(Convertor convertor) {
    int vertexCount = 8;
    Vertex* vertexes = new Vertex[vertexCount];
    vertexes[0].x = 0;   vertexes[0].y = 0;
    vertexes[1].x = 3;   vertexes[1].y = 0;
    vertexes[2].x = 2;   vertexes[2].y = 1;
    vertexes[3].x = 4;   vertexes[3].y = 2;
    vertexes[4].x = 2;   vertexes[4].y = 3;
    vertexes[5].x = 3;   vertexes[5].y = 4;
    vertexes[6].x = 0;   vertexes[6].y = 4;
    vertexes[7].x = 1;   vertexes[7].y = 2;

    for(int i = 0; i != vertexCount; ++i) {
        vertexes[i].z = 0;
        vertexes[i] = convertor(vertexes[i]);
    }

    int polygonsCount = 1;
    VolumePolygon** polygons = new VolumePolygon*[polygonsCount];
    polygons[0] = new VolumePolygon(vertexes, vertexCount);
    Tree* tree = new Tree();
    tree->addPolyhedron(new Polyhedron(polygons, polygonsCount));
    return tree;
}

Tree* test_ComplesSurface_5(Convertor convertor) {
    int vertexCount = 8;
    Vertex* vertexes = new Vertex[vertexCount];
    vertexes[0].x = 3;    vertexes[0].y = 0;
    vertexes[1].x = 1;    vertexes[1].y = 1;
    vertexes[2].x = 0;    vertexes[2].y = 3;
    vertexes[3].x = -1;   vertexes[3].y = 1;
    vertexes[4].x = -3;   vertexes[4].y = 0;
    vertexes[5].x = -1;   vertexes[5].y = -1;
    vertexes[6].x = 0;    vertexes[6].y = -3;
    vertexes[7].x = 1;    vertexes[7].y = -1;

    for(int i = 0; i != vertexCount; ++i) {
        vertexes[i].z = 0;
        vertexes[i] = convertor(vertexes[i]);
    }

    int polygonsCount = 1;
    VolumePolygon** polygons = new VolumePolygon*[polygonsCount];
    polygons[0] = new VolumePolygon(vertexes, vertexCount);
    Tree* tree = new Tree();
    tree->addPolyhedron(new Polyhedron(polygons, polygonsCount));
    return tree;
}

Tree* create_star(int n) {
    if(n < 3) return nullptr;

    int polygonsCount =
            //2
            (n + 1) * 2;
            //2 + n;
    std::vector<Vertex*> arrayPolygonsVertex(polygonsCount);
    arrayPolygonsVertex[0] = new Vertex[n * 2];
    arrayPolygonsVertex[1] = new Vertex[n * 2];
    for(int i = 2; i != polygonsCount; ++i) {
        arrayPolygonsVertex[i] = new Vertex[4];
    }

    double da = 2 * M_PI / n;
    double radius_outerCircle = 4, radius_innerCircle = sqrt(2);
    double angle_outerCircle = 0, angle_innerCircle = da / 2;
    double near_z = 1, distant_z = 3;

    Vertex* nearStarVertexes = arrayPolygonsVertex[0];
    Vertex* distantStarVertexes = arrayPolygonsVertex[1];
    for(int i = 0; i != n; ++i) {
        nearStarVertexes[i * 2].x = radius_outerCircle * sin(angle_outerCircle);
        nearStarVertexes[i * 2].y = radius_outerCircle * cos(angle_outerCircle);
        nearStarVertexes[i * 2].z = near_z;

        nearStarVertexes[i * 2 + 1].x = radius_innerCircle * sin(angle_innerCircle);
        nearStarVertexes[i * 2 + 1].y = radius_innerCircle * cos(angle_innerCircle);
        nearStarVertexes[i * 2 + 1].z = near_z;

        distantStarVertexes[i * 2].x = nearStarVertexes[i * 2].x;
        distantStarVertexes[i * 2].y = nearStarVertexes[i * 2].y;
        distantStarVertexes[i * 2].z = distant_z;

        distantStarVertexes[i * 2 + 1].x = nearStarVertexes[i * 2 + 1].x;
        distantStarVertexes[i * 2 + 1].y = nearStarVertexes[i * 2 + 1].y;
        distantStarVertexes[i * 2 + 1].z = distant_z;


        Vertex* connectingPolygon = arrayPolygonsVertex[2 + i * 2];
        connectingPolygon[0] = nearStarVertexes[i * 2];
        connectingPolygon[1] = nearStarVertexes[i * 2 + 1];
        connectingPolygon[2] = distantStarVertexes[i * 2 + 1];
        connectingPolygon[3] = distantStarVertexes[i * 2];

        angle_outerCircle += da;
        angle_innerCircle += da;
    }

    Vertex* lastConnectingPolygon = arrayPolygonsVertex.back();
    lastConnectingPolygon[0] = distantStarVertexes[n * 2 - 1];
    lastConnectingPolygon[1] = nearStarVertexes[n * 2 - 1];
    lastConnectingPolygon[2] = nearStarVertexes[0];
    lastConnectingPolygon[3] = distantStarVertexes[0];

    for(int i = 0; i != n - 1; ++i) {
        Vertex* connectingPolygon = arrayPolygonsVertex[3 + i * 2];
        connectingPolygon[0] = distantStarVertexes[i * 2 + 1];
        connectingPolygon[1] = nearStarVertexes[i * 2 + 1];
        connectingPolygon[2] = nearStarVertexes[i * 2 + 2];
        connectingPolygon[3] = distantStarVertexes[i * 2 + 2];
    }

    VolumePolygon** polygons = new VolumePolygon*[polygonsCount];
    polygons[0] = new VolumePolygon(arrayPolygonsVertex[0], n * 2);
    polygons[1] = new VolumePolygon(arrayPolygonsVertex[1], n * 2);
    for(unsigned i = 2; i != arrayPolygonsVertex.size(); ++i)
        polygons[i] = new VolumePolygon(arrayPolygonsVertex[i], 4);

    Tree* tree = new Tree();
    tree->addPolyhedron(new Polyhedron(polygons, polygonsCount));
    return tree;
}

}
