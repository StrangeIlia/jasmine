#ifndef ICAMERACONTROLLERFACTORY_H
#define ICAMERACONTROLLERFACTORY_H

#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender>

namespace bstu {

using namespace Qt3DExtras;

/// Интерфейс для фабрики контроллера камеры
class ICameraControllerFactory
{
public:
    explicit ICameraControllerFactory();

    /// Настраивает камеру и создает для нее контроллер
    virtual QAbstractCameraController* createCameraController(Qt3DRender::QCamera* camera) const = 0;
};
}



#endif // ICAMERACONTROLLERFACTORY_H
