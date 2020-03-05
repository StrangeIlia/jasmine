#ifndef IENTITYCONTROLLER_H
#define IENTITYCONTROLLER_H

#include <QHash>
#include <QQueue>
#include <QObject>


#include <limits>


#include "View3D.h"
#include "GeometryRendererFactory_Polyhedron.h"


#include "../geometry/Polyhedron.h"


namespace bstu {

using namespace Qt3DCore;
using namespace Qt3DExtras;

/// Класс отвечает за добавление/удаление/отображение сущностей на сцене
class EntityController_Polyhedron : public QObject
{
    Q_OBJECT
public:
    explicit EntityController_Polyhedron(View3D *parent);
    ~EntityController_Polyhedron();

signals:

public slots:
    void addEntity(Polyhedron* polyhedron);
    void removeEntity(Polyhedron* polyhedron);

private:
    /// Возвращает истина, если в списке активных полихедронов
    /// есть старший родственник данного полихедрона
    bool hasParrent(Polyhedron* polyhedron);
    static bool hasParrent(Polyhedron* parent, Polyhedron* polyhedron);

    /// Активируем или отключаем дочерние элементы (для повышение производительности)
    void changeStateChilds(Polyhedron* polyhedron, bool state);

    QEntity* surrogateObject;
    QPhongAlphaMaterial* material;
    Qt3DCore::QTransform* transform;
    GeometryRendererFactory_Polyhedron* factory;
    QHash<Polyhedron*, QEntity*> activePolyhedrons;


    struct MinMaxValue{
        //Определяет, какому полихедрону принадлежит минимум или максимум
        Polyhedron* polyhedron;
        float value;
    };

    MinMaxValue min_x, max_x;
    MinMaxValue min_y, max_y;
    MinMaxValue min_z, max_z;

    /// Инициализирует крайние точки
    void minMaxPointsInit();
    /// Центровка при добавлении нового полихедрона
    void centeringOnAppend(Polyhedron* polyhedron);
    /// Центровка при удалении полихедрона
    void centeringOnDelete(Polyhedron* polyhedron);

    void changeTransform();
};
}


#endif // IENTITYCONTROLLER_H
