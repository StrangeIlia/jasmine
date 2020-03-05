#ifndef CAMERACONTROLLERFACTORY_MYORBITCAMERA_H
#define CAMERACONTROLLERFACTORY_MYORBITCAMERA_H

#include "ICameraControllerFactory.h"

#include "../MyOrbitCameraController_Qt3D.h"

namespace bstu {
class CameraControllerFactory_MyOrbitCamera : public ICameraControllerFactory
{
public:
    CameraControllerFactory_MyOrbitCamera();

    virtual QAbstractCameraController* createCameraController(Qt3DRender::QCamera* camera) const;
};
}



#endif // CAMERACONTROLLERFACTORY_MYORBITCAMERA_H
