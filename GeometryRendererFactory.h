#ifndef QENTITYFACTORY_POLYHEDRON_H
#define QENTITYFACTORY_POLYHEDRON_H

#include "../geometry/Polyhedron.h"

#include "IGeometryRendererFactory.h"

#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender>

namespace bstu {

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

/// Класс отвечает за создание геометрии сущности на основе полихедрона (дочерние полихедроны игнорируются)
/// Созданный QGeometryRenderer не привязан к какому - то rootEntity
class GeometryRendererFactory : public IGeometryRendererFactory
{
public:
    explicit GeometryRendererFactory(Polyhedron* polyhedron = nullptr);

    void setPolyhedron(Polyhedron* polyhedron);
    virtual QGeometryRenderer* createGeometryRenderer() const;
private:
    Polyhedron* pattern;

    static QGeometry* createGeomenty(Polyhedron* polyhedron);
    static QGeometryRenderer* createGeomentyRenderer(Polyhedron* polyhedron);
    static void createVertexAttribute(QGeometry* geometry, Polyhedron* polyhedron);
    static void createIndexesAttribute(QGeometry* geometry, Polyhedron* polyhedron);
};

}


#endif // QENTITYFACTORY_POLYHEDRON_H
