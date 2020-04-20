#ifndef CAMERAANDLIGHTINITALIZER_H
#define CAMERAANDLIGHTINITALIZER_H

#include "initializers/AbstractViewInitializer.hpp"
#include "MyOrbitCameraController.h"

namespace bstu {
class CameraAndLightInitalizer : public AbstractViewInitializer
{
public:
    CameraAndLightInitalizer(View3D* view);
    Qt3DCore::QTransform* transform();
    MyOrbitCameraController* controller();
    Qt3DCore::QEntity *lightEntity();
    Qt3DRender::QPointLight *light();
protected:
     void init() override;
private:
    View3D* _view = nullptr;
    Qt3DCore::QTransform* _transform = nullptr;
    MyOrbitCameraController* _controller = nullptr;
    Qt3DCore::QEntity *_lightEntity = nullptr;
    Qt3DRender::QPointLight *_light = nullptr;
};

}

#endif // CAMERAANDLIGHTINITALIZER_H
