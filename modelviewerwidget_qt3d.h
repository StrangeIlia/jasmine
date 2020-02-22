#ifndef MODELVIEWERWIDGET_QT3D_H
#define MODELVIEWERWIDGET_QT3D_H

#include <QWidget>
#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender>
#include "geometry/Tree.h"
#include <QVector>

namespace bstu {

using namespace Qt3DCore;
using namespace Qt3DInput;
using namespace Qt3DRender;
using namespace Qt3DExtras;

class ModelViewerWidget_Qt3D : private Qt3DWindow
{
    Q_OBJECT
public:
    ModelViewerWidget_Qt3D(QWidget* parent = nullptr);
    ~ModelViewerWidget_Qt3D();


    void setTree(Tree* tree);

    QWidget* container();
private:
    void clearEntities();
    void clearSourcesOfLight();

    QGeometry* createGeomenty(Polyhedron* polyhedron);
    QGeometryRenderer* createGeomentyRenderer(Polyhedron* polyhedron);

    void appendGeomentyRenderer(QEntity* entity, Polyhedron* polyhedron);

    static void createVertexAttribute(QGeometry* geometry, Polyhedron* polyhedron);
    static void createIndexesAttribute(QGeometry* geometry, Polyhedron* polyhedron);

    /*
    void appendGeomentyRenderer(QEntity* entity, Polyhedron* polyhedron);

    QGeometry* createGeomenty(Polygon* polygon);
    QGeometryRenderer* createGeomentyRenderer(Polygon* polygon);
    static void appendPositionAttribute(QGeometry* geometry, Polygon* polygon);
    static void appendIndexesAttribute(QGeometry* geometry, Polygon* polygon);
    */

    QVector<QEntity*> _entities;
    QWidget* _container = nullptr;
    QEntity* _rootEntity = nullptr;
    QVector<QAbstractLight*> _sourcesOfLight;
    QAbstractCameraController* _cameraController = nullptr;
};

}

#endif // MODELVIEWERWIDGET_QT3D_H
