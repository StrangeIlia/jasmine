#include "MyOrbitCameraController.h"

namespace bstu
{

MyOrbitCameraController::MyOrbitCameraController(Qt3DCore::QNode *parent) :
    QAbstractCameraController(parent),
    _zoomInLimit(2.0f)
{

}


MyOrbitCameraController::~MyOrbitCameraController()
{

}

float MyOrbitCameraController::zoomInLimit() const
{
    return this->_zoomInLimit;
}

void MyOrbitCameraController::setZoomInLimit(float zoomInLimit)
{
    this->_zoomInLimit = zoomInLimit;
}

inline float zoomDistance(QVector3D firstPoint, QVector3D secondPoint)
{
    return (secondPoint - firstPoint).lengthSquared();
}

void MyOrbitCameraController::moveCamera(const QAbstractCameraController::InputState &state, float dt)
{
    Qt3DRender::QCamera *camera = this->camera();
    if (camera == nullptr) return;

    if(state.leftMouseButtonActive || state.rightMouseButtonActive)
    {
        camera->panAboutViewCenter((state.rxAxisValue * lookSpeed()) * dt, camera->upVector());
        camera->tiltAboutViewCenter((state.ryAxisValue * lookSpeed()) * dt);
    }

    if (!state.altKeyActive && !state.shiftKeyActive)
    {
        if (state.tzAxisValue < 0 || zoomDistance(camera->position(), camera->viewCenter()) > _zoomInLimit * _zoomInLimit)
            camera->translate(QVector3D(0, 0, state.tzAxisValue * linearSpeed()) * dt, camera->DontTranslateViewCenter);
    }
}

}

