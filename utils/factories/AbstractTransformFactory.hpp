#ifndef ABSTRACTTRANSFORMFACTORY_H
#define ABSTRACTTRANSFORMFACTORY_H

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>

#include "geometry/Polyhedron.h"

namespace bstu {
using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;
class AbstractTransformFactory {
public:
    virtual ~AbstractTransformFactory() {}
    virtual Qt3DCore::QTransform* create(Polyhedron* polyhedron) = 0;
};
}

#endif // ABSTRACTTRANSFORMFACTORY_H
