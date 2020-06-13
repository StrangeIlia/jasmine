#include "utils/SmartComponent.h"

namespace bstu{
SmartComponent::SmartComponent(PolyhedronExtension* polyhedron) : _polyhedron(polyhedron) {}
PolyhedronExtension* SmartComponent::polyhedron() {return _polyhedron;}
}
