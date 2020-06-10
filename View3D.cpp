#include "View3D.h"

namespace bstu {


View3D::View3D(QWidget* parent)
{
    registerAspect(new Qt3DInput::QInputAspect);
    defaultFrameGraph()->setClearColor(Qt::black);

    _rootEntity = new Qt3DCore::QEntity;
    setRootEntity(_rootEntity);
}

QEntity* View3D::rootEntity() {
    return _rootEntity;
}

}
