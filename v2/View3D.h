#ifndef VIEW3D_H
#define VIEW3D_H

#include <QVector>
#include <QWidget>

#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender>

#include "ICameraControllerFactory.h"

namespace bstu {

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

/// Класс для отображения объектов
class View3D : public Qt3DWindow
{
    Q_OBJECT
public:
    explicit View3D(QWidget* parent = nullptr);
    ~View3D();

    QWidget* container();

    void addEntity(QEntity* entity);
    void removeEntity(QEntity* entity);

    void addComponent(QComponent *comp);
    void removeComponent(QComponent *comp);

    void setCameraController(ICameraControllerFactory* factory);

    QComponentVector components() const;
    template<class T> QVector<T*> componentsOfType() const;
private:
    QWidget* _container = nullptr;
    QEntity* _rootEntity = nullptr;
    QAbstractCameraController* _cameraController = nullptr;
};

}

#endif // VIEW3D_H
