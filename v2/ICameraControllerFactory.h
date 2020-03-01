#ifndef ICAMERACONTROLLERFACTORY_H
#define ICAMERACONTROLLERFACTORY_H

#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender>

namespace bstu {

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

/// Интерфейс для фабрики контроллера камеры
class ICameraControllerFactory
{
public:
    explicit ICameraControllerFactory();

    virtual QAbstractCameraController* createCameraController(QEntity* root) = 0;
};
}



#endif // ICAMERACONTROLLERFACTORY_H
