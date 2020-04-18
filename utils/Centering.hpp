#ifndef CENTERING_H
#define CENTERING_H

#include "collections/TransformsSet.h"
#include "collections/AbstractEntityMap.hpp"

#include <QSet>
#include <limits>

namespace bstu {
class Centering : QObject
{
    Q_OBJECT
public:
    Centering(AbstractEntityMap* map, Enumerable<Qt3DCore::QTransform*> set);
private slots:
    void appended(EntityPair pair);
    void changed(EntityPair oldPair, QEntity* newEntity);
    void removed(EntityPair pair);
private:
    void initVectors();
    void changePositions(Vector dVector);
    struct BoolPair {
        bool x = false, y = false, z = false;
        operator bool () { return x || y || z; }
    };

    struct PolyhedronPair {
        Polyhedron *x = nullptr, *y = nullptr, *z = nullptr;
        bool has(Polyhedron *v) {return v == x || v == y || v == z;}
    };

    static BoolPair minmax(Vertex& a, Vertex& b);

    static Vector minus(const Vector& x, const Vector& y);

    Enumerable<Qt3DCore::QTransform*> set;
    AbstractEntityMap* map;
    Vertex minVector, maxVector;
    PolyhedronPair minPol, maxPol;
};

}

#endif // CENTERING_H
