#ifndef IQENTITYFACTORY_H
#define IQENTITYFACTORY_H

#include <Qt3DRender>

namespace bstu {

using namespace Qt3DRender;

/// Класс занимается созданием геомертией объектов
class IGeometryRendererFactory
{
public:
    IGeometryRendererFactory();
    virtual ~IGeometryRendererFactory();

    virtual QGeometryRenderer* createGeometryRenderer() const = 0;
};

}

#endif // IQENTITYFACTORY_H
