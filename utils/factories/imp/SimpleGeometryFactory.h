#ifndef SIMPLEENTITYFACTORY_H
#define SIMPLEENTITYFACTORY_H

#include "utils/factories/AbstractGeometryFactory.hpp"

#define DEBUG 1

namespace bstu {
class SimpleGeometryFactory : public AbstractGeometryFactory
{
public:
    QGeometryRenderer* create(Polyhedron* polyhedron) override;
protected:
    static void createVertexAttribute(QGeometry* geometry, Polyhedron* polyhedron);
    static void createIndexesAttribute(QGeometry* geometry, Polyhedron* polyhedron);
};
}

#endif // SIMPLEENTITYFACTORY_H
