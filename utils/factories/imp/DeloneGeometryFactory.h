#ifndef DELONEGEOMETRYFACTORY_H
#define DELONEGEOMETRYFACTORY_H

#include "exceptions/NotImplementedException.hpp"
#include "utils/factories/imp/SimpleGeometryFactory.h"

#include <QMap>

namespace bstu {
/// Используется алгоритм Делоне с динамическим кешированием и предварительным вычислением радиуса
class DeloneGeometryFactory : public SimpleGeometryFactory
{
public:
    QGeometryRenderer* create(Polyhedron* polyhedron) override;
private:
    static void createIndexesAttribute(QGeometry* geometry, Polyhedron* polyhedron);
};
}


#endif // DELONEGEOMETRYFACTORY_H
