#ifndef SMARTCOMPONENT_H
#define SMARTCOMPONENT_H

#include <Qt3DCore>

#include "extensions/PolyhedronExtension.h"

namespace bstu {
class SmartComponent : public Qt3DCore::QComponent {
    Q_OBJECT
public:
    SmartComponent(PolyhedronExtension* polyhedron);
    PolyhedronExtension* polyhedron();
private:
    PolyhedronExtension* _polyhedron;
};


}

#endif // SMARTCOMPONENT_H
