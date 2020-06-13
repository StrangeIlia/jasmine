#ifndef POINTSGEOMETRYFACTORY_H
#define POINTSGEOMETRYFACTORY_H

#include "utils/factories/AbstractGeometryFactory.hpp"
namespace bstu {
class PointsGeometryFactory : public AbstractGeometryFactory
{
public:
    QGeometryRenderer* create(Polyhedron* polyhedron) override;
private:
    static void createVertexAttribute(QGeometry* geometry, Polyhedron* polyhedron);
    static void createIndexesAttribute(QGeometry* geometry, Polyhedron* polyhedron);
};
}


#endif // POINTSGEOMETRYFACTORY_H
