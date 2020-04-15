#include "CameraAndLightInitalizer.h"

namespace bstu {
CameraAndLightInitalizer::CameraAndLightInitalizer(View3D* view) : AbstractViewInitializer(view) {
}

void CameraAndLightInitalizer::init(){
    if(view() == nullptr)
        return;

    QCamera *camera = view()->camera();
    _transform = new Qt3DCore::QTransform();
    _controller = new MyOrbitCameraController(view()->rootEntity());
    _lightEntity = new Qt3DCore::QEntity(view()->rootEntity());
    _light = new Qt3DRender::QPointLight(_lightEntity);

    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0.0f, 0.0f, -5.0f));
    camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));

    _transform->setShareable(true);

    _controller->setCamera(camera);
    _controller->addComponent(_transform);
    _light->setColor("white");
    _light->setIntensity(1);
    _lightEntity->addComponent(_light);
    _lightEntity->addComponent(_transform);
}

Qt3DCore::QTransform* CameraAndLightInitalizer::transform() { return _transform; }
MyOrbitCameraController* CameraAndLightInitalizer::controller() { return _controller; }
Qt3DCore::QEntity* CameraAndLightInitalizer::lightEntity() { return _lightEntity; }
Qt3DRender::QPointLight *CameraAndLightInitalizer::light() { return _light; }

}
