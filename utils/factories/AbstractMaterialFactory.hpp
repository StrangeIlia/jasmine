#ifndef ABSTRACTMATERIALFACTORY_H
#define ABSTRACTMATERIALFACTORY_H

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>

#include "geometry/Polyhedron.h"

namespace bstu {
using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;
class AbstractMaterialFactory {
public:
    virtual ~AbstractMaterialFactory() {}
    virtual QMaterial* create(Polyhedron* polyhedron) = 0;
};
}

#endif // ABSTRACTMATERIALFACTORY_H
