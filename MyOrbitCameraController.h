#ifndef MYORBITCAMERACONTROLLER_H
#define MYORBITCAMERACONTROLLER_H

#include <QWidget>
#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender>

namespace bstu
{

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

class MyOrbitCameraController : public QAbstractCameraController
{
public:
    explicit MyOrbitCameraController(Qt3DCore::QNode *parent = nullptr);
    ~MyOrbitCameraController();

    float zoomInLimit() const;
    void setZoomInLimit(float zoomInLimit);

private:
    void moveCamera(const QAbstractCameraController::InputState &state, float dt) override;

    float _zoomInLimit;
};

}



#endif // MYORBITCAMERACONTROLLER_H
