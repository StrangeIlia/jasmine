#include "CameraControllerFactory_MyOrbitCamera.h"

namespace bstu{
CameraControllerFactory_MyOrbitCamera::CameraControllerFactory_MyOrbitCamera()
{

}

QAbstractCameraController* CameraControllerFactory_MyOrbitCamera::createCameraController(Qt3DRender::QCamera* camera) const
{
    MyOrbitCameraController* cameraController = new MyOrbitCameraController();
    cameraController->setCamera(camera);
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0.0f, 0.0f, -5.0f));
    camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
    return cameraController;
}

}
