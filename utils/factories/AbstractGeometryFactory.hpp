#ifndef ABSTRACTENTITYFACTORY_H
#define ABSTRACTENTITYFACTORY_H

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>

#include "geometry/Polyhedron.h"

namespace bstu {
using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;
class AbstractGeometryFactory {
public:
    virtual ~AbstractGeometryFactory() {}
    virtual QGeometryRenderer* create(Polyhedron* polyhedron) = 0;
};
}

#endif // ABSTRACTENTITYFACTORY_H
