#ifndef MODELVIEWERWIDGET_QT3D_H
#define MODELVIEWERWIDGET_QT3D_H

#include <QWidget>
#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <QMouseEvent>
#include "geometry/Tree.h"
#include <QVector>

#include <limits>
#include "MyOrbitCameraController_Qt3D.h"

namespace bstu {

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

class ModelViewerWidget_Qt3D : public Qt3DWindow
{
    Q_OBJECT
public:
    ModelViewerWidget_Qt3D(QWidget* parent = nullptr);
    ~ModelViewerWidget_Qt3D();


    void setTree(Tree* tree, bool entitiesEnabled = true);

    // Напрямую сущность Qt3DWindow нельзя отображать
    // Можно отобразить только контейнер
    QWidget* container();
    // Список всех сущностей
    const QVector<QEntity*>& entities();   
    // Цвет выделяемой сущности
    QPhongAlphaMaterial* selectedObjectMaterial();

protected:
    // Обрабатываем события на выбор сущности
    virtual void mousePressEvent(QMouseEvent* event);

private slots:
    // Обработка поподания луча в сущность
    void hitsEvent(const Qt3DRender::QAbstractRayCaster::Hits hits);

private:
    // Удаление всех сущностей
    void clearEntities();
    // Удаление всех источников света
    void clearSourcesOfLight();

    // Создание абстрактной геометрии
    QGeometry* createGeomenty(Polyhedron* polyhedron);
    // Создание "отобразителя" геометрии
    QGeometryRenderer* createGeomentyRenderer(Polyhedron* polyhedron);

    // Добавление сущности геометрии
    void appendGeomentyRenderer(QEntity* entity, Polyhedron* polyhedron);

    // Заполнение информации о Vertex
    static void createVertexAttribute(QGeometry* geometry, Polyhedron* polyhedron);
    // Заполение информации о позициях Vertex
    static void createIndexesAttribute(QGeometry* geometry, Polyhedron* polyhedron);

    // Родительский контейнер
    QWidget* _container = nullptr;

    // Список всех сущностей (не обязательно, но проще обращение)
    QVector<QEntity*> _entities;
    // Корневая сущность
    QEntity* _rootEntity = nullptr;
    // Список всех источников света (не обязательно, но проще обращение)
    QVector<QAbstractLight*> _sourcesOfLight;

    // Контроллер камеры (отвечает за поведение камеры)
    QAbstractCameraController* _cameraController = nullptr;

    // Выбранная сущность
    QEntity* _selectedEntity = nullptr;
    // Цвет выделения сущности
    QPhongAlphaMaterial* _selectedObjectMaterial;
    // Цвет сущности до выделения
    QPhongAlphaMaterial* _oldSelectedObjectMaterial = nullptr;
    // Ищет пересечения сущностей с лучом
    QScreenRayCaster* _screenRayCaster;
};

}

#endif // MODELVIEWERWIDGET_QT3D_H
