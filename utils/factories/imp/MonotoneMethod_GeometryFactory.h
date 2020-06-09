#ifndef DELONEGEOMETRYFACTORY_H
#define DELONEGEOMETRYFACTORY_H

#include "exceptions/NotImplementedException.hpp"
#include "utils/factories/AbstractGeometryFactory.hpp"

#include <QLinkedList>

namespace bstu {
/// Используется алгоритм Делоне с динамическим кешированием и предварительным вычислением радиуса
class MonotoneMethod_GeometryFactory : public AbstractGeometryFactory
{
public:
    QGeometryRenderer* create(Polyhedron* polyhedron) override;
private:
    static void createVertexAttribute(QGeometry* geometry, Polyhedron* polyhedron);
    static void createIndexesAttribute(QGeometry* geometry, Polyhedron* polyhedron);
};
}


#endif // DELONEGEOMETRYFACTORY_H
